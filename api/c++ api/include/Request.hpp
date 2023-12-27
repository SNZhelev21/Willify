#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <regex>

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "ctre.hpp"

namespace Core {
	namespace Net {
		struct RequestInformation {
			SOCKET sender;
			std::string original;
			std::string route;
			std::string method;
			std::unordered_map<std::string, std::string> parameters;
			std::unordered_map<std::string, std::string> headers;
			std::string body;
		};
		class Request {
		public:
			Request();
			Request(std::string& req, SOCKET& sender);

			RequestInformation m_info;

			// Split a string by a delimiter
			static std::vector<std::string> Split(std::string str, char delimiter);

			// Extract route from request
			static std::string GetRoute(std::string req);

			// Extract parameters from request
			// Returns a map of parameters
			static std::unordered_map<std::string, std::string> GetParameters(std::string req);

			// Extract method from request
			static std::string GetMethod(std::string req);

			// Extract body from request
			static std::string GetBody(std::string req);

			// Extract header from request
			static std::string GetHeader(std::string req, std::string header);

			// Extract headers from request
			// Returns a map of headers
			static std::unordered_map<std::string, std::string> GetHeaders(std::string req);

			// Extract protocol from request
			static std::string GetProtocol(std::string req);

			// Extract protocol version from request
			static std::string GetProtocolVersion(std::string req);
		};

		#ifndef REQOPOVERLOAD
			#define REQOPOVERLOAD
			inline std::ostream& operator<<(std::ostream& os, const Core::Net::Request& req) {
				return os << req.m_info.original;
			}
		#endif
	}
}
