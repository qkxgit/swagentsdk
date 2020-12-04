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

AgentConfig::AgentConfig(const std::string& swhost, const std::string& localIp, const std::string& service, const std::string& serviceInstance)
	:swhost(swhost), localIp(localIp), service(service), serviceInstance(serviceInstance)
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

void SwBroker::Stop()
{
	klib::KLockGuard<klib::KMutex> lock(*agentMtx);
	if (reporter->IsRunning())
	{
		running = false;
		reporter->Stop();
	}
}

void SwBroker::WaitForStop()
{
	wkThread->Join();
	reporter->WaitForStop();
}

bool SwBroker::IsRunning() const
{
	return running;
}

bool SwBroker::Post(const klib::KAny& ev)
{
	return segQueue->PushBack(ev);
}

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

bool SwBroker::Commit(const SwSegment& seg)
{
	return reporter->Post(seg);
}

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
			if (!broker->Start(AgentConfig(servers[i], c.localIp, c.service, c.serviceInstance)))
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

void SwAgent::Stop()
{
	klib::KLockGuard<klib::KMutex> lock(*brokerMtx);
	for (uint32_t i = 0; i < brokers.size(); ++i)
		brokers[i]->Stop();
}

void SwAgent::WaitForStop()
{
	klib::KLockGuard<klib::KMutex> lock(*brokerMtx);
	for (uint32_t i = 0; i < brokers.size(); ++i)
	{
		SwBroker* b = brokers[i];
		b->WaitForStop();
		delete b;
	}
	brokers.clear();
}

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

const std::string& SwAgent::GetService() const
{
	return config->service;
}

const std::string& SwAgent::GetServiceInstance() const
{
	return config->serviceInstance;
}

const std::string& SwAgent::GetLocalIp() const
{
	return config->localIp;
}

SwAgent& AgentInst::GetRef()
{
	static klib::KMutex agentMtx;
	static SwAgent agent;
	klib::KLockGuard<klib::KMutex> lock(agentMtx);
	return agent;
}
