/*
segment 类
date:2020/12/08
author:qkx
*/
#include "SwSegment.h"
#include <IceUtil/UUID.h>
#include "util/KTime.h"
#include "util/KStringUtility.h"

SwSegment::SwSegment()
	:defaultTraceId(true)
{
	klib::KTime::NowMillisecond(timestamp);

	segmentId = IceUtil::generateUUID();
	segmentId.erase(std::remove(segmentId.begin(), segmentId.end(), '-'), segmentId.end());

	std::string traceId = IceUtil::generateUUID();
	traceId.erase(std::remove(traceId.begin(), traceId.end(), '-'), traceId.end());
	relatedTraceIds.push_back(traceId);
}

// 归档span, span 待归档的span
void SwSegment::Archive(SwSpan* span)
{
	spans.push_back(span);
}

// 关联traceid
void SwSegment::Relate(const std::string& traceId)
{
	if (defaultTraceId)
	{
		relatedTraceIds.pop_back();
		defaultTraceId = false;
	}
	relatedTraceIds.push_back(traceId);
}
