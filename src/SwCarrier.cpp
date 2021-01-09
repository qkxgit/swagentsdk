/*
用于跨进程传递span上下文信息
date:2020/12/08
author:qkx
*/
#include <cstdlib>
#include "SwCarrier.h"
#include "SwSnapshot.h"
#include "util/KStringUtility.h"

// 判断carrier是否有效
// 有效返回true,无效返回false
bool SwCarrier::IsValid() const
{
	return !dat.traceId.empty()
		&& !dat.segmentId.empty()
		&& !dat.service.empty()
		&& !dat.serviceInstance.empty()
		&& !dat.endpoint.empty()
		&& !dat.networkAddressUsedAtPeer.empty()
		&& dat.spanId > -1;
}

// ice context 转换成 carrier
// ctx 为ice context
void SwCarrier::FromIceContext(const StringStringMap& ctx)
{
	StringStringMap::const_iterator it = ctx.begin();
	while (it != ctx.end())
	{
		if (it->first.compare(SwConstTraceId) == 0)
			dat.traceId = it->second;
		else if (it->first.compare(SwConstSegmentId) == 0)
			dat.segmentId = it->second;
		else if (it->first.compare(SwConstSpanId) == 0)
			dat.spanId = atoi(it->second.c_str());
		else if (it->first.compare(SwConstService) == 0)
			dat.service = it->second;
		else if (it->first.compare(SwConstServiceInstance) == 0)
			dat.serviceInstance = it->second;
		else if (it->first.compare(SwConstOperationName) == 0)
			dat.endpoint = it->second;
		else if (it->first.compare(SwConstPeer) == 0)
			dat.networkAddressUsedAtPeer = it->second;
		else
			dat.correlation[it->first] = it->second;
		++it;
	}
}

// carrier 转换成ice context
// 返回ice context
StringStringMap SwCarrier::ToIceContext() const
{
	StringStringMap ctx;
	ctx[SwConstTraceId] = dat.traceId;
	ctx[SwConstSegmentId] = dat.segmentId;
	ctx[SwConstSpanId] = klib::KStringUtility::Int32ToString(dat.spanId);
	ctx[SwConstService] = dat.service;
	ctx[SwConstServiceInstance] = dat.serviceInstance;
	ctx[SwConstOperationName] = dat.endpoint;
	ctx[SwConstPeer] = dat.networkAddressUsedAtPeer;
	ctx.insert(dat.correlation.begin(), dat.correlation.end());
	return ctx;
}

void SwCarrier::FromSnapshot(const SwSnapshot& snapshot)
{
	dat = snapshot.dat;
}
