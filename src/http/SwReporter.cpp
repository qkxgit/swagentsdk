#include "http/SwReporter.h"
#include "SwSpan.h"
#include "http/SwHttp.h"
#include "thread/KQueue.h"
#include "thread/KPthread.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

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

SwReporter::SwReporter(const std::string& service, const std::string& serviceInstance)
	:service(service), serviceInstance(serviceInstance), running(false)
{
	httpClient = new SwHttp;
	segQueue = new klib::KQueue<SwSegment>(5000);
	wkThread = new klib::KPthread("SwReporter thread");
}

SwReporter::~SwReporter()
{
	delete segQueue;
	delete wkThread;
	delete httpClient;
}

bool SwReporter::Start(const std::string& host)
{
	this->host = host;
	try
	{
		running = true;
		wkThread->Run(this, &SwReporter::SegmentWorker, 1);
		return true;
	}
	catch (const std::exception&e)
	{
		printf("SwReporter exception:[%s]\n", e.what());
		running = false;
	}
	return false;
}

void SwReporter::Stop()
{
	running = false;
}

bool SwReporter::Post(const SwSegment& ev)
{
	return segQueue->PushBack(ev);
}

void SwReporter::WaitForStop()
{
	wkThread->Join();
}

bool SwReporter::IsRunning() const
{
	return running;
}

int SwReporter::SegmentWorker(int)
{
	while (running)
	{
		ProcessEventBatch();
	}
	return 0;
}

void SwReporter::ProcessEventBatch()
{
	if (!segQueue->IsEmpty())
	{
		std::string url = host + "/v3/segments";
		RapidJsonWriter jw;
		std::vector<SwSegment> segments;
		segQueue->GetPart(200, segments);
		std::vector<SwSegment>::const_iterator it = segments.begin();
		jw.StartArray();
		while (it != segments.end())
		{
			WriteJson(*it, jw);
			++it;
		}
		jw.EndArray();
		std::string resp;
		if (!httpClient->HttpPost(url, jw.GetString(), resp))
			printf("%s error:[%s], url:[%s]\n", __FUNCTION__, resp.c_str(), url.c_str());
	}
	else
	{
		klib::KTime::MSleep(10);
	}
}

void SwReporter::ProcessEventSingle()
{
	SwSegment seg;
	while (running && segQueue->PopFront(seg))
	{
		RapidJsonWriter jw;
		WriteJson(seg, jw);
		std::string url = host + "/v3/segment";
		std::string resp;
		if (!httpClient->HttpPost(url, jw.GetString(), resp))
			printf("%s error:[%s], url:[%s]\n", __FUNCTION__, resp.c_str(), url.c_str());
	}	
}


std::string SwReporter::KindToString(SwEnumSpanKind kind) const
{
	switch (kind)
	{
	case Entry:
		return "Entry";
	case Exit:
		return "Exit";
	case Local:
		return "Local";
	default:
		return "Unknown";
	}
}

std::string SwReporter::LayerToString(SwEnumSpanLayer layer) const
{
	switch (layer)
	{
	case Database:
		return "Database";
	case RPCFramework:
		return "RPCFramework";
	case Http:
		return "Http";
	case MQ:
		return "MQ";
	case Cache:
		return "Cache";
	default:
		return "Unknown";
	}
}

void SwReporter::WriteJson(const SwSegment& seg, RapidJsonWriter& jw)
{
	jw.StartObject();
	jw.Key(SwConstTraceId); jw.String(seg.relatedTraceIds[0]);
	jw.Key(SwConstSegmentId); jw.String(seg.segmentId);
	jw.Key(SwConstService); jw.String(service);
	jw.Key(SwConstServiceInstance); jw.String(serviceInstance);
	jw.Key("spans");
	jw.StartArray();
	std::vector<SwSpan*>::const_iterator it = seg.spans.begin();
	while (it != seg.spans.end())
	{
		SwSpan* span = *it;
		jw.StartObject();
		jw.Key(SwConstOperationName); jw.String(span->GetOperationName());
		jw.Key("startTime"); jw.Uint64(span->GetStartTime());
		jw.Key("endTime"); jw.Uint64(span->GetEndTime());
		jw.Key("spanType"); jw.String(KindToString(span->GetKind()));
		jw.Key(SwConstSpanId); jw.Int(span->GetSpanId());
		jw.Key("parentSpanId"); jw.Int(span->GetParentSpanId());
		jw.Key("isError"); jw.Bool(span->IsError());
		jw.Key("componentId"); jw.Int(span->GetComponent());
		jw.Key("spanLayer"); jw.String(LayerToString(span->GetLayer()));
		jw.Key(SwConstPeer); jw.String(span->GetPeer());

		const StringStringMap& tags = span->GetTags();
		if (!tags.empty())
		{
			jw.Key("tags");
			jw.StartArray();
			StringStringMap::const_iterator tit = tags.begin();
			while (tit != tags.end())
			{
				jw.StartObject();
				jw.Key("key"); jw.String(tit->first);
				jw.Key("value"); jw.String(tit->second);
				jw.EndObject();
				++tit;
			}
			jw.EndArray();
		}

		const std::vector<SwLog>& logs = span->GetLogs();
		if (!logs.empty())
		{
			jw.Key("logs");
			jw.StartArray();
			std::vector<SwLog>::const_iterator lit = logs.begin();
			while (lit != logs.end())
			{
				const SwLog& log = *lit;
				jw.StartObject();
				jw.Key("time"); jw.Uint64(log.timestamp);
				jw.Key("data"); jw.StartArray();
				StringStringMap::const_iterator ait = log.kvMap.begin();
				while (ait != log.kvMap.end())
				{
					jw.StartObject();
					jw.Key("key"); jw.String(ait->first);
					jw.Key("value"); jw.String(ait->second);
					jw.EndObject();
					++ait;
				}
				jw.EndArray();
				jw.EndObject();
				++lit;
			}
			jw.EndArray();
		}

		const std::vector<SwSegmentRef>& refs = span->GetRefs();
		if (!refs.empty())
		{
			jw.Key("refs");
			jw.StartArray();
			std::vector<SwSegmentRef>::const_iterator sit = refs.begin();
			while (sit != refs.end())
			{
				const SwData& dat = sit->dat;
				jw.StartObject();
				jw.Key("refType"); jw.Int(sit->refType.compare("CrossProcess") == 0 ? 0 : 1);
				jw.Key(SwConstTraceId); jw.String(dat.traceId);
				jw.Key("parentTraceSegmentId"); jw.String(dat.segmentId);
				jw.Key("parentSpanId"); jw.Int(dat.spanId);
				jw.Key("parentService"); jw.String(dat.service);
				jw.Key("parentServiceInstance"); jw.String(dat.serviceInstance);
				jw.Key("parentEndpoint"); jw.String(dat.operationName);
				jw.Key("networkAddressUsedAtPeer"); jw.String(dat.peer);
				jw.EndObject();
				++sit;
			}
			jw.EndArray();
		}

		jw.EndObject();
		delete span;
		++it;
	}
	jw.EndArray();
	jw.EndObject();
}
