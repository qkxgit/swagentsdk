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

void SwSpan::Start()
{
	klib::KTime::NowMillisecond(startTime);
	ctx.Start(this);
}

bool SwSpan::Stop()
{
	return ctx.Stop(this);
}

bool SwSpan::Finish()
{
	klib::KTime::NowMillisecond(endTime);
	ctx.segment.Archive(this);
	return true;
}

void SwSpan::AppendLog(const std::string& key, const std::string& value)
{
	SwLog lg;
	klib::KTime::NowMillisecond(lg.timestamp);
	lg.kvMap[key] = value;
	logs.push_back(lg);
}

void SwSpan::SetError(const std::string& key, const std::string& value)
{
	errorOcurred = true; AppendLog(key, value);
}

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

void SwEntrySpan::Start()
{
	depth = ++ctx.depth;
	if (depth == 1)
		SwStackedSpan::Start();
}

bool SwEntrySpan::Finish()
{
	return (ctx.depth-- == 1) && SwSpan::Finish();
}

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

void SwExitSpan::Start()
{
	depth = ++ctx.depth;
	SwStackedSpan::Start();
}

bool SwExitSpan::Finish()
{
	return (ctx.depth-- == depth) && SwSpan::Finish();
}

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


