#include "SwContextSnapshot.h"
#include "SwAgent.h"
SwContextSnapshot::SwContextSnapshot()
{

}

SwContextSnapshot::SwContextSnapshot(const std::string& traceId, const std::string& segmentId, int spanId, const std::string& operationName, const StringStringMap& correlation)
{
	dat.traceId = traceId;
	dat.segmentId = segmentId;
	dat.spanId = spanId;
	dat.service = AgentInst::GetRef().GetService();
	dat.serviceInstance = AgentInst::GetRef().GetServiceInstance();
	dat.endpoint = operationName;
	dat.networkAddressUsedAtPeer = "";
	dat.correlation = correlation;
}

bool SwContextSnapshot::IsValid() const
{
	return !dat.traceId.empty()
		&& !dat.segmentId.empty()
		&& dat.spanId > -1;
}

const SwData& SwContextSnapshot::GetData() const
{
	return dat;
}
