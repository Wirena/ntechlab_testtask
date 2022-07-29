#include "HTTPServer.h"
#include <iostream>

using tcp = boost::asio::ip::tcp;
using error_code = boost::system::error_code;

// These functions have to be defined by user if BOOST_NO_EXCEPTIONS is defined
void boost::throw_exception(std::exception const &e) {
    std::cerr << "Aborting" << std::endl;
    abort();
}
void boost::throw_exception(std::exception const &, boost::source_location const &) {
    std::cerr << "Aborting" << std::endl;
    abort();
}


int main(int argc, char *argv[]) {
    HTTPServer httpServer;
    httpServer.bindTo("127.0.0.1","8080");
    httpServer.stopOnSignals({SIGINT, SIGTERM});
    httpServer.setThreadNumber(3);
    httpServer.runBlocking();

}