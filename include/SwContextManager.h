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
	static SwSpan* CreateLocalSpan(const std::string& operationName);

	static SwSpan* CreateEntrySpan(const std::string& operationName, const SwContextCarrier& carrier);

	static SwSpan* CreateExitSpan(const std::string& operationName, const std::string &peer, SwContextCarrier& carrier);

	static SwContextSnapshot Capture();

	static void Continued(const SwContextSnapshot& snapShot);

private:
	static SwContext& GetContext();
};
#endif // !__SWCONTEXTMANAGER__
