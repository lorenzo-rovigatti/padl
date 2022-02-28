#include <iostream>
#include <asio.hpp>

using namespace asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

string read_(tcp::socket &socket) {
	asio::streambuf buf;
	asio::read_until(socket, buf, "\n");
	string data = asio::buffer_cast<const char*>(buf.data());
	// remove the last character, which is an \n
	data.pop_back();
	return data;
}

void send_(tcp::socket &socket, const string &message) {
	asio::write(socket, asio::buffer(message + "\n"));
}

int main() {
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 1234;
	asio::error_code error;

	asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, error);
	if(error.value() != 0) {
		// Provided IP address is invalid. Breaking execution.
		std::cout
				<< "Failed to parse the IP address. Error code = "
				<< error.value() << ". Message: " << error.message();
		return error.value();
	}

	tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);

	asio::io_service io_service;
	//socket creation
	tcp::socket socket(io_service);
	//connection
	socket.connect(ep, error);
	if(error.value() != 0) {
		// Failed to open the socket.
		std::cout
				<< "Failed to open the socket! Error code = "
				<< error.value() << ". Message: " << error.message();
		return error.value();
	}

	while(true) {
		string msg;
		std::getline(std::cin, msg);
		send_(socket, msg);

		// getting response from server
		asio::streambuf receive_buffer;
		string message = read_(socket);
		cout << "Message from server: " << message << endl;
	}

	return 0;
}
