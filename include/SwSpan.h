#ifndef __SWSPAN__
#define __SWSPAN__
#include <vector>
#include "SwDefine.h"
#include "SwSegment.hpp"
class SwContext;
class SwSpan
{
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
	inline void SetException(const std::string& key, const std::string& value)
	{
		errorOcurred = true;
		AppendLog(key, value);
	}
	inline void SetTag(const std::string& key, const std::string& value) { tags[key] = value; }
	inline void SetTags(const StringStringMap& ts){ tags.insert(ts.begin(), ts.end()); }	
	void AppendLog(const std::string& key, const std::string& value);
	inline void AppendRefs(const SwSegmentRef& ref) { refs.push_back(ref); }

	virtual void Start();
	virtual void Finish();
	virtual void Extract(const SwContextCarrier& carrier)
	{
		throw "only support EntrySpan";
	}
	virtual void Inject(SwContextCarrier& carrier)
	{
		throw "only support ExitSpan";
	}

protected:
	SwContext& ctx;

	int32_t spanId;
	int32_t parentSpanId;
	SwEnumSpanKind kind;

	std::string operationName;
	std::string peer;

	SwEnumSpanLayer layer;
	SwEnumComponent component;
	StringStringMap tags;
	bool errorOcurred;
	std::vector<SwLog> logs;

	std::vector<SwSegmentRef> refs;
	
	uint64_t startTime;
	uint64_t endTime;
};


class SwEntrySpan :public SwSpan
{
public:
	SwEntrySpan(SwContext& ctx, const std::string& op);
	virtual void Start();
	virtual void Finish();
	virtual void Extract(const SwContextCarrier& carrier);

private:
	int maxDepth;
};

class SwExitSpan :public SwSpan
{
public:
	SwExitSpan(SwContext& seg, const std::string &op, const std::string& p);
	virtual void Start();
	virtual void Finish();
	virtual void Inject(SwContextCarrier& carrier);
};

#endif