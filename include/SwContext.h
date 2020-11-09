#ifndef __SWCONTEXT__
#define __SWCONTEXT__


#include <vector>
#include <algorithm>
#include "SwSegment.hpp"

class SwSpan;
class SwContext
{
public:
	SwContext()
		:spanId(-1),timestamp(0),depth(0)
	{

	}

	inline const std::string& GetTraceId() const { return traceId; }
	inline const std::string& GetSegmentId() const { return segmentId; }
	inline uint64_t GetTimestamp() const { return timestamp; }
	inline int32_t GetSpanId() const { return spanId; }
	inline const StringStringMap& GetCorrelation() const { return correlation; }
	inline std::vector<SwSpan*> GetSpans() const { return archiveSpanStack; }

	inline void Relate(const SwSegmentRef& ref) { traceId = ref.GetData().traceId; }
	inline void SetCorrelation(const StringStringMap& c) { correlation = c; }
	inline int32_t IncreSpanId() { return ++spanId; }
	inline uint32_t IncreDepth() { return ++depth; };
	inline uint32_t DecreDepth() { return --depth; }

	void Start(SwSpan* span);
	void Finish(SwSpan* span);
	void Initialize();
	SwSpan* ActiveSpan();
	SwSpan* EntrySpan();

private:
	void Cleanup();

private:
	std::string traceId;
	std::string segmentId;
	uint64_t timestamp;
	int32_t spanId;
	uint32_t depth;
	StringStringMap correlation;
	std::vector<SwSpan *> archiveSpanStack;
	std::vector<SwSpan*> activeSpanStack;
};

#endif