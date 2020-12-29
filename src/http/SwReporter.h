#ifndef __SWHTTPREPORTER__
#define __SWHTTPREPORTER__

#include <string>
#include "SwContext.h"

class RapidJsonWriter;
namespace klib {
	class KPthread;
	template<typename T>
	class KQueue;
}
class SwHttp;
class SwReporter 
{
public:
	SwReporter(const std::string &service, const std::string &serviceInstance);
	~SwReporter();
	// 启动
	virtual bool Start(const std::string& host);
	// 停止
	virtual void Stop();
	// 等待停止
	virtual void WaitForStop();
	// 线程是否运行中
	bool IsRunning() const;
	// 消息入队
	bool Post(const SwSegment& ev);

protected:
	// 处理线程
	int SegmentWorker(int);	

private:
	// 单个提交
	void ProcessEventSingle();
	// 批量提交
	void ProcessEventBatch();
	// span类型转字符串
	std::string KindToString(SwEnumSpanKind kind) const;
	// span层转字符串
	std::string LayerToString(SwEnumSpanLayer layer) const;
	// segment转换成json格式
	void WriteJson(const SwSegment& seg, RapidJsonWriter& jw);

private:
	// skywalking 服务端ip
	std::string host;
	// 当前服务名称
	const std::string& service;
	// 服务名称实例
	const std::string& serviceInstance;
	// 是否运行中
	volatile bool running;
	// 工作线程
	klib::KPthread* wkThread;
	// 队列
	klib::KQueue<SwSegment>* segQueue;
	// http 客户端
	SwHttp* httpClient;

};
#endif