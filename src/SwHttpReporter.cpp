#include "SwHttpReporter.h"
#include "SwSpan.h"

SwHttpReporter::SwHttpReporter(const std::string& service, const std::string& serviceInstance)
	:klib::KEventObject<SwContext>("SwHttpReporter Thread", 2000), service(service), serviceInstance(serviceInstance), counter(0)
{

}

bool SwHttpReporter::Start(const std::string& host)
{
	this->host = host;
	return klib::KEventObject<SwContext>::Start();
}

void SwHttpReporter::ProcessEvent(const SwContext& ev)
{
	RapidJsonWriter jw;
	WriteJson(ev, jw);
	std::string url = host + "/v3/segment";
	std::string resp;
	if (!SwHttpClient::HttpPost(url, jw.GetString(), resp))
		printf("%s error:[%s], url:[%s]\n", __FUNCTION__, resp.c_str(), url.c_str());
}


std::string SwHttpReporter::KindToString(SwEnumSpanKind kind) const
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

std::string SwHttpReporter::LayerToString(SwEnumSpanLayer layer) const
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

void SwHttpReporter::BatchCommit(const SwContext& ctx, RapidJsonWriter& jw)
{
	if (counter++ == 0)
		jw.StartArray();
	WriteJson(ctx, jw);
	if (counter == 100)
	{
		jw.EndArray();
		counter = 0;
		std::string url = host + "/v3/segments";
		std::string resp;
		if (!SwHttpClient::HttpPost(url, jw.GetString(), resp))
			printf("%s error:[%s]\n", __FUNCTION__, resp.c_str());
		jw.Reset();
	}
}

void SwHttpReporter::WriteJson(const SwContext& ctx, RapidJsonWriter& jw)
{
	jw.StartObject();
	jw.Key(SwConstTraceId); jw.String(ctx.GetTraceId());
	jw.Key(SwConstSegmentId); jw.String(ctx.GetSegmentId());
	jw.Key(SwConstService); jw.String(service);
	jw.Key(SwConstServiceInstance); jw.String(serviceInstance);
	jw.Key("spans");
	jw.StartArray();
	std::vector<SwSpan*> spans = ctx.GetSpans();
	std::vector<SwSpan*>::const_iterator it = spans.begin();
	while (it != spans.end())
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
				const SwData& dat = sit->GetData();
				jw.StartObject();
				jw.Key("refType"); jw.Int(sit->GetRefType().compare("CrossProcess") == 0 ? 0 : 1);
				jw.Key(SwConstTraceId); jw.String(dat.traceId);
				jw.Key("parentTraceSegmentId"); jw.String(dat.segmentId);
				jw.Key("parentSpanId"); jw.Int(dat.spanId);
				jw.Key("parentService"); jw.String(dat.service);
				jw.Key("parentServiceInstance"); jw.String(dat.serviceInstance);
				jw.Key("parentEndpoint"); jw.String(dat.endpoint);
				jw.Key("networkAddressUsedAtPeer"); jw.String(dat.networkAddressUsedAtPeer);
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
