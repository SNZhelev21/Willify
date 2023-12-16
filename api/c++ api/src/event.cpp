#include "../include/event.hpp"

template <typename... Args>
void Event::operator+=(std::function<void(Args...)> callback)
{
	Function func;
	func.dataFunction = callback;
	func.defaultFunction = nullptr;
	this->m_Listeners.push_back(func);
}

void Event::operator+=(std::function<void()> callback) {
	Function func;
	func.defaultFunction = callback;
	func.dataFunction = nullptr;
	this->m_Listeners.push_back(func);
}

void Event::operator()(std::string data, SOCKET sock)
{
	for (auto& func : this->m_Listeners) {
		if (func.dataFunction != nullptr) {
			func.dataFunction(data, sock);
		}
	}
}

void Event::operator()() {
	for (auto& func : this->m_Listeners) {
		if (func.defaultFunction != nullptr) {
			func.defaultFunction();
		}
	}
}

//void Event::operator-=(std::function<void()> Callback)
//{
//	this->m_Listeners.erase(std::remove(this->m_Listeners.begin(), this->m_Listeners.end(), Callback), this->m_Listeners.end());
//}

//template <typename... Args>
Event* Event::Attach(std::function<void(std::string, SOCKET)> callback)
{
	Function func;
	func.dataFunction = callback;
	func.defaultFunction = nullptr;
	this->m_Listeners.push_back(func);
	return this;
}

Event* Event::Attach(std::function<void()> callback) {
	Function func;
	func.defaultFunction = callback;
	func.dataFunction = nullptr;
	this->m_Listeners.push_back(func);
	return this;
}


//void Event::Detach(std::function<void()> Callback)
//{
//	this->m_Listeners.erase(std::remove(this->m_Listeners.begin(), this->m_Listeners.end(), Callback), this->m_Listeners.end());
//}

void Event::DetachAll()
{
	this->m_Listeners.clear();
}

void Event::Invoke(std::string data, SOCKET sock)
{
	for (auto& func : this->m_Listeners)
	{
		if (func.dataFunction != nullptr) {
			func.dataFunction(data, sock);
		}
	}
}

//template <typename... Args>
void Event::Invoke() {
	for (auto& func : this->m_Listeners) {
		if (func.defaultFunction != nullptr) {
			func.defaultFunction();
		}
	}
}

//template Event* Event::Attach<std::string, SOCKET>(std::function<void(std::string, SOCKET)>);