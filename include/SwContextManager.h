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
	// ����LocalSpan
	static SwSpan* CreateLocalSpan(const std::string& operationName);
	// ����EntrySpan
	static SwSpan* CreateEntrySpan(const std::string& operationName, const SwContextCarrier& carrier);
	// ����ExitSpan
	static SwSpan* CreateExitSpan(const std::string& operationName, const std::string &peer, SwContextCarrier& carrier);
	// ��ȡ��ǰ�̼߳����Span����
	static SwContextSnapshot Capture();
	// ��ȡ��ǰ�̼߳����Span
	static SwSpan* ActiveSpan();
	// ����snapshot����ǰ�����Span
	static void Continued(const SwContextSnapshot& snapShot);

private:
	static SwContext& GetContext();
};
#endif // !__SWCONTEXTMANAGER__
