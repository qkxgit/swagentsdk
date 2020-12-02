#ifndef __SWCONTEXTCARRIER__
#define __SWCONTEXTCARRIER__
#include <string>
#include "SwDefine.h"

class SwContextCarrier
{
	friend class SwSegmentRef;
	friend class SwSpan;
	friend class SwExitSpan;
public:
	bool IsValid() const;
	// Ice Context ת���� carrier
	void FromIceContext(const StringStringMap& ctx);
	// carrier ת����Ice Context
	StringStringMap ToIceContext() const;
	
private:
	SwData dat;
};

#endif // !__SWCONTEXTCARRIER__