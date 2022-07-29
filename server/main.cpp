#include "TcpListner.hpp"
#include "HTTPConnection.h"
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/bind.hpp>
#include <iostream>


using tcp = boost::asio::ip::tcp;
using error_code = boost::system::error_code;


void boost::throw_exception(std::exception const &e) {
    std::cerr << "aborting" << std::endl;
    abort();
}

void boost::throw_exception(std::exception const &, boost::source_location const &) {
    std::cerr << "aborting" << std::endl;
    abort();
}


int main(int argc, char *argv[]) {
    boost::system::error_code err;
    boost::asio::io_context ioc;
    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(boost::bind(&boost::asio::io_service::stop, &ioc));
    TcpListner<HTTPConnection> listner(ioc);
    listner.listen(tcp::endpoint(boost::asio::ip::make_address("127.0.0.1", err), 8080));
    ioc.run();
}