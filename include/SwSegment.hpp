#ifndef __SWSEGMENT__
#define __SWSEGMENT__
#include "SwContextCarrier.h"
#include "SwContextSnapshot.h"

class SwSegmentRef
{
public:
	inline const SwData& GetData() const { return dat; }
	inline const std::string GetRefType() const { return refType; }

	SwSegmentRef()
		:refType("CrossProcess") {}

	SwSegmentRef(const SwContextSnapshot& snapshot)
		:refType("CrossThread"), dat(snapshot.GetData()) {}

	SwSegmentRef(const SwContextCarrier& carrier)
		:refType("CrossProcess"), dat(carrier.GetData()) {}

	inline bool operator==(const SwSegmentRef& other) const
	{
		if (this != &other)
			return refType == other.refType && other.dat == dat;
		return true;
	}

private:
	std::string refType;
	SwData dat;
};

#endif // !__SWSEGMENT__

