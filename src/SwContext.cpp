#include "SwContext.h"
#include "SwSpan.h"
#include "SwAgent.h"
#include <IceUtil/UUID.h>
#include "util/KTime.h"
#include "util/KStringUtility.h"
void SwContext::Start(SwSpan* span)
{
	if (span && std::find(activeSpanStack.begin(), activeSpanStack.end(), span) == activeSpanStack.end())
		activeSpanStack.push_back(span);
}

void SwContext::Finish(SwSpan* span)
{
	SwSpan* s = ActiveSpan();
	assert(s == span);
	activeSpanStack.pop_back();

	if (std::find(archiveSpanStack.begin(), archiveSpanStack.end(), span) == archiveSpanStack.end())
		archiveSpanStack.push_back(span);

	if (activeSpanStack.empty())
	{
		if (!AgentInst::GetRef().Commit(*this))
		{
			std::vector<SwSpan*>::iterator it = archiveSpanStack.begin();
			while (it != archiveSpanStack.end())
			{
				SwSpan* span = *it;
				delete span;
				++it;
			}
			printf("Commit segment failed\n");
			// clear span
		}
		Cleanup();
	}
}

void SwContext::Initialize()
{
	if (activeSpanStack.empty())
	{
		klib::KTime::NowMillisecond(timestamp);

		segmentId = IceUtil::generateUUID();
		segmentId.erase(std::remove(segmentId.begin(), segmentId.end(), '-'), segmentId.end());

		traceId = IceUtil::generateUUID();
		traceId.erase(std::remove(traceId.begin(), traceId.end(), '-'), traceId.end());
	}
}


void SwContext::Cleanup()
{
	spanId = -1;
	depth = 0;
	timestamp = 0;
	segmentId = std::string();
	traceId = std::string();
	correlation.clear();
	archiveSpanStack.clear();
	activeSpanStack.clear();
}


SwSpan* SwContext::ActiveSpan()
{
	if (activeSpanStack.empty())
		return NULL;
	return activeSpanStack[activeSpanStack.size() - 1];
}

SwSpan* SwContext::EntrySpan()
{
	if (activeSpanStack.empty())
		return NULL;
	return activeSpanStack[0];
}
