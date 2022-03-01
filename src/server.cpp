#include <iostream>
#include <asio.hpp>

using namespace asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

string read_from_socket(tcp::socket &socket) {
	asio::streambuf buf;
	asio::read_until(socket, buf, "\n");
	string data = asio::buffer_cast<const char*>(buf.data());
	// remove the last character, which is an \n
	data.pop_back();
	return data;
}

void write_to_socket(tcp::socket &socket, const string &message) {
	asio::write(socket, asio::buffer(message + "\n"));
}

int main() {
	unsigned short port_num = 1234;
	tcp::endpoint ep(tcp::v4(), port_num);

	asio::io_service io_service;
	// listen for new connection
	tcp::acceptor acceptor_(io_service, ep);
	// socket creation
	tcp::socket socket_(io_service);
	// waiting for connection
	acceptor_.accept(socket_);

	while(true) {
		//read operation
		string message = read_from_socket(socket_);
		cout << "Message from client: " << message << endl;
		//write operation
		write_to_socket(socket_, "Message '" + message + "' received");
	}

	return 0;
}
