#ifndef __SWCONTEXTMANAGER__
#define __SWCONTEXTMANAGER__
#include <string>
class SwContext;
class SwSpan;
class SwContextCarrier;
class SwContextSnapshot;
class SwContextManager
{
public:
	// 创建LocalSpan
	static SwSpan* CreateLocalSpan(const std::string& operationName);
	// 创建EntrySpan
	static SwSpan* CreateEntrySpan(const std::string& operationName, const SwContextCarrier& carrier);
	// 创建ExitSpan
	static SwSpan* CreateExitSpan(const std::string& operationName, const std::string &peer, SwContextCarrier& carrier);
	// 获取当前线程激活的Span快照
	static SwContextSnapshot Capture();
	// 获取当前线程激活的Span
	static SwSpan* ActiveSpan();
	// 关联snapshot到当前激活的Span
	static void Continued(const SwContextSnapshot& snapShot);

private:
	static SwContext& GetContext();
};
#endif // !__SWCONTEXTMANAGER__
