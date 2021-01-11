/*
skywalking 客户端，用于连接skywalking服务端并提交数据
date:2020/12/08
author:qkx
*/

#include "SwAgent.h"
#include "SwDefine.h"
#include "http/SwReporter.h"
#include "http/SwHttp.h"

#include "util/KStringUtility.h"
#include "thread/KEventObject.h"
#include "thread/KAny.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

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

AgentConfig::AgentConfig(const std::string& swhost, const std::string& localHost, const std::string& service, const std::string& serviceInstance)
	:swhost(swhost), localHost(localHost), service(service), serviceInstance(serviceInstance)
{

}

class RapidJsonWriter :public rapidjson::Writer<rapidjson::StringBuffer>
{
public:
	RapidJsonWriter() :rapidjson::Writer<rapidjson::StringBuffer>(buffer) {}
	void Reset()
	{
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer>::Reset(buffer);
	}
	inline std::string GetString() const
	{
		return std::string(buffer.GetString(), buffer.GetSize());
	}

private:
	rapidjson::StringBuffer buffer;
};

SwBroker::SwBroker() 
	:ready(false), propsent(false),running(false)
{
	segQueue = new klib::KQueue<klib::KAny>(2000);
	wkThread = new klib::KPthread("SwAgent thread");
	agentMtx = new klib::KMutex;
	config = new AgentConfig;
	reporter = new SwReporter(config->service, config->serviceInstance);
	httpClient = new SwHttp;
}

SwBroker::~SwBroker()
{
	delete wkThread;
	delete segQueue;
	
	delete agentMtx;
	delete config;
	delete reporter;
	delete httpClient;
}

// 启动
// c skywalking 服务端配置
bool SwBroker::Start(const AgentConfig& c)
{
	klib::KLockGuard<klib::KMutex> lock(*agentMtx);
	if (!reporter->IsRunning())
	{
		*config = c;

		try
		{
			running = true;
			wkThread->Run(this, &SwBroker::SegmentWorker, 1);
		}
		catch (const std::exception& e)
		{
			running = false;
			printf("SwBroker exception:[%s]\n", e.what());
			return false;
		}

		if (!reporter->Start(config->swhost))
		{
			running = false;
			wkThread->Join();
			return false;
		}
		KeepAlive();
		Properties(config->service, config->serviceInstance);
	}
	return true;
}

// 停止
void SwBroker::Stop()
{
	klib::KLockGuard<klib::KMutex> lock(*agentMtx);
	if (reporter->IsRunning())
	{
		running = false;
		reporter->Stop();
	}
}

// 等待停止
void SwBroker::WaitForStop()
{
	wkThread->Join();
	reporter->WaitForStop();
}

// 
bool SwBroker::IsRunning() const
{
	return running;
}

// 
bool SwBroker::Post(const klib::KAny& ev)
{
	return segQueue->PushBack(ev);
}

// 
int SwBroker::SegmentWorker(int)
{
	while (IsRunning())
	{
		if (IsReady())
		{
			klib::KAny msg;
			while (IsRunning() && IsReady() && segQueue->PopFront(msg))
			{
				ProcessEvent(msg);
			}
		}
		else
			klib::KTime::MSleep(100);
		
	}
	return 0;
}

// 提交segment，队列满返回false，否则返回true
bool SwBroker::Commit(const SwSegment& seg)
{
	return reporter->Post(seg);
}

// 处理消息
void SwBroker::ProcessEvent(const klib::KAny& ev)
{
	const std::type_info& ti = ev.TypeInfo();
	if (ti == typeid(AgentHeartbeat))
	{
		const AgentHeartbeat& hb = ev.AnyCast<AgentHeartbeat>(ev);
		KeepAlive();
		klib::KTime::MSleep(1000);
	}
	else if (ti == typeid(AgentProperties))
	{
		const AgentProperties& prop = ev.AnyCast<AgentProperties>(ev);
		Properties(prop.service, prop.serviceInstance);
		klib::KTime::MSleep(1000);
	}
}

// 提交服务相关信息
void SwBroker::Properties(const std::string& service, const std::string& serviceInstance)
{
	if (!propsent)
	{
		RapidJsonWriter jw;
		jw.StartObject();
		jw.Key(SwConstService); jw.String(service);
		jw.Key(SwConstServiceInstance); jw.String(serviceInstance);
		jw.Key("properties");
		jw.StartArray();
		jw.StartObject();
		jw.Key("language"); jw.String("Java");
		jw.EndObject();
		jw.EndArray();
		jw.EndObject();

		std::string url = config->swhost + "/v3/management/reportProperties";
		std::string resp;
		if (!(propsent = httpClient->HttpPost(url, jw.GetString(), resp)))
			printf("%s error:[%s], url:[%s]\n", __FUNCTION__, resp.c_str(), url.c_str());

		if (!propsent)
		{
			if (!Post(AgentProperties(config->service, config->serviceInstance)))
				printf("set properties failed\n");
		}
	}
}

// 发送心跳消息
void SwBroker::KeepAlive()
{
	RapidJsonWriter jw;
	jw.StartObject();
	jw.Key(SwConstService); jw.String(config->service);
	jw.Key(SwConstServiceInstance); jw.String(config->serviceInstance);
	jw.EndObject();

	std::string url = config->swhost + "/v3/management/keepAlive";
	std::string resp;
	if (!(ready = httpClient->HttpPost(url, jw.GetString(), resp)))
		printf("%s error:[%s], url:[%s]\n", __FUNCTION__, resp.c_str(), url.c_str());

	if (!Post(AgentHeartbeat()))
		printf("set heartbeat failed\n");
}

static const std::string AgentVersion = "1.0.0#20201205-1";

SwAgent::SwAgent()
{
	brokerMtx = new klib::KMutex;
	config = new AgentConfig;
}

SwAgent::~SwAgent()
{
	delete brokerMtx;
	delete config;
}

// 启动skywalking agent
bool SwAgent::Start(const AgentConfig& c)
{
	bool rc = true;
	{
		klib::KLockGuard<klib::KMutex> lock(*brokerMtx);
		std::vector<std::string> servers;
		klib::KStringUtility::SplitString(c.swhost, ",", servers);
		for (uint32_t i = 0; i < servers.size(); ++i)
		{
			SwBroker* broker = new SwBroker;
			brokers.push_back(broker);
			if (!broker->Start(AgentConfig(servers[i], c.localHost, c.service, c.serviceInstance)))
			{
				rc = false;
				break;
			}
		}
		*config = c;
	}
	if (!rc)
	{
		Stop();
		WaitForStop();
	}
	return rc;
}

// 停止
void SwAgent::Stop()
{
	klib::KLockGuard<klib::KMutex> lock(*brokerMtx);
	for (uint32_t i = 0; i < brokers.size(); ++i)
		brokers[i]->Stop();
}

void SwAgent::WaitForStop()
{
	for (uint32_t i = 0; i < brokers.size(); ++i)
	{
		SwBroker* b = brokers[i];
		b->WaitForStop();
		delete b;
	}
	brokers.clear();
}

// 判断是否有连接上的skywalking 服务端
bool SwAgent::IsReady() const
{
	klib::KLockGuard<klib::KMutex> lock(*brokerMtx);
	for (uint32_t i = 0; i < brokers.size(); ++i)
	{
		if (brokers[i]->IsReady())
			return true;
	}
	return false;
}

// 提交segment
bool SwAgent::Commit(const SwSegment& seg)
{
	klib::KLockGuard<klib::KMutex> lock(*brokerMtx);
	for (uint32_t i = 0; i < brokers.size(); ++i)
	{
		SwBroker* broker = brokers[i];
		if (broker->IsReady() && broker->Commit(seg))
			return true;
	}
	for (uint32_t i = 0; i < brokers.size(); ++i)
	{
		SwBroker* broker = brokers[i];
		if (broker->Commit(seg))
			return true;
	}
	return false;
}

// 获取服务名称
const std::string& SwAgent::GetService() const
{
	return config->service;
}

// 获取服务实例名称
const std::string& SwAgent::GetServiceInstance() const
{
	return config->serviceInstance;
}

// 获取本地IP
const std::string& SwAgent::GetLocalHost() const
{
	return config->localHost;
}

const std::string SwAgent::GetVersion() const
{
	return AgentVersion;
}

SwAgent& AgentInst::GetRef()
{
	static klib::KMutex agentMtx;
	static SwAgent agent;
	klib::KLockGuard<klib::KMutex> lock(agentMtx);
	return agent;
}
