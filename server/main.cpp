#include "HTTPServer.h"
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <limits>

void printHelp(){};

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

bool parseArguments(int argc, char *argv[], int *threadsNumber, std::string *ip, std::string *port) {
    int c;
    char *endPtr;
    long argNumber;
    while ((c = getopt(argc, argv, "hi:p:j:")) != -1)
        switch (c) {
            case 'i':
                *ip = optarg;
                break;
            case 'p':
                *port = optarg;
                break;
            case 'j':
                argNumber = std::strtol(optarg, &endPtr, 10);
                if (endPtr == optarg || *endPtr != '\0' ||
                    argNumber > std::numeric_limits<int>::max() || argNumber <= 0) {
                    std::cerr << "Invalid argument for -j option" << std::endl;
                    return false;
                }
                *threadsNumber = static_cast<int>(argNumber);
                break;
            case '?':
                std::cerr << "Invalid option " << static_cast<char>(optopt) << std::endl;
                return false;
            default:
                std::cerr << "Invalid option " << c << std::endl;
                return false;
        }
    return true;
}


int main(int argc, char *argv[]) {
    std::string ipAddress{"127.0.0.1"};
    std::string port{"8080"};
    int threadsNumber = 1;
    if (!parseArguments(argc, argv, &threadsNumber, &ipAddress, &port))
        return 1;
    std::cout << "Starting server on " << ipAddress << ':' << port << " using " << threadsNumber << " threads" << std::endl;
    HTTPServer httpServer;
    httpServer.bindTo(ipAddress, port);
    httpServer.stopBlockingOnSignals({SIGINT, SIGTERM});
    httpServer.setThreadNumber(threadsNumber);
    httpServer.setHandler({"POST", "/"}, handler);
    httpServer.runBlocking();
    return 0;
}