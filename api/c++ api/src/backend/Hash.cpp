#include "../../include/backend/Hash.hpp"

std::string Hash(std::string message) {
	unsigned char hash[SHA512_DIGEST_LENGTH];
	SHA512((const unsigned char*)message.c_str(), message.length(), hash);
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
		ss << std::setw(2) << static_cast<int>(hash[i]);

	return ss.str();
}