#ifndef __SWDEFINE__
#define __SWDEFINE__

#include <map>
#include <string>
#include <stdint.h>

// span层类型
enum SwEnumSpanLayer
{
	LUnknown = 0,
	Database = 1,
	RPCFramework = 2,
	Http = 3,
	MQ = 4,
	Cache = 5
};

// span类型
enum SwEnumSpanKind
{
	KUnknown = 0,
	Entry = 1,
	Exit = 2,
	Local = 3
};

// 组件类型
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

// 数据
struct SwData
{
	// trace id
	std::string traceId;
	// segment id
	std::string segmentId;
	// span id
	int spanId;
	// 服务名称
	std::string service;
	// 服务实例名称
	std::string serviceInstance;
	// 操作
	std::string endpoint;
	// 对端
	std::string networkAddressUsedAtPeer;
	// 关联信息
	StringStringMap correlation;

	SwData() :spanId(-1) {}

	bool operator==(const SwData& r) const;
};

struct SwLog
{
	uint64_t timestamp;
	StringStringMap kvMap;
};
#endif