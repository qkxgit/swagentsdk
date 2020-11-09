#ifndef __SWHTTPREPORTER__
#define __SWHTTPREPORTER__


#include <string>
#include "thread/KEventObject.h"

#include "SwContext.h"
#include "SwHttpClient.hpp"

class SwHttpReporter :public klib::KEventObject<SwContext>, public SwHttpClient
{
public:
	SwHttpReporter(const std::string &service, const std::string &serviceInstance);

	virtual bool Start(const std::string& host);

protected:
	virtual void ProcessEvent(const SwContext& ev);

private:
	std::string KindToString(SwEnumSpanKind kind) const;
	std::string LayerToString(SwEnumSpanLayer layer) const;
	void BatchCommit(const SwContext& ctx, RapidJsonWriter& jw);
	void WriteJson(const SwContext& ctx, RapidJsonWriter& jw);

private:
	std::string host;
	const std::string& service;
	const std::string& serviceInstance;
	uint32_t counter;
};
#endif