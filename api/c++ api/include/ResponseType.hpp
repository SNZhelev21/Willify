#pragma once

namespace Core {
	namespace Net {
		enum class ResponseType {
			OK,
			CREATED,
			BAD_REQUEST,
			NOT_FOUND,
			INTERNAL_ERROR,
			NOT_IMPLEMENTED,
			NOT_AUTHORIZED,
			JSON,
			HTML,
			TEXT,
			REDIRECT,
			ALREADY_EXISTS
		};
	}
}