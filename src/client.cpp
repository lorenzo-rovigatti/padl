#include <iostream>
#include <chrono>
#include <asio.hpp>

using namespace asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

class TCPClient {
public:
	TCPClient(std::string raw_ip_address, unsigned short port);

	void connect();
	string read();
	void write(const string &message);
private:
	uint64_t _creation_time;
	asio::error_code _error;
	asio::io_service _io_service;
	tcp::endpoint _endpoint;
	tcp::socket _socket;
};

TCPClient::TCPClient(std::string raw_ip_address, unsigned short port) : _socket(_io_service) {
	auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
	_creation_time = std::chrono::duration_cast<std::chrono::microseconds>(time).count();

	asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, _error);
	if(_error.value() != 0) {
		// Provided IP address is invalid. Breaking execution.
		std::cerr
				<< "Failed to parse the IP address. Error code = "
				<< _error.value() << ". Message: " << _error.message();
		exit(1);
	}

	_endpoint.address(asio::ip::address::from_string(raw_ip_address));
	_endpoint.port(port);
}

void TCPClient::connect() {
	_socket.connect(_endpoint, _error);
	if(_error.value() != 0) {
		// Failed to open the socket.
		std::cerr
				<< "Failed to open the socket! Error code = "
				<< _error.value() << ". Message: " << _error.message();
		exit(1);
	}
}

string TCPClient::read() {
	asio::streambuf buf;
	asio::read_until(_socket, buf, "\n");
	string data = asio::buffer_cast<const char*>(buf.data());
	// remove the last character, which is an \n
	data.pop_back();
	return data;
}

void TCPClient::write(const string &message) {
	asio::write(_socket, asio::buffer(message + "\n"));
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		std::cerr << "Usage is " << argv[0] << " ip port" << std::endl;
		exit(1);
	}

	std::string raw_ip_address(argv[1]);
	unsigned short port_num = std::atoi(argv[2]);

	TCPClient client(raw_ip_address, port_num);
	client.connect();

	while(true) {
		string msg;
		std::getline(std::cin, msg);
		client.write(msg);

		// getting response from server
		asio::streambuf receive_buffer;
		string message = client.read();
		cout << "Message from server: " << message << endl;
	}

	return 0;
}
