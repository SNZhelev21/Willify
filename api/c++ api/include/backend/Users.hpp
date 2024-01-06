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

httpReturn Register(Core::Net::Request& req);

httpReturn Login(Core::Net::Request& req);

httpReturn GetUser(Core::Net::Request& req);

httpReturn DeleteUser(Core::Net::Request& req);

httpReturn UpdateUser(Core::Net::Request& req);

httpReturn AdminUpdateUser(Core::Net::Request& req);

httpReturn AdminDeleteUser(Core::Net::Request& req);

httpReturn AdminGetUser(Core::Net::Request& req);

httpReturn AdminGetUsers(Core::Net::Request& req);