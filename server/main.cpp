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

const auto handler = [](boost::beast::http::message<true, boost::beast::http::string_body> &&msg, HTTPConnection::WriteCallback callback) {
    namespace http = boost::beast::http;
    http::response<http::string_body> res{http::status::ok, msg.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(false);
    res.body() = "EveryThing's cool!";
    res.prepare_payload();
    callback(std::move(res));
};

int main(int argc, char *argv[]) {
    HTTPServer httpServer;
    httpServer.bindTo("127.0.0.1", "8080");
    //httpServer.stopBlockingOnSignals({SIGINT, SIGTERM});
    httpServer.setThreadNumber(1);
    httpServer.setHandler({"POST", "/"}, handler);
    //httpServer.runBlocking();
    httpServer.runNonBlocking();
    sleep(10);
    httpServer.stopNonBlocking();
}