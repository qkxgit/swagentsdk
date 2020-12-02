#ifndef __SWCONTEXT__
#define __SWCONTEXT__


#include <vector>
#include "SwSegment.hpp"

class SwSpan;
class SwContext
{
	friend class SwSpan;
	friend class SwEntrySpan;
	friend class SwExitSpan;
	friend class SwContextManager;
public:
	SwContext();
	~SwContext();
	void Start(SwSpan* span);
	bool Stop(SwSpan* span);
	SwSpan* ActiveSpan();
	int32_t NextSpanId();

private:
	uint32_t depth;
	int32_t sid;
	StringStringMap correlation;
	SwSegment segment;
	std::vector<SwSpan*> spans;
};

#endif