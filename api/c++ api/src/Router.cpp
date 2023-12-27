#include "../include/Router.hpp"

void Core::Net::Router::Handle(Request& req) {
	//std::cout << "Received data: \n" << req << '\n';

	std::string method = req.m_info.method;

	if (method == "GET") {
		this->get[req.m_info.route].Invoke(req);
	}
	else if (method == "POST") {
		this->post[req.m_info.route].Invoke(req);
	}
	else if (method == "PUT") {
		this->put[req.m_info.route].Invoke(req);
	}
	else if (method == "DELETE") {
		this->del[req.m_info.route].Invoke(req);
	}
}

void Core::Net::Router::AddRoute(std::string method, std::string path, std::function<void(Request&)> callback) {
	for (auto& c : method) {
		c = toupper(c);
	}

	if (method == "GET") {
		this->get[path].Attach(callback);
	}
	else if (method == "POST") {
		this->post[path].Attach(callback);
	}
	else if (method == "PUT") {
		this->put[path].Attach(callback);
	}
	else if (method == "DELETE") {
		this->del[path].Attach(callback);
	}
}