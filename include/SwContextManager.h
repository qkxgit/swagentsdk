#ifndef __SWCONTEXTMANAGER__
#define __SWCONTEXTMANAGER__
#include <string>
#include "SwContext.h"
#include "thread/KMutex.h"
class SwSpan;
class SwContextCarrier;
class SwContextSnapshot;
class SwContextManager
{
	friend class SwContext;
public:
	// ����LocalSpan
	static SwSpan* CreateLocalSpan(const std::string& operationName);
	// ����EntrySpan
	static SwSpan* CreateEntrySpan(const std::string& operationName, const SwContextCarrier& carrier);
	// ����ExitSpan
	static SwSpan* CreateExitSpan(const std::string& operationName, const std::string &peer, SwContextCarrier& carrier);
	// ��ȡ��ǰ�̼߳����Span����
	static SwContextSnapshot Capture();
	// ����snapshot����ǰ�����Span
	static void Continued(const SwContextSnapshot& snapShot);
	// ��ȡ��ǰ�̼߳����Span
	static SwSpan* ActiveSpan();
	// ����context
	static void DestroyContext();

private:
	static SwContextSnapshot CaptureInner(SwContext& ctx);
	static bool IsFromCurrent(const SwContextSnapshot& snapshot, SwContext& ctx);

private:
	static klib::KMutex ctxMtx;
	static std::map<uint32_t, SwContext> threadCtxMap;
};
#endif // !__SWCONTEXTMANAGER__
