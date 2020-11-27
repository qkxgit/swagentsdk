#ifndef __SWAGENT__
#define __SWAGENT__


#include <string>
#include "thread/KEventObject.h"
#include "thread/KAny.h"
#include "util/KSingleton.hpp"

#include "SwHttpClient.hpp"
#include "SwDefine.h"
#include "SwHttpReporter.h"

struct AgentHeartbeat
{
	int32_t hb;
	AgentHeartbeat()
		:hb(0)
	{

	}
};

struct AgentProperties
{
	AgentProperties(const std::string& s, const std::string& si)
		:service(s), serviceInstance(si)
	{}
	std::string service;
	std::string serviceInstance;
};

struct AgentConfig
{
	std::string swhost;
	std::string localIp;
	std::string service;
	std::string serviceInstance;

	AgentConfig(){}

	AgentConfig(const std::string& swh, const std::string& lip, const std::string& s, const std::string& si)
		:swhost(swh),localIp(lip),service(s),serviceInstance(si)
	{

	}
};

class SwBroker:public klib::KEventObject<klib::KAny>, public SwHttpClient
{
public:
	SwBroker();	
	// 启动
	virtual bool Start(const AgentConfig& c);
	// 停止
	virtual void Stop();
	// 等待停止
	virtual void WaitForStop();
	// 提交数据
	bool Commit(const SwContext& ctx);
	// 与skywalking服务端通信是否正常
	virtual bool IsReady() const { return ready; }	
	
protected:
	virtual void ProcessEvent(const klib::KAny& ev);

private:
	// 提交服务信息
	void Properties(const std::string& service, const std::string& serviceInstance);
	// 心跳
	void KeepAlive() ;

private:
	AgentConfig config;
	SwHttpReporter reporter;
	klib::KMutex agentMtx;
	volatile bool ready;
	volatile bool propsent;
};

class SwAgent
{
public:
	inline const std::string& GetService() const { return config.service; }
	inline const std::string& GetServiceInstance() const { return config.serviceInstance; }
	inline const std::string& GetLocalIp() const { return config.localIp; }

	bool Start(const AgentConfig& c);
	void Stop();
	void WaitForStop();
	// 提交数据
	bool Commit(const SwContext& ctx);
	// 与skywalking服务端通信是否正常
	bool IsReady() const;	

private:
	AgentConfig config;
	klib::KMutex brokerMtx;
	std::vector<SwBroker *> brokers;
};

typedef KSingleton<SwAgent> AgentInst;

#endif // !__SWAGENT__