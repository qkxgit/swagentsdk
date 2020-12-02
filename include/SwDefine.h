#ifndef __SWDEFINE__
#define __SWDEFINE__

#include <map>
#include <string>
#include <stdint.h>

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

class RapidJsonWriter :public rapidjson::Writer<rapidjson::StringBuffer>
{
public:
	RapidJsonWriter()
		:rapidjson::Writer<rapidjson::StringBuffer>(buffer)
	{

	}

	void Reset() { buffer.Clear(); rapidjson::Writer<rapidjson::StringBuffer>::Reset(buffer); }

	inline std::string GetString() const { return std::string(buffer.GetString(), buffer.GetSize()); }

private:
	rapidjson::StringBuffer buffer;
};

enum SwEnumSpanLayer
{
	LUnknown = 0,
	Database = 1,
	RPCFramework = 2,
	Http = 3,
	MQ = 4,
	Cache = 5
};

enum SwEnumSpanKind
{
	KUnknown = 0,
	Entry = 1,
	Exit = 2,
	Local = 3
};

enum SwEnumComponent
{
	CUnknown = 0,
	Mysql = 5,
	ORACLE = 6,
	Redis = 7,
	Kafka = 27,
	ActiveMQ = 44,
	http = 49,
	rpc = 50,
	InfluxDB = 89
};

extern const char* SwConstTraceId;
extern const char* SwConstSegmentId;
extern const char* SwConstSpanId;
extern const char* SwConstService;
extern const char* SwConstServiceInstance;
extern const char* SwConstOperationName;
extern const char* SwConstPeer;

typedef std::map<std::string, std::string> StringStringMap;

struct SwData
{
	std::string traceId;
	std::string segmentId;
	int spanId;
	std::string service;
	std::string serviceInstance;
	std::string endpoint;
	std::string networkAddressUsedAtPeer;
	StringStringMap correlation;

	SwData() :spanId(-1) {}

	bool operator==(const SwData& r) const
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
};

struct SwLog
{
	uint64_t timestamp;
	StringStringMap kvMap;
};
#endif