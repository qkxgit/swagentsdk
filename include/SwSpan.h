#ifndef __SWSPAN__
#define __SWSPAN__

#include <vector>
#include "SwDefine.h"
#include "SwSegment.h"

class SwContext;
class SwSpan
{
	friend class SwContext;
public:
	SwSpan(SwContext & ctx, const std::string &op, SwEnumSpanKind k = Local);

	inline bool IsEntry() const { return kind == Entry; }
	inline bool IsExit() const { return kind == Exit; }
	inline bool IsLocal() const { return kind == Local; }
	inline bool IsError() const { return errorOcurred; }

	inline int32_t GetSpanId() const { return spanId; }
	inline uint64_t GetStartTime() const { return startTime; }
	inline uint64_t GetEndTime() const { return endTime; }
	inline int32_t GetParentSpanId() const { return parentSpanId; }
	inline SwEnumSpanKind GetKind() const { return kind; }
	inline SwEnumSpanLayer GetLayer() const { return layer; }
	inline SwEnumComponent GetComponent() const { return component; }
	inline const std::string& GetPeer() const { return peer; }
	inline const std::string& GetOperationName() const { return operationName; }
	inline const std::vector<SwSegmentRef>& GetRefs() const { return refs; }
	inline const std::vector<SwLog>& GetLogs() const { return logs; }
	inline const StringStringMap& GetTags() const { return tags; }

	inline void SetOperationName(const std::string& op) { operationName = op; }
	inline void SetComponent(SwEnumComponent c) { component = c; }
	inline void SetLayer(SwEnumSpanLayer l) { layer = l; }
	inline void SetTag(const std::string& key, const std::string& value) { tags[key] = value; }
	inline void SetTags(const StringStringMap& ts){ tags.insert(ts.begin(), ts.end()); }	
	void SetError(const std::string& key, const std::string& value);
	void AppendLog(const std::string& key, const std::string& value);
	inline void AppendRef(const SwSegmentRef& ref) { refs.push_back(ref); }
	// 开始span
	virtual void Start();
	// 停止span
	bool Stop();
	// 解压context
	virtual void Extract(const SwCarrier& carrier);
	// 注入context
	virtual void Inject(SwCarrier& carrier){ throw "only support ExitSpan"; }

protected:
	// 停止span
	virtual bool Finish();

protected:
	SwContext& ctx;
	// 是否产生错误
	bool errorOcurred;
	// span id
	int32_t spanId;
	// 父 span id
	int32_t parentSpanId;
	// span 类型
	SwEnumSpanKind kind;
	// 操作名称
	std::string operationName;
	// 对端
	std::string peer;
	// 层
	SwEnumSpanLayer layer;
	// 组件
	SwEnumComponent component;
	// 标签
	StringStringMap tags;
	// 日志
	std::vector<SwLog> logs;
	// segment 引用
	std::vector<SwSegmentRef> refs;
	// 开始时间
	uint64_t startTime;
	// 结束时间
	uint64_t endTime;
};

class SwStackedSpan:public SwSpan
{
public:
	SwStackedSpan(SwContext& ctx, SwEnumSpanKind k, const std::string& op, const std::string& p);

protected:
	uint32_t depth;
};

class SwEntrySpan :public SwStackedSpan
{
public:
	SwEntrySpan(SwContext& ctx, const std::string& op);
	// 开始
	virtual void Start();
	// 解压
	virtual void Extract(const SwCarrier& carrier);

protected:
	// 结束
	virtual bool Finish();
};

class SwExitSpan :public SwStackedSpan
{
public:
	SwExitSpan(SwContext& ctx, const std::string &op, const std::string& p);
	// 开始
	virtual void Start();
	// 注入
	virtual void Inject(SwCarrier& carrier);

protected:
	// 结束
	virtual bool Finish();
};

#endif