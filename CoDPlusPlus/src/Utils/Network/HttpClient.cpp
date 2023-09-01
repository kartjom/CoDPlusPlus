#include "HttpClient.h"

httplib::Result HttpClient::Get(std::string host, std::string endpoint)
{
	httplib::Client cli(host);
	auto res = cli.Get(endpoint);

	return res;
}