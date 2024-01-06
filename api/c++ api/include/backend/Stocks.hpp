#pragma once
#undef max
#undef min
#include <string>
#include "Hash.hpp"
#include <optional>
#include <vector>
#include <tuple>
#include <jwt-cpp/traits/nlohmann-json/traits.h>
#include "../Request.hpp"
#include "../Database.hpp"
#include "../nlohmann json/json.hpp"
#include "Secrets.hpp"

using httpReturn = std::tuple<Core::Net::ResponseType, std::string, std::optional<std::vector<std::string>>>;
using json = nlohmann::json;

httpReturn CreateStock(Core::Net::Request& req);
httpReturn GetStocks(Core::Net::Request& req);
httpReturn AdminGetStocks(Core::Net::Request& req);
httpReturn GetStock(Core::Net::Request& req);
httpReturn AdminGetStock(Core::Net::Request& req);
httpReturn UpdateStock(Core::Net::Request& req);
httpReturn AdminUpdateStock(Core::Net::Request& req);
httpReturn DeleteStock(Core::Net::Request& req);
httpReturn AdminDeleteStock(Core::Net::Request& req);