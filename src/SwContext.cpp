/*
用于存储线程的上下文信息，并提供创建span、获取span快照和关联span等操作
date:2020/12/08
author:qkx
*/
#include <stdio.h>
#include "SwContext.h"
#include "SwSpan.h"
#include "SwAgent.h"

#include <assert.h>
#include "thread/KPthread.h"
#include "thread/KMutex.h"

#include "SwSnapshot.h"
#include "SwCarrier.h"

static klib::KMutex ctxMtx;
static std::map<uint32_t, SwContext> threadCtxMap;

SwContext::SwContext()
	:sid(-1),depth(0), agent(NULL)
{
	Initialize(&AgentInst::GetRef());
}

SwContext::~SwContext()
{

}

// 设置agent
// agent
void SwContext::Initialize(SwAgent* ag)
{
	agent = ag;
	assert(agent != NULL);
}

// 线程退出时销毁context
void SwContext::DestroyContext()
{
	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	threadCtxMap.erase(id);
}

// 获取当前线程的SwContext
SwContext& SwContext::GetContext()
{
	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	return threadCtxMap[id];
}

// 创建localspan
// operationName 操作名称
// 返回span
SwSpan* SwContext::CreateLocalSpan(const std::string& operationName)
{
	return new SwSpan(*this, operationName);
}

// 创建entryspan
// operationName 操作名称
// carrier 上一个span的上下文信息
// 返回span
SwSpan* SwContext::CreateEntrySpan(const std::string& operationName, const SwCarrier& carrier)
{
	SwSpan* entrySpan = NULL;
	SwSpan* parentSpan = ActiveSpan();
	if (parentSpan && parentSpan->IsEntry())
		entrySpan = parentSpan;
	else
		entrySpan = new SwEntrySpan(*this, operationName);
	if (carrier.IsValid())
		entrySpan->Extract(carrier);
	return entrySpan;
}

// 创建exitspan
// operationName 操作名称
// peer 
// 返回span
SwSpan* SwContext::CreateExitSpan(const std::string& operationName, const std::string& peer)
{
	SwSpan* exitSpan = NULL;
	SwSpan* parentSpan = ActiveSpan();
	if (parentSpan && parentSpan->IsExit())
		exitSpan = parentSpan;
	else
		exitSpan = new SwExitSpan(*this, operationName, peer);
	return exitSpan;
}

// 获取当前线程激活的span的快照
SwSnapshot SwContext::Capture()
{
	SwSpan* span = ActiveSpan();
	if (span)
		return SwSnapshot(segment.relatedTraceIds[0], segment.segmentId, span->GetSpanId(),
			span->GetOperationName(), agent->GetService(), agent->GetServiceInstance(), correlation);
	return SwSnapshot();
}

// 关联上span
// snapShot为span的快照
void SwContext::Continued(const SwSnapshot& snapShot)
{
	if (snapShot.IsValid() && !IsFromCurrent(snapShot))
	{
		SwSegmentRef ref(snapShot);
		SwSpan* span = ActiveSpan();
		assert(span != NULL);
		span->AppendRef(ref);
		segment.Relate(ref.dat.traceId);
		correlation = snapShot.dat.correlation;
	}
}

// 当前线程激活的span
// 返回span
SwSpan* SwContext::ActiveSpan()
{
	if (spans.empty())
		return NULL;
	return spans[spans.size() - 1];
}

SwAgent* SwContext::GetAgent()
{
	return agent;
}

// 启动context
// span 激活的span，放入激活容器中
void SwContext::Start(SwSpan* span)
{
	if (span && std::find(spans.begin(), spans.end(), span) == spans.end())
		spans.push_back(span);
}

// 停止contex
// 结束span，如果激活容器中空了则提交segment数据重置SwContext并返回成功，否则不提交并返回失败
bool SwContext::Stop(SwSpan* span)
{
	assert(agent != NULL);
	std::vector<SwSpan*>::iterator it = std::find(spans.begin(), spans.end(), span);
	assert(it != spans.end());
	if (span->Finish())
		spans.erase(it);

	if (spans.empty())
	{
		if (!agent->Commit(segment))
		{
			std::vector<SwSpan*>::iterator it = segment.spans.begin();
			while (it != segment.spans.end())
			{
				SwSpan* span = *it;
				delete span;
				++it;
			}
			printf("Commit segment failed\n");
		}
		depth = 0;
		sid = -1;
		correlation.clear();
		segment = SwSegment();
		return true;
	}
	return false;
}

// 判断snapshot是否属于当前上下文
bool SwContext::IsFromCurrent(const SwSnapshot& snapshot)
{
	return !snapshot.dat.segmentId.empty() && snapshot.dat.segmentId == Capture().dat.segmentId;
}
