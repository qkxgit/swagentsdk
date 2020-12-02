#ifndef __SWCONTEXTMANAGER__
#define __SWCONTEXTMANAGER__
#include <string>
#include "SwContext.h"

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
	// 获取当前线程激活的Span快照
	static SwContextSnapshot CaptureInner(SwContext& ctx);
	// 判断snapshot是否来自于ctx所在线程
	static bool IsFromCurrent(const SwContextSnapshot& snapshot, SwContext& ctx);
};
#endif // !__SWCONTEXTMANAGER__
