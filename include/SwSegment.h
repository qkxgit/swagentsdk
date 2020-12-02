#ifndef __SWSEGMENT__
#define __SWSEGMENT__

#include <algorithm>
#include <vector>
#include "SwContextCarrier.h"
#include "SwContextSnapshot.h"

class SwSegmentRef
{
	friend class SwHttpReporter;
	friend class SwContextManager;
public:
	SwSegmentRef()
		:refType("CrossProcess") {}

	SwSegmentRef(const SwContextSnapshot& snapshot)
		:refType("CrossThread"), dat(snapshot.dat) {}

	SwSegmentRef(const SwContextCarrier& carrier)
		:refType("CrossProcess"), dat(carrier.dat) {}

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

class SwSpan;
class SwSegment
{
	friend class SwHttpReporter;
	friend class SwExitSpan;
	friend class SwContext;
	friend class SwContextManager;
public:
	SwSegment();
	// 归档span
	void Archive(SwSpan* span);
	// 关联span
	void Relate(const std::string& traceId);

private:
	// 段ID
	std::string segmentId;
	// 时间戳
	uint64_t timestamp;
	// 归档的span集合
	std::vector<SwSpan*> spans;
	// 关联的trace id
	std::vector<std::string> relatedTraceIds;
	bool defaultTraceId;
};


#endif // !__SWSEGMENT__

