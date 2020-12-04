#ifndef __SWCONTEXTSNAPSHOT__
#define __SWCONTEXTSNAPSHOT__

#include "SwDefine.h"
class SwSnapshot
{
	friend class SwSegmentRef;
	friend class SwContext;
public:
	SwSnapshot() {}

	SwSnapshot(const std::string &traceId,
		const std::string&segmentId,
		int spanId,
		const std::string& operationName,
		const std::string& service,
		const std::string& serviceInstance,
		const StringStringMap & correlation);
	// 序列化
	void Serialize(std::string& r);
	// 反序列化
	bool Parse(const std::string& s);
	// 是否有效的快照
	bool IsValid() const;

private:
	// 数据
	SwData dat;
};

#endif // !__SWCONTEXTSNAPSHOT__
