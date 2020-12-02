#ifndef __SWHTTPCLIENT__
#define __SWHTTPCLIENT__

#include <string>

class SwHttpClient
{
public:
	// 发送http请求
	bool HttpPost(const std::string& url, const std::string& dat, std::string& resp) const;

private:
	// 设置请求参数
	void SetEasyOpt(const std::string& url, void* curl, std::string* resp) const;
	// 回调函数
	static size_t WriteCallback(void* data, size_t size, size_t nmemb, void* buffer);
};
#endif // !__SWHTTPCLIENT__