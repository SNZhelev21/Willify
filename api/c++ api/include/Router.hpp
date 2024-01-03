#pragma once

#include "Debug.hpp"
#include <functional>
#include <iostream>
#include <sstream>
#include <regex>
#include <string>
#include <optional>
#include <unordered_map>
#include "Event.hpp"
#include "Request.hpp"
#include "ResponseType.hpp"

namespace Core {
	namespace Net {
		class Router {
			using returnType = std::tuple<ResponseType, std::string, std::optional<std::vector<std::string>>>;

			public:
				void Handle(Request& req);

				void AddRoute(std::string method, std::string path, std::function<returnType(Request&)> callback);

			private:
				std::unordered_map<std::string, Event<returnType, Core::Net::Request&>> get;
				std::unordered_map<std::string, Event<returnType, Core::Net::Request&>> post;
				std::unordered_map<std::string, Event<returnType, Core::Net::Request&>> put;
				std::unordered_map<std::string, Event<returnType, Core::Net::Request&>> del;

				void Respond(Request& req, returnType);
		};
	}
}