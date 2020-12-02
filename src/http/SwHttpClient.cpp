#include "SwHttpClient.h"
#include "SwDefine.h"
#include <curl/curl.h>
bool SwHttpClient::HttpPost(const std::string& url, const std::string& dat, std::string& resp) const
{
	//printf("data:[%s]\n", dat.c_str());
	long rc = 0;
	try
	{
		CURL* curl = curl_easy_init();
		if (curl)
		{
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dat.c_str()); // data  
			curl_easy_setopt(curl, CURLOPT_POST, 1); // post req 
			SetEasyOpt(url, curl, &resp);
			CURLcode cc;
			if (CURLE_OK == (cc = curl_easy_perform(curl)))
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rc);
			else
				resp = curl_easy_strerror(cc);
			curl_easy_cleanup(curl);
		}
	}
	catch (const std::exception& e)
	{
		resp = e.what();
	}
	return (rc / 100 == 2);
}

void SwHttpClient::SetEasyOpt(const std::string& url, void* curl, std::string* resp) const
{
	// set opts  
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url  
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https  
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false  
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0); // debug switch
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, resp);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);

	curl_slist* headers = NULL;
	headers = curl_slist_append(headers, "Content-Type:application/json; charset=UTF-8");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

size_t SwHttpClient::WriteCallback(void* data, size_t size, size_t nmemb, void* buffer)
{
	std::string* resp = static_cast<std::string*>(buffer);
	size_t sz = size * nmemb;
	if (resp)
		resp->append(std::string((char*)data, sz));
	return sz;
}

