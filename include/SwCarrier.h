#ifndef __SWCONTEXTCARRIER__
#define __SWCONTEXTCARRIER__
#include <string>
#include "SwDefine.h"

class SwSnapshot;
class SwCarrier
{
	friend class SwSegmentRef;
	friend class SwSpan;
	friend class SwExitSpan;
public:
	// 判断carrier是否有效
	bool IsValid() const;
	// Ice Context 转换成 carrier
	void FromIceContext(const StringStringMap& ctx);
	// carrier 转换成Ice Context
	StringStringMap ToIceContext() const;

	void FromSnapshot(const SwSnapshot& snapshot);
	
private:
	// 数据
	SwData dat;
};

#endif // !__SWCONTEXTCARRIER__