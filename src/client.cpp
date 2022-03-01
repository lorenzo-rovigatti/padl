#include <iostream>
#include <chrono>
#include <thread>
#include <asio.hpp>
#include "strings.h"

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

	uint64_t last_write_time();
	uint64_t last_read_time();
private:
	uint64_t _time();
	uint64_t _creation_time;
	uint64_t _last_write_time;
	uint64_t _last_read_time;
	asio::error_code _error;
	asio::io_service _io_service;
	tcp::endpoint _endpoint;
	tcp::socket _socket;
};

TCPClient::TCPClient(std::string raw_ip_address, unsigned short port) : _socket(_io_service) {
	_creation_time = _time();
	_last_write_time = _creation_time;

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

uint64_t TCPClient::_time() {
	auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::microseconds>(time).count();
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
	_last_read_time = _time();

	asio::streambuf buf;
	asio::read_until(_socket, buf, "\n");
	string data = asio::buffer_cast<const char*>(buf.data());
	// remove the last character, which is an \n
	data.pop_back();
	return data;
}

void TCPClient::write(const string &message) {
	_last_write_time = _time();

	asio::write(_socket, asio::buffer(message + "\r\n"));
}

uint64_t TCPClient::last_write_time() {
	return _last_write_time - _creation_time;
}

uint64_t TCPClient::last_read_time() {
	return _last_read_time - _creation_time;
}

std::vector<int> parse_message(const string &message) {
	auto spl = utils::split(message, ",");
	std::vector<int> results(spl.size() / 3);
	for(int i = 0; i < spl.size() / 3; i++) {
		results[i] = utils::lexical_cast<int>(spl[2 * i + 2]);
	}

	return results;
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		std::cerr << "Usage is " << argv[0] << " ip port [sleep in ms]" << endl;
		exit(1);
	}

	string raw_ip_address(argv[1]);
	unsigned short port_num = std::atoi(argv[2]);

	auto sleep_duration = std::chrono::milliseconds(0);
	if(argc > 3) {
		sleep_duration = std::chrono::milliseconds(std::atoi(argv[3]));
	}

	TCPClient client(raw_ip_address, port_num);
	client.connect();

	while(true) {
		string msg;
		client.write("MS");

		// getting response from server
		asio::streambuf receive_buffer;
		string message = client.read();
		auto sensor_values = parse_message(message);
		cout << client.last_write_time() << " " << client.last_read_time();
		for(auto &value : sensor_values) {
			cout << " " << value;
		}
		cout << endl;

		std::this_thread::sleep_for(sleep_duration);
	}

	return 0;
}
