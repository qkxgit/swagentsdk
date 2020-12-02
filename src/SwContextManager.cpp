#include <assert.h>
#include "thread/KPthread.h"
#include "SwContextManager.h"

#include "SwSpan.h"
#include "SwContextSnapshot.h"
#include "SwContextCarrier.h"

klib::KMutex SwContextManager::ctxMtx;

std::map<uint32_t, SwContext> SwContextManager::threadCtxMap;

SwSpan* SwContextManager::CreateLocalSpan(const std::string& operationName)
{
	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	SwContext& ctx = threadCtxMap[id];
	return new SwSpan(ctx, operationName);
}

SwSpan* SwContextManager::CreateEntrySpan(const std::string& operationName, const SwContextCarrier& carrier)
{
	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	SwContext& ctx = threadCtxMap[id];
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
	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	SwContext& ctx = threadCtxMap[id];
	SwSpan* exitSpan = NULL;
	SwSpan* parentSpan = ctx.ActiveSpan();
	if (parentSpan && parentSpan->IsExit())
		exitSpan = parentSpan;
	else
		exitSpan = new SwExitSpan(ctx, operationName, peer);
	exitSpan->Inject(carrier);
	return exitSpan;
}

SwContextSnapshot SwContextManager::CaptureInner(SwContext& ctx)
{
	SwSpan* a = ctx.ActiveSpan();
	if (a)
		return SwContextSnapshot(ctx.segment.relatedTraceIds[0], ctx.segment.segmentId, a->GetSpanId(),
			a->GetOperationName(), ctx.correlation);
	return SwContextSnapshot();
}

bool SwContextManager::IsFromCurrent(const SwContextSnapshot& snapshot, SwContext& ctx)
{
	return !snapshot.dat.segmentId.empty() && snapshot.dat.segmentId == CaptureInner(ctx).dat.segmentId;
}

SwContextSnapshot SwContextManager::Capture()
{
	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	SwContext& ctx = threadCtxMap[id];
	return CaptureInner(ctx);
}

void SwContextManager::Continued(const SwContextSnapshot& snapShot)
{
	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	SwContext& ctx = threadCtxMap[id];
	if (snapShot.IsValid() && !IsFromCurrent(snapShot,ctx))
	{
		SwSegmentRef ref(snapShot);
		SwSpan* span = ctx.ActiveSpan();
		assert(span != NULL);
		span->AppendRef(ref);
		ctx.segment.Relate(ref.dat.traceId);
		ctx.correlation = snapShot.dat.correlation;
	}
}

SwSpan* SwContextManager::ActiveSpan()
{
	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	SwContext& ctx = threadCtxMap[id];
	return ctx.ActiveSpan();
}

void SwContextManager::DestroyContext()
{
	klib::KLockGuard<klib::KMutex> lock(ctxMtx);
	uint32_t id = klib::KPthread::GetThreadId();
	threadCtxMap.erase(id);
}
