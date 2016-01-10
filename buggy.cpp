#include <iostream>
#include <asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/array.hpp>

#define PORT 6000

using asio::ip::udp;

int main(int argc, char *argv[]) {
  try {
    asio::io_service io;
    udp::socket socket(io, udp::endpoint(udp::v4(), PORT));
    while (1) {
      boost::array<short int, 2> msg;
      asio::error_code error;
      udp::endpoint ground_endpoint;
      socket.receive_from(asio::buffer(msg), ground_endpoint);
      if (error && error != asio::error::message_size)
        throw asio::system_error(error);

      std::cout << msg[0] << " - " << msg[1] << std::endl;
    }
  }
  catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
