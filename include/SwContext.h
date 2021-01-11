#ifndef __SWCONTEXT__
#define __SWCONTEXT__

#include <vector>
#include "SwSegment.h"

class SwSpan;
class SwAgent;
class SwContext
{
	friend class SwSpan;
	friend class SwEntrySpan;
	friend class SwExitSpan;
public:
	SwContext();
	~SwContext();
	// 设置agent
	void Initialize(SwAgent* ag);
	// 获取context
	static SwContext& GetContext();
	// 线程退出时销毁context
	static void DestroyContext();
	// 创建LocalSpan
	SwSpan* CreateLocalSpan(const std::string& operationName, const std::string& peer);
	// 创建EntrySpan
	SwSpan* CreateEntrySpan(const std::string& operationName, const std::string& peer, const SwCarrier& carrier);
	// 创建ExitSpan
	SwSpan* CreateExitSpan(const std::string& operationName, const std::string& peer);
	// 获取当前激活的Span快照
	SwSnapshot Capture();
	// 关联snapshot到当前激活的Span
	void Continued(const SwSnapshot& snapShot);
	// 当前激活的span
	SwSpan* ActiveSpan();
	SwAgent* GetAgent();
	
private:
	// 启动span
	void Start(SwSpan* span);
	// 停止span
	bool Stop(SwSpan* span);
	// 判断snapshot是否来自于自己
	bool IsFromCurrent(const SwSnapshot& snapshot);

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
	SwAgent* agent;
};

#endif