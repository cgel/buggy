#include <iostream>
#include <asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/array.hpp>
//#include <pigpio.h>

#define PORT 6000
#define PIN 19

using asio::ip::udp;

using Msg = boost::array<short int, 2>;

class Reciver {
 public:
  Reciver(asio::io_service &io_s)
      : socket_(io_s, udp::endpoint(udp::v4(), PORT)),
        failsafe_time(500),
        update_servo_period(100),
        update_servo_timer(io_s, update_servo_period) {
    update_servo_timer.async_wait(
        boost::bind(&Reciver::update_servo_command, this));
    start_async_recive();
  }

 private:
  void start_async_recive() {
    socket_.async_receive(asio::buffer(msg_holder),
                          boost::bind(&Reciver::handle_msg, this));
  }
  
  //Called asynchronously when a new msg arrives
  void handle_msg() {
    msg_time = boost::posix_time::microsec_clock::local_time();
    start_async_recive();
  }

  //Called asynchronously
  void update_servo_command() {
    if (boost::posix_time::microsec_clock::local_time() - msg_time >
        failsafe_time) {
      std::cout << "Failsafe activated!" << std::endl;
      default_msg();
    }
    std::cout << msg_holder[0] << " / " << msg_holder[1] << std::endl; 
    // call again asynchronously
    update_servo_timer.expires_at(update_servo_timer.expires_at() +
                                  update_servo_period);
    update_servo_timer.async_wait(
        boost::bind(&Reciver::update_servo_command, this));
  }

  void default_msg() {
    msg_holder[0] = 0;
    msg_holder[1] = 0;
  }

  boost::posix_time::ptime msg_time =
      boost::posix_time::microsec_clock::local_time();
  boost::posix_time::milliseconds failsafe_time;
  boost::posix_time::milliseconds update_servo_period;
  udp::socket socket_;
  asio::deadline_timer update_servo_timer;

  Msg msg_holder;
};

int main(int argc, char *argv[]) {
  // if(gpioInitialise() < 0) return -1;
  try {
    asio::io_service io_s;
    Reciver reciver(io_s);

    io_s.run();
    // int val = 1400 + msg[1] * 2;
    // gpioServo(PIN, val);
  }
  catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  // gpioTerminate();
  return 0;
}
