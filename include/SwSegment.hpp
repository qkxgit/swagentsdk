#ifndef __SWSEGMENT__
#define __SWSEGMENT__
#include "SwContextCarrier.h"
#include "SwContextSnapshot.h"
#include <IceUtil/UUID.h>
#include "util/KTime.h"
#include "util/KStringUtility.h"
#include <algorithm>
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
	SwSegment()
		:defaultTraceId(true)
	{
		klib::KTime::NowMillisecond(timestamp);

		segmentId = IceUtil::generateUUID();
		segmentId.erase(std::remove(segmentId.begin(), segmentId.end(), '-'), segmentId.end());

		std::string traceId = IceUtil::generateUUID();
		traceId.erase(std::remove(traceId.begin(), traceId.end(), '-'), traceId.end());
		relatedTraceIds.push_back(traceId);
	}

	void Archive(SwSpan* span)
	{
		spans.push_back(span);
	}

	void Relate(const std::string& traceId)
	{
		if (defaultTraceId)
		{
			relatedTraceIds.pop_back();
			defaultTraceId = false;
		}
		relatedTraceIds.push_back(traceId);
	}

private:
	std::string segmentId;
	uint64_t timestamp;
	std::vector<SwSpan*> spans;
	std::vector<std::string> relatedTraceIds;
	bool defaultTraceId;
};


#endif // !__SWSEGMENT__

