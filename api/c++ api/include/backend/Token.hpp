#pragma once
#undef max
#undef min
#include <string>
#include <optional>
#include <vector>
#include <tuple>
#include <jwt-cpp/traits/nlohmann-json/traits.h>
#include "../Request.hpp"
#include "../ResponseType.hpp"
#include "Secrets.hpp"

using httpReturn = std::tuple<Core::Net::ResponseType, std::string, std::optional<std::vector<std::string>>>;

httpReturn CheckAccessToken(Core::Net::Request& req);