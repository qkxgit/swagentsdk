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
	// 创建LocalSpan
	static SwSpan* CreateLocalSpan(const std::string& operationName);
	// 创建EntrySpan
	static SwSpan* CreateEntrySpan(const std::string& operationName, const SwContextCarrier& carrier);
	// 创建ExitSpan
	static SwSpan* CreateExitSpan(const std::string& operationName, const std::string &peer, SwContextCarrier& carrier);
	// 获取当前线程激活的Span快照
	static SwContextSnapshot Capture();
	// 关联snapshot到当前激活的Span
	static void Continued(const SwContextSnapshot& snapShot);
	// 获取当前线程激活的Span
	static SwSpan* ActiveSpan();
	// 销毁context
	static void DestroyContext();

private:
	static SwContextSnapshot CaptureInner(SwContext& ctx);
	static bool IsFromCurrent(const SwContextSnapshot& snapshot, SwContext& ctx);

private:
	static klib::KMutex ctxMtx;
	static std::map<uint32_t, SwContext> threadCtxMap;
};
#endif // !__SWCONTEXTMANAGER__
