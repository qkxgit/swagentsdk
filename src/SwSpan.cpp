/*
span类，提供span相关的操作，包括启动、停止、context解析注入、设置标签、日志等
date:2020/12/08
author:qkx
*/
#include "SwSpan.h"
#include "SwContext.h"
#include "SwAgent.h"
#include "util/KTime.h"
SwSpan::SwSpan(SwContext& ctx, const std::string& op, SwEnumSpanKind k) 
	:ctx(ctx), operationName(op), kind(k),component(rpc), layer(RPCFramework),
	errorOcurred(false),startTime(0),endTime(0)
{
	spanId = ++ctx.sid;
	SwSpan* span = ctx.ActiveSpan();
	parentSpanId = (span ? span->spanId : -1);
}

// 启动span
void SwSpan::Start()
{
	klib::KTime::NowMillisecond(startTime);
	ctx.Start(this);
}

// 停止span，成功返回true，失败返回false
bool SwSpan::Stop()
{
	return ctx.Stop(this);
}

// 结束span，返回true
bool SwSpan::Finish()
{
	klib::KTime::NowMillisecond(endTime);
	ctx.segment.Archive(this);
	return true;
}

// 追加日志
void SwSpan::AppendLog(const std::string& key, const std::string& value)
{
	SwLog lg;
	klib::KTime::NowMillisecond(lg.timestamp);
	lg.kvMap[key] = value;
	logs.push_back(lg);
}

// 调用报错时，设置错误信息
// key：错误标题
// value：具体错误内容
void SwSpan::SetError(const std::string& key, const std::string& value)
{
	errorOcurred = true; AppendLog(key, value);
}

// 设置传递过来的上下文信息，carrier上下文信息载体
void SwSpan::Extract(const SwCarrier& carrier)
{
	if (carrier.IsValid())
	{
		ctx.correlation = carrier.dat.correlation;
		ctx.segment.Relate(carrier.dat.traceId);
	}
}

SwStackedSpan::SwStackedSpan(SwContext& ctx, SwEnumSpanKind k, const std::string& op, const std::string& p)
	:SwSpan(ctx, op, k),depth(0)
{
	peer = p;
}

SwEntrySpan::SwEntrySpan(SwContext& ctx, const std::string& op) 
	:SwStackedSpan(ctx, Entry, op, std::string())
{

}

// 启动span
void SwEntrySpan::Start()
{
	depth = ++ctx.depth;
	if (depth == 1)
		SwStackedSpan::Start();
}

// 结束span
bool SwEntrySpan::Finish()
{
	return (ctx.depth-- == 1) && SwSpan::Finish();
}

// 设置传递过来的上下文信息，carrier上下文信息载体
void SwEntrySpan::Extract(const SwCarrier& carrier)
{
	if (carrier.IsValid())
	{
		SwSpan::Extract(carrier);
		SwSegmentRef ref(carrier);
		if (std::find(refs.begin(), refs.end(), ref) == refs.end())
			refs.push_back(ref);
	}
}

SwExitSpan::SwExitSpan(SwContext& ctx, const std::string& op, const std::string& p)
	:SwStackedSpan(ctx, Exit, op, p)
{
	
}

// 启动span
void SwExitSpan::Start()
{
	depth = ++ctx.depth;
	SwStackedSpan::Start();
}

// 结束span，成功返回true,失败返回false
bool SwExitSpan::Finish()
{
	return (ctx.depth-- == depth) && SwSpan::Finish();
}

// 注入上下文信息，carrier上下文载体
void SwExitSpan::Inject(SwCarrier& carrier)
{
	carrier.dat.traceId = ctx.segment.relatedTraceIds[0];
	carrier.dat.segmentId = ctx.segment.segmentId;
	carrier.dat.spanId = spanId;
	carrier.dat.service = ctx.agent->GetService();//
	carrier.dat.serviceInstance = ctx.agent->GetServiceInstance();//
	carrier.dat.endpoint = operationName;
	carrier.dat.networkAddressUsedAtPeer = peer;
	carrier.dat.correlation = ctx.correlation;
}


