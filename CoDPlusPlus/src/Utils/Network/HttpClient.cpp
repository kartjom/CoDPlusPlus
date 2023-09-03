#include "HttpClient.h"

httplib::Result HttpClient::Get(std::string host, std::string endpoint)
{
	httplib::Client cli(host);
	cli.set_read_timeout(15, 0);
	cli.set_write_timeout(15, 0);

	auto res = cli.Get(endpoint);

	return res;
}