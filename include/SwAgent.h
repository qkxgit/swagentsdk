#ifndef __SWAGENT__
#define __SWAGENT__

#include <string>
#include <vector>

struct AgentConfig;
class SwSegment;
class SwHttpReporter;
class SwHttpClient;
namespace klib {
	class KPthread;
	class KAny;
	template<typename T>
	class KQueue;
	class KMutex;
};

class SwBroker
{
public:
	SwBroker();	
	~SwBroker();
	// 启动
	virtual bool Start(const AgentConfig& c);
	// 停止
	virtual void Stop();
	// 等待停止
	virtual void WaitForStop();
	// 线程是否运行中
	bool IsRunning() const;
	// 消息入队
	bool Post(const klib::KAny& ev);
	// 与skywalking服务端通信是否正常
	virtual bool IsReady() const { return ready; }
	// 提交数据
	bool Commit(const SwSegment& seg);
	
protected:
	// 数据处理
	virtual void ProcessEvent(const klib::KAny& ev);

private:
	// 提交服务信息
	void Properties(const std::string& service, const std::string& serviceInstance);
	// 心跳
	void KeepAlive() ;
	// 工作线程处理函数
	int SegmentWorker(int);

private:
	// 配置
	AgentConfig *config;
	// 数据提交实例
	SwHttpReporter *reporter;
	// 互斥量
	klib::KMutex *agentMtx;
	// 与服务端通信状态
	volatile bool ready;
	// 服务属性是否提交
	volatile bool propsent;
	// 线程运行状态
	volatile bool running;
	// 线程实例
	klib::KPthread* wkThread;
	// 队列
	klib::KQueue<klib::KAny>* segQueue;
	SwHttpClient* httpClient;
};

class SwAgent
{
public:
	SwAgent();
	~SwAgent();
	// 启动
	bool Start(const AgentConfig& c);
	// 停止
	void Stop();
	// 等待停止
	void WaitForStop();
	// 提交数据
	bool Commit(const SwSegment& seg);
	// 与skywalking服务端通信是否正常
	bool IsReady() const;
	// 获取服务名称
	const std::string& GetService() const;
	// 获取服务实例
	const std::string& GetServiceInstance() const;
	// 获取本地IP
	const std::string& GetLocalIp() const;

private:
	// 配置
	AgentConfig* config;
	// 互斥量
	klib::KMutex *brokerMtx;
	// 服务端
	std::vector<SwBroker *> brokers;
};

extern SwAgent SwInst;

#endif // !__SWAGENT__