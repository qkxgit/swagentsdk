#include <cstdlib>
#include "SwContextCarrier.h"
#include "util/KStringUtility.h"


bool SwContextCarrier::IsValid() const
{
	return !dat.traceId.empty()
		&& !dat.segmentId.empty()
		&& !dat.service.empty()
		&& !dat.serviceInstance.empty()
		&& !dat.endpoint.empty()
		&& !dat.networkAddressUsedAtPeer.empty();
}

const SwData& SwContextCarrier::GetData() const
{
	return dat;
}

void SwContextCarrier::SetData(const SwData& d)
{
	dat = d;
}

void SwContextCarrier::FromIce(const StringStringMap& ctx)
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

StringStringMap SwContextCarrier::ToIce() const
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
