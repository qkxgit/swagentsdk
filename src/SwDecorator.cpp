#include "SwDecorator.h"

template<>
void SetCarrier(SwEnumSpanKind k, const StringStringMap& c, const StringStringMap& t)
{
	if (k == Exit && !c.empty())
	{
		StringStringMap& m = const_cast<StringStringMap&>(t);
		m.insert(c.begin(), c.end());
	}
}

template<>
void SetCarrier(SwEnumSpanKind k, const StringStringMap& c, StringStringMap& t)
{
	if (k == Exit && !c.empty())
		t.insert(c.begin(), c.end());
}


SwSpan* CreateSpan(const SwParameter& dat, StringStringMap& ctx, SwSnapshot& ss)
{
	SwSpan* span = NULL;
	SwContext& swCtx = SwContext::GetContext();
	switch (dat.kind)
	{
	case Entry:
	{
		span = swCtx.CreateEntrySpan(dat.operationName, dat.carrier);
		span->SetTags(dat.tags);
		span->SetLayer(dat.layer);
		span->SetComponent(dat.component);
		span->Start();
		break;
	}
	case Exit:
	{
		span = swCtx.CreateExitSpan(dat.operationName, dat.peer);
		span->SetTags(dat.tags);
		span->SetLayer(dat.layer);
		span->SetComponent(dat.component);
		span->Start();
		if (dat.snapshot.IsValid())
			swCtx.Continued(dat.snapshot);
		SwCarrier carrier;
		span->Inject(carrier);
		ctx = carrier.ToIceContext();
		span->SetTag("traceId", ctx["traceId"]);
		break;
	}
	default:
		span = swCtx.CreateLocalSpan(dat.operationName);
		span->SetTags(dat.tags);
		span->SetLayer(dat.layer);
		span->SetComponent(dat.component);
		span->Start();
		if (dat.snapshot.IsValid())
			swCtx.Continued(dat.snapshot);
		break;
	}
	ss = swCtx.Capture();
	return span;
}

SwGDecoratorV0 BindV0(void(*nf)(), const SwParameter& dat)
{
	assert(nf != NULL);
	return SwGDecoratorV0(nf, dat);
};
