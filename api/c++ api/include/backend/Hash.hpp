#pragma once
#include <string>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include "../ResponseType.hpp"

std::string Hash(std::string message);