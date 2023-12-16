#include "../include/core.hpp"

Core& Core::Instance()
{
	return core;
}

Core& Core::Start() {
	//this->onStart.Invoke();
	return core;
}

void Core::Exit() {
	//this->onExit.Invoke();

	delete& core;
}

Core core;