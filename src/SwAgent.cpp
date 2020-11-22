#include "SwAgent.h"
#include "util/KStringUtility.h"

SwBroker::SwBroker() 
	:reporter(config.service, config.serviceInstance), 
	klib::KEventObject<klib::KAny>("SwAgent Thread"),ready(false), propsent(false)
{

}

bool SwBroker::Start(const AgentConfig& c)
{
	klib::KLockGuard<klib::KMutex> lock(agentMtx);
	if (!reporter.IsRunning())
	{
		config = c;

		if (!klib::KEventObject<klib::KAny>::Start())
			return false;

		if (!reporter.Start(config.swhost))
		{
			klib::KEventObject<klib::KAny>::Stop();
			klib::KEventObject<klib::KAny>::WaitForStop();
			return false;
		}
		KeepAlive();
		Properties(config.service, config.serviceInstance);
	}
	return true;
}

bool SwBroker::Commit(const SwContext& ctx)
{
	return reporter.Post(ctx);
}

void SwBroker::Stop()
{
	klib::KLockGuard<klib::KMutex> lock(agentMtx);
	if (reporter.IsRunning())
	{
		klib::KEventObject<klib::KAny>::Stop();
		reporter.Stop();
	}
}

void SwBroker::WaitForStop()
{
	klib::KEventObject<klib::KAny>::WaitForStop();
	reporter.WaitForStop();
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

		std::string url = config.swhost + "/v3/management/reportProperties";
		std::string resp;
		if (!(propsent = SwHttpClient::HttpPost(url, jw.GetString(), resp)))
			printf("%s error:[%s], url:[%s]\n", __FUNCTION__, resp.c_str(), url.c_str());

		if (!propsent)
		{
			if (!klib::KEventObject<klib::KAny>::Post(AgentProperties(config.service, config.serviceInstance)))
				printf("set properties failed\n");
		}
	}
}

void SwBroker::KeepAlive()
{
	RapidJsonWriter jw;
	jw.StartObject();
	jw.Key(SwConstService); jw.String(config.service);
	jw.Key(SwConstServiceInstance); jw.String(config.serviceInstance);
	jw.EndObject();

	std::string url = config.swhost + "/v3/management/keepAlive";
	std::string resp;
	if (!(ready = SwHttpClient::HttpPost(url, jw.GetString(), resp)))
		printf("%s error:[%s], url:[%s]\n", __FUNCTION__, resp.c_str(), url.c_str());

	if (!klib::KEventObject<klib::KAny>::Post(AgentHeartbeat()))
		printf("set heartbeat failed\n");
}

bool SwAgent::Start(const AgentConfig& c)
{
	bool rc = true;
	{
		klib::KLockGuard<klib::KMutex> lock(brokerMtx);
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
	}
	if (!rc)
	{
		Stop();
		WaitForStop();
	}
	return rc;
}

bool SwAgent::Commit(const SwContext& ctx)
{
	klib::KLockGuard<klib::KMutex> lock(brokerMtx);
	for (uint32_t i = 0; i < brokers.size(); ++i)
	{
		SwBroker* broker = brokers[i];
		if (broker->IsReady() && broker->Commit(ctx))
			return true;
	}
	for (uint32_t i = 0; i < brokers.size(); ++i)
	{
		SwBroker* broker = brokers[i];
		if (broker->Commit(ctx))
			return true;
	}
	return false;
}

bool SwAgent::IsReady() const
{
	klib::KLockGuard<klib::KMutex> lock(brokerMtx);
	for (uint32_t i = 0; i < brokers.size(); ++i)
	{
		if (brokers[i]->IsReady())
			return true;
	}
	return false;
}

void SwAgent::Stop()
{
	klib::KLockGuard<klib::KMutex> lock(brokerMtx);
	for (uint32_t i = 0; i < brokers.size(); ++i)
		brokers[i]->Stop();
}

void SwAgent::WaitForStop()
{
	klib::KLockGuard<klib::KMutex> lock(brokerMtx);
	for (uint32_t i = 0; i < brokers.size(); ++i)
	{
		SwBroker* b = brokers[i];
		b->WaitForStop();
		delete b;
	}
	brokers.clear();
}
