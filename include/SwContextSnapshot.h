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
	// ���л�
	void Serialize(std::string& r);
	// �����л�
	bool Parse(const std::string& s);
	// �Ƿ���Ч�Ŀ���
	bool IsValid() const;
	// ��ȡ����
	const SwData& GetData() const;

private:
	SwData dat;
};

#endif // !__SWCONTEXTSNAPSHOT__
