#ifndef __SWCONTEXT__
#define __SWCONTEXT__

#include <vector>
#include "SwSegment.h"

class SwSpan;
class SwContext
{
	friend class SwSpan;
	friend class SwEntrySpan;
	friend class SwExitSpan;
	friend class SwContextManager;
public:
	SwContext();
	~SwContext();
	// 启动span
	void Start(SwSpan* span);
	// 停止span
	bool Stop(SwSpan* span);
	// 当前激活的span
	SwSpan* ActiveSpan();
	// 下一个Span id
	int32_t NextSpanId();

private:
	// 栈深度
	uint32_t depth;
	// span id
	int32_t sid;
	// 关联信息
	StringStringMap correlation;
	// 段
	SwSegment segment;
	// 跨度
	std::vector<SwSpan*> spans;
};

#endif