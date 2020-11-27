#ifndef __SWCONTEXTSNAPSHOT__
#define __SWCONTEXTSNAPSHOT__

#include "SwDefine.h"
class SwContextSnapshot
{
public:
	SwContextSnapshot() {}

	SwContextSnapshot(const std::string &traceId,
		const std::string&segmentId,
		int spanId,
		const std::string& operationName,
		const StringStringMap & correlation);
	// 序列化
	void Serialize(std::string& r);
	// 反序列化
	bool Parse(const std::string& s);
	// 是否有效的快照
	bool IsValid() const;
	// 获取数据
	const SwData& GetData() const;

private:
	SwData dat;
};

#endif // !__SWCONTEXTSNAPSHOT__
