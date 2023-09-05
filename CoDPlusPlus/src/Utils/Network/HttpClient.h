#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT // link libssl and libcrypto
#include <httplib.h>
#include <string>

class HttpClient
{
public:
	static httplib::Result Get(std::string host, std::string endpoint);
};