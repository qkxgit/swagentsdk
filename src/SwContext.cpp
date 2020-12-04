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

void SwContext::Initialize(SwAgent* ag)
{
	agent = ag;
	assert(agent != NULL);
}

void SwContext::DestroyContext()
{
	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	threadCtxMap.erase(id);
}

SwContext& SwContext::GetContext()
{
	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	return threadCtxMap[id];
}

SwSpan* SwContext::CreateLocalSpan(const std::string& operationName)
{
	return new SwSpan(*this, operationName);
}

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

SwSnapshot SwContext::Capture()
{
	SwSpan* span = ActiveSpan();
	if (span)
		return SwSnapshot(segment.relatedTraceIds[0], segment.segmentId, span->GetSpanId(),
			span->GetOperationName(), agent->GetService(), agent->GetServiceInstance(), correlation);
	return SwSnapshot();
}

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

SwSpan* SwContext::ActiveSpan()
{
	if (spans.empty())
		return NULL;
	return spans[spans.size() - 1];
}

void SwContext::Start(SwSpan* span)
{
	if (span && std::find(spans.begin(), spans.end(), span) == spans.end())
		spans.push_back(span);
}

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

bool SwContext::IsFromCurrent(const SwSnapshot& snapshot)
{
	return !snapshot.dat.segmentId.empty() && snapshot.dat.segmentId == Capture().dat.segmentId;
}
