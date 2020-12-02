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


SwSpan* CreateSpan(const SwParameter& dat, StringStringMap& ctx, SwContextSnapshot& ss)
{
	SwSpan* span = NULL;
	switch (dat.kind)
	{
	case Entry:
	{
		span = SwContextManager::CreateEntrySpan(dat.operationName, dat.carrier);
		span->SetTags(dat.tags);
		span->SetLayer(dat.layer);
		span->SetComponent(dat.component);
		span->Start();
		break;
	}
	case Exit:
	{
		SwContextCarrier carrier;
		span = SwContextManager::CreateExitSpan(dat.operationName, dat.peer, carrier);
		span->SetTags(dat.tags);
		span->SetLayer(dat.layer);
		span->SetComponent(dat.component);
		span->Start();
		if (dat.snapshot.IsValid())
			SwContextManager::Continued(dat.snapshot);
		span->Inject(carrier);
		ctx = carrier.ToIceContext();
		span->SetTag("traceId", ctx["traceId"]);
		break;
	}
	default:
		span = SwContextManager::CreateLocalSpan(dat.operationName);
		span->SetTags(dat.tags);
		span->SetLayer(dat.layer);
		span->SetComponent(dat.component);
		span->Start();
		if (dat.snapshot.IsValid())
			SwContextManager::Continued(dat.snapshot);
		break;
	}
	ss = SwContextManager::Capture();
	return span;
}

SwGDecoratorV0 BindV0(void(*nf)(), const SwParameter& dat)
{
	assert(nf != NULL);
	return SwGDecoratorV0(nf, dat);
};
