/*
用于线程间传递span信息
date:2020/12/08
author:qkx
*/
#include "SwSnapshot.h"
#include "SwAgent.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

class RapidJsonWriter :public rapidjson::Writer<rapidjson::StringBuffer>
{
public:
	RapidJsonWriter() :rapidjson::Writer<rapidjson::StringBuffer>(buffer) {}
	void Reset()
	{
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer>::Reset(buffer);
	}
	inline std::string GetString() const
	{
		return std::string(buffer.GetString(), buffer.GetSize());
	}

private:
	rapidjson::StringBuffer buffer;
};

SwSnapshot::SwSnapshot(const std::string& traceId, const std::string& segmentId, int spanId, const std::string& operationName, 
	const std::string& service,
	const std::string& serviceInstance,
	const StringStringMap& correlation)
{
	dat.traceId = traceId;
	dat.segmentId = segmentId;
	dat.spanId = spanId;
	dat.service = service;
	dat.serviceInstance = serviceInstance;
	dat.endpoint = operationName;
	dat.networkAddressUsedAtPeer = "";
	dat.correlation = correlation;
}

// 序列化snapshot，r序列化结果
void SwSnapshot::Serialize(std::string& r)
{
	RapidJsonWriter w;
	w.StartObject();
	w.Key(SwConstTraceId); w.String(dat.traceId);
	w.Key(SwConstSegmentId); w.String(dat.segmentId);
	w.Key(SwConstSpanId); w.Int(dat.spanId);
	w.Key(SwConstService); w.String(dat.service);
	w.Key(SwConstServiceInstance); w.String(dat.serviceInstance);
	w.Key(SwConstOperationName); w.String(dat.endpoint);
	w.Key(SwConstPeer); w.String(dat.networkAddressUsedAtPeer);
	w.Key("correlation"); w.StartObject();
	StringStringMap::const_iterator it = dat.correlation.begin();
	while (it != dat.correlation.end())
	{
		w.Key(it->first.c_str()); w.String(it->second);
		++it;
	}
	w.EndObject();
	w.EndObject();
	r = w.GetString();
}

// 反序列化字符串为snapshot，包含所有必须字段返回成功，否则返回失败
bool SwSnapshot::Parse(const std::string& s)
{
	rapidjson::Document doc;
	if (doc.Parse(s).HasParseError())
		return false;
	
	if (!doc.HasMember(SwConstTraceId))
		return false;
	dat.traceId = doc[SwConstTraceId].GetString();

	if (!doc.HasMember(SwConstSegmentId))
		return false;
	dat.segmentId = doc[SwConstSegmentId].GetString();

	if (!doc.HasMember(SwConstSpanId))
		return false;
	dat.spanId = doc[SwConstSpanId].GetInt();

	if (!doc.HasMember(SwConstService))
		return false;
	dat.service = doc[SwConstService].GetString();

	if (!doc.HasMember(SwConstServiceInstance))
		return false;
	dat.serviceInstance = doc[SwConstServiceInstance].GetString();

	if (!doc.HasMember(SwConstOperationName))
		return false;
	dat.endpoint = doc[SwConstOperationName].GetString();

	if (!doc.HasMember(SwConstPeer))
		return false;
	dat.networkAddressUsedAtPeer = doc[SwConstPeer].GetString();

	const rapidjson::Value &correlation = doc["correlation"];
	if (correlation.IsObject())
	{
		rapidjson::Value::ConstMemberIterator it = correlation.MemberBegin();
		while (it != correlation.MemberEnd())
		{
			dat.correlation[it->name.GetString()] = it->value.GetString();
			++it;
		}
	}
	return true;
}

bool SwSnapshot::IsValid() const
{
	return !dat.traceId.empty()
		&& !dat.segmentId.empty()
		&& dat.spanId > -1;
}