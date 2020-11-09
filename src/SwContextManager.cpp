#include <assert.h>
#include "thread/KPthread.h"
#include "SwContextManager.h"
#include "SwContext.h"
#include "SwSpan.h"
#include "SwContextSnapshot.h"
#include "SwContextCarrier.h"

SwContext& SwContextManager::GetContext()
{
	static klib::KMutex ctxMtx;
	static std::map<uint32_t, SwContext> threadCtxMap;

	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	SwContext& c = threadCtxMap[id];
	c.Initialize();
	return c;
}

SwSpan* SwContextManager::CreateLocalSpan(const std::string& operationName)
{
	SwContext& ctx = GetContext();
	return new SwSpan(ctx, operationName);
}

SwSpan* SwContextManager::CreateEntrySpan(const std::string& operationName, const SwContextCarrier& carrier)
{
	SwContext& ctx = GetContext();
	SwSpan* entrySpan = NULL;
	SwSpan* parentSpan = ctx.ActiveSpan();
	if (parentSpan && parentSpan->IsEntry())
		entrySpan = parentSpan;
	else
		entrySpan = new SwEntrySpan(ctx, operationName);
	if (carrier.IsValid())
		entrySpan->Extract(carrier);
	return entrySpan;
}

SwSpan* SwContextManager::CreateExitSpan(const std::string& operationName, const std::string& peer, SwContextCarrier& carrier)
{
	SwContext& ctx = GetContext();
	SwSpan* exitSpan = NULL;
	SwSpan* parentSpan = ctx.ActiveSpan();
	if (parentSpan && parentSpan->IsExit())
		exitSpan = parentSpan;
	else
		exitSpan = new SwExitSpan(ctx, operationName, peer);
	exitSpan->Inject(carrier);
	return exitSpan;
}

SwContextSnapshot SwContextManager::Capture()
{
	SwContext& ctx = GetContext();
	SwSpan* a = ctx.ActiveSpan();
	SwSpan* b = ctx.EntrySpan();
	if (a && b)
		return SwContextSnapshot(ctx.GetTraceId(), ctx.GetSegmentId(), a->GetSpanId(),
			b->GetOperationName(), ctx.GetCorrelation());
	return SwContextSnapshot();
}

void SwContextManager::Continued(const SwContextSnapshot& snapShot)
{
	SwContext& ctx = GetContext();
	const SwData& dat = snapShot.GetData();
	if (!(!dat.segmentId.empty() && ctx.GetSegmentId() == dat.segmentId)
		&& snapShot.IsValid())
	{
		SwSegmentRef ref(snapShot);
		SwSpan* span = ctx.ActiveSpan();
		assert(span != NULL);
		span->AppendRefs(ref);
		ctx.Relate(ref);
		ctx.SetCorrelation(dat.correlation);
	}
}
