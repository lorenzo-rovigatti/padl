#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <thread>
#include <asio.hpp>
#include <RS-232/rs232.h>
#include <tclap/CmdLine.h>

#include "strings.h"

using namespace asio;
using ip::tcp;

class TCPClient {
public:
	TCPClient(std::string raw_ip_address, unsigned short port);

	void connect();
	void connect_dummy();
	std::string read();
	void write(const std::string &message);

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
	bool _is_dummy = false;
};

TCPClient::TCPClient(std::string raw_ip_address, unsigned short port) : _socket(_io_service) {
	_creation_time = _time();
	_last_write_time = _creation_time;

	asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, _error);
	if(_error.value() != 0) {
		// Provided IP address is invalid. Breaking execution.
		std::cerr
				<< "Failed to parse the IP address. Error code = "
				<< _error.value() << ". Message: " << _error.message() << std::endl;;
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
				<< _error.value() << ". Message: " << _error.message() << std::endl;;
		exit(1);
	}
}

void TCPClient::connect_dummy() {
	_is_dummy = true;

	std::srand(std::time(NULL));
}

std::string TCPClient::read() {
	_last_read_time = _time();

	if(_is_dummy) {
		std::stringstream ss;
		ss << std::rand() % 1024;
		for(int i = 0; i < 8; i++) {
			ss << "," << std::rand() % 1024;
		}
		return ss.str();
	}

	asio::streambuf buf;
	asio::read_until(_socket, buf, "\n");
	std::string data = asio::buffer_cast<const char*>(buf.data());
	// remove the last character, which is an \n
	data.pop_back();
	return data;
}

void TCPClient::write(const std::string &message) {
	_last_write_time = _time();

	if(!_is_dummy) {
		asio::write(_socket, asio::buffer(message + "\r\n"));
	}
}

uint64_t TCPClient::last_write_time() {
	return _last_write_time - _creation_time;
}

uint64_t TCPClient::last_read_time() {
	return _last_read_time - _creation_time;
}

std::vector<int> parse_message(const std::string &message) {
	auto spl = utils::split(message, ",");
	std::vector<int> results(spl.size() / 3);
	for(int i = 0; i < spl.size() / 3; i++) {
		results[i] = utils::lexical_cast<int>(spl[2 * i + 2]);
	}

	return results;
}

std::string current_time() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) -
			std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%T") << "." << std::setfill('0') << std::setw(3) << ms.count();
	return ss.str();
}

int main(int argc, char *argv[]) {
	try {
		TCLAP::CmdLine cmd("PADL - Polling Asincrono di DL", ' ', "0.1");

		TCLAP::UnlabeledValueArg<std::string> ip_arg("ip", "The IP address of the DL device", true, "127.0.0.1", "an IP address (e.g. 192.168.0.1)");
		TCLAP::UnlabeledValueArg<int> port_arg("port", "The TCP port of the DL device", true, 6000, "a port number (e.g. 6000)");

		TCLAP::SwitchArg dummy_arg("d", "dummy", "Generate synthetic data", false);

		TCLAP::ValueArg<int> ms_arg("s", "sleep", "Sleeping time between sendings (in milliseconds)", false, 0, "milliseconds");

		TCLAP::ValueArg<int> com_port_arg("p", "serial-port", "The COM port number of the serial port to which the output will be printed", false, -1, "COM port number (e.g. 0)");
		TCLAP::ValueArg<int> baud_rate_arg("b", "baudrate", "Baudrate of the serial connection, defaults to 9600", false, 9600, "bauds");
		TCLAP::ValueArg<std::string> mode_arg("", "mode", "Mode of the serial connection, defaults to 8N1", false, "8N1", "serial mode");

		cmd.add(ip_arg);
		cmd.add(port_arg);
		cmd.add(dummy_arg);
		cmd.add(ms_arg);
		cmd.add(com_port_arg);
		cmd.add(baud_rate_arg);
		cmd.add(mode_arg);

		cmd.parse(argc, argv);

		std::string raw_ip_address(ip_arg.getValue());
		unsigned short port_num = port_arg.getValue();
		bool dummy = dummy_arg.getValue();

		auto sleep_duration = std::chrono::milliseconds(ms_arg.getValue());

		bool write_com = false;
		int com_port_number = com_port_arg.getValue();
		if(com_port_number >= 0) {
			write_com = true;
			int baud_rate = baud_rate_arg.getValue();
			std::string mode = mode_arg.getValue();
			// open the COM port
			RS232_OpenComport(com_port_number, baud_rate, mode.c_str(), 0);
		}

		TCPClient client(raw_ip_address, port_num);

		if(dummy) {
			client.connect_dummy();
		}
		else {
			client.connect();
		}

		while(true) {
			std::string msg;
			client.write("MS");

			// getting response from server
			asio::streambuf receive_buffer;
			std::string message = client.read();
			auto sensor_values = parse_message(message);
			uint64_t average_time = (client.last_write_time() + client.last_read_time()) / 2;

			if(write_com) {
				uint n_values = sensor_values.size();

				std::stringstream ss;
				ss << n_values << " ";

				for(auto &value : sensor_values) {
					ss << " " << value;
				}
				ss << '\n';
				std::string output = ss.str();

				RS232_cputs(com_port_number, output.c_str());
			}
			else {
				std::stringstream ss;
				ss << average_time << " " << current_time();

				for(auto &value : sensor_values) {
					ss << " " << value;
				}
				std::string output = ss.str();

				std::cout << output << std::endl;
			}

			std::this_thread::sleep_for(sleep_duration);
		}

		if(write_com) {
			RS232_CloseComport(com_port_number);
		}

	}
	catch(TCLAP::ArgException &e) {
		std::cerr << "ERROR: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	return 0;
}
