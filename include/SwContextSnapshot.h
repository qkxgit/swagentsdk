#ifndef __SWCONTEXTSNAPSHOT__
#define __SWCONTEXTSNAPSHOT__

#include "SwDefine.h"
class SwContextSnapshot
{
public:
	SwContextSnapshot() {}

	SwContextSnapshot(const std::string &traceId,
		const std::string&segmentId,
		int spanId,
		const std::string& operationName,
		const StringStringMap & correlation);

	void Serialize(std::string& r);
	bool Parse(const std::string& s);

	bool IsValid() const;

	const SwData& GetData() const;

private:
	SwData dat;
};

#endif // !__SWCONTEXTSNAPSHOT__
