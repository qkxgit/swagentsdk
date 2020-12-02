#include "SwDefine.h"

const char* SwConstTraceId = "traceId";
const char* SwConstSegmentId = "traceSegmentId";
const char* SwConstSpanId = "spanId";
const char* SwConstService = "service";
const char* SwConstServiceInstance = "serviceInstance";
const char* SwConstOperationName = "operationName";
const char* SwConstPeer = "peer";

bool SwData::operator==(const SwData& r) const
{
	return traceId == r.traceId
		&& segmentId == r.segmentId
		&& spanId == r.spanId
		&& service == r.service
		&& serviceInstance == r.serviceInstance
		&& endpoint == r.endpoint
		&& networkAddressUsedAtPeer == r.networkAddressUsedAtPeer
		&& correlation == r.correlation;
}