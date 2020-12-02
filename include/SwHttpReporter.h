#ifndef __SWHTTPREPORTER__
#define __SWHTTPREPORTER__


#include <string>
#include "thread/KEventObject.h"

#include "SwContext.h"
#include "SwHttpClient.hpp"

class SwHttpReporter :public klib::KEventObject<SwSegment>, public SwHttpClient
{
public:
	SwHttpReporter(const std::string &service, const std::string &serviceInstance);

	virtual bool Start(const std::string& host);

protected:
	virtual void ProcessEvent(const SwSegment& seg);

private:
	std::string KindToString(SwEnumSpanKind kind) const;
	std::string LayerToString(SwEnumSpanLayer layer) const;
	void WriteJson(const SwSegment& seg, RapidJsonWriter& jw);

private:
	std::string host;
	const std::string& service;
	const std::string& serviceInstance;
	uint32_t counter;
};
#endif