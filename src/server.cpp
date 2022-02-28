#include <iostream>
#include <asio.hpp>

using namespace asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

string read_(tcp::socket & socket) {
       asio::streambuf buf;
       asio::read_until( socket, buf, "\n" );
       string data = asio::buffer_cast<const char*>(buf.data());
       return data;
}

void send_(tcp::socket & socket, const string& message) {
       const string msg = message + "\n";
       asio::write( socket, asio::buffer(message) );
}

int main() {
      asio::io_service io_service;
      //listen for new connection
      tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234 ));
      //socket creation 
      tcp::socket socket_(io_service);
      //waiting for connection
      acceptor_.accept(socket_);
      //read operation
      string message = read_(socket_);
      cout << message << endl;
      //write operation
      send_(socket_, "Hello From Server!");
      cout << "Server sent Hello message to Client!" << endl;
      return 0;
}
