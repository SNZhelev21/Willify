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
#include <cpr/cpr.h>

using httpReturn = std::tuple<Core::Net::ResponseType, std::string, std::optional<std::vector<std::string>>>;
using json = nlohmann::json;

httpReturn CreateWill(Core::Net::Request& req);
httpReturn GetWills(Core::Net::Request& req);
httpReturn AdminGetWills(Core::Net::Request& req);
httpReturn GetWill(Core::Net::Request& req);
httpReturn AdminGetWill(Core::Net::Request& req);
httpReturn UpdateWill(Core::Net::Request& req);
httpReturn AdminUpdateWill(Core::Net::Request& req);
httpReturn DeleteWill(Core::Net::Request& req);
httpReturn DeleteWills(Core::Net::Request& req);
httpReturn AdminDeleteWill(Core::Net::Request& req);
httpReturn DoWill(Core::Net::Request& req);