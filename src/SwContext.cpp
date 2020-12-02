#include <stdio.h>
#include "SwContext.h"
#include "SwSpan.h"
#include "SwAgent.h"
#include "SwContextManager.h"

SwContext::SwContext()
	:sid(-1),depth(0)
{

}

SwContext::~SwContext()
{

}

void SwContext::Start(SwSpan* span)
{
	if (span && std::find(spans.begin(), spans.end(), span) == spans.end())
		spans.push_back(span);
}

bool SwContext::Stop(SwSpan* span)
{
	std::vector<SwSpan*>::iterator it = std::find(spans.begin(), spans.end(), span);
	if (span->Finish())
		spans.erase(it);

	if (spans.empty())
	{
		if (!SwInst.Commit(segment))
		{
			std::vector<SwSpan*>::iterator it = segment.spans.begin();
			while (it != segment.spans.end())
			{
				SwSpan* span = *it;
				delete span;
				++it;
			}
			printf("Commit segment failed\n");
			// clear span
		}
		depth = 0;
		sid = -1;
		correlation.clear();
		segment = SwSegment();
		return true;
	}
	printf("Stop span failed\n");
	return false;
}

SwSpan* SwContext::ActiveSpan()
{
	if (spans.empty())
		return NULL;
	return spans[spans.size() - 1];
}

int32_t SwContext::NextSpanId()
{
	return ++sid;
}
