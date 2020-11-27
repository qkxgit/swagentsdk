#ifndef __SWCONTEXTCARRIER__
#define __SWCONTEXTCARRIER__
#include <string>
#include "SwDefine.h"

class SwContextCarrier
{
public:
	bool IsValid() const;
	const SwData& GetData() const;
	void SetData(const SwData& d);
	void FromIce(const StringStringMap& ctx);
	// ת����Ice Context
	StringStringMap ToIce() const;
	
private:
	SwData dat;
};

#endif // !__SWCONTEXTCARRIER__