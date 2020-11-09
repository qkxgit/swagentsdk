#include "SwSpan.h"
#include "SwContext.h"
#include "SwAgent.h"
#include "util/KTime.h"
SwSpan::SwSpan(SwContext& ctx, const std::string& op, SwEnumSpanKind k) 
	:ctx(ctx), operationName(op), kind(k),component(rpc), layer(RPCFramework),
	errorOcurred(false),startTime(0),endTime(0)
{
	spanId = ctx.IncreSpanId();
	SwSpan* span = ctx.ActiveSpan();
	parentSpanId = (span ? span->GetSpanId() : -1);
}

void SwSpan::AppendLog(const std::string& key, const std::string& value)
{
	SwLog lg;
	klib::KTime::NowMillisecond(lg.timestamp);
	lg.kvMap[key] = value;
	logs.push_back(lg);
}

void SwSpan::Start()
{
	klib::KTime::NowMillisecond(startTime);
	ctx.Start(this);
}

void SwSpan::Finish()
{
	klib::KTime::NowMillisecond(endTime);
	ctx.Finish(this);
}

SwEntrySpan::SwEntrySpan(SwContext& ctx, const std::string& op) 
	:SwSpan(ctx, op, Entry), maxDepth(0)
{

}

void SwEntrySpan::Start()
{
	maxDepth = ctx.IncreDepth();
	if (maxDepth == 1)
		SwSpan::Start();
}

void SwEntrySpan::Finish()
{
	maxDepth = ctx.DecreDepth();
	if (maxDepth == 0)
		SwSpan::Finish();
}

void SwEntrySpan::Extract(const SwContextCarrier& carrier)
{
	SwSegmentRef ref(carrier);
	refs.push_back(ref);
	ctx.SetCorrelation(carrier.GetData().correlation);
	ctx.Relate(ref);
}

SwExitSpan::SwExitSpan(SwContext& seg, const std::string& op, const std::string& p) 
	:SwSpan(seg, op, Exit)
{
	peer = p;
}

void SwExitSpan::Start()
{
	ctx.IncreDepth();
	SwSpan::Start();
}

void SwExitSpan::Finish()
{
	ctx.DecreDepth();
	SwSpan::Finish();
}

void SwExitSpan::Inject(SwContextCarrier& carrier)
{
	SwData dat;
	dat.traceId = ctx.GetTraceId();
	dat.segmentId = ctx.GetSegmentId();
	dat.spanId = ctx.GetSpanId();
	dat.service = AgentInst::GetRef().GetService();//
	dat.serviceInstance = AgentInst::GetRef().GetServiceInstance();//
	dat.endpoint = operationName;
	dat.networkAddressUsedAtPeer = peer;
	dat.correlation = ctx.GetCorrelation();
	carrier.SetData(dat);
}
