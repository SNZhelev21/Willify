#pragma once
#include "Event.hpp"

class Core {
	public:
		Event onRequest;
		Event onStart;
		Event onExit;

		Core& operator=(const Core&) = delete;

		static Core& Instance();

		Core& Start();
		void Exit();
}extern core;