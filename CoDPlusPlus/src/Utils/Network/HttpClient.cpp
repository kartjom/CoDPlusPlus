#include "HttpClient.h"

httplib::Result HttpClient::Get(std::string url)
{
	httplib::Client cli(url);
	auto res = cli.Get("/");

	return res;
}