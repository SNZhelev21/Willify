#pragma once
#include "Event.hpp"

class Core {
	public:
		Core& operator=(const Core&) = delete;

		static Core& Instance();

		Core& Start();
		void Exit();
}extern core;