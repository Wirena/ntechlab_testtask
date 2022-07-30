#ifndef NTECHLAB_TESTTASK_HTTPSERVER_H
#define NTECHLAB_TESTTASK_HTTPSERVER_H
#include "HTTPConnection.h"
#include "Responses.h"
#include "TcpListener.hpp"
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <functional>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

struct Endpoint {
    boost::beast::http::verb method;
    std::string path;

    inline bool operator==(const Endpoint &other) const {
        return this->method == other.method && this->path == other.path;
    }

    Endpoint(boost::beast::string_view method, boost::beast::string_view path) : path(path), method(boost::beast::http::string_to_verb(method)){};

    Endpoint(boost::beast::http::verb method, boost::beast::string_view path) : method(method), path(path){};
};


namespace std {

    template<>
    struct hash<Endpoint> {
        typedef typename underlying_type<boost::beast::http::verb>::type enumType;

        std::size_t operator()(const Endpoint &e) const {
            return std::hash<std::string>()(e.path) ^ std::hash<enumType>()(static_cast<enumType>(e.method));
        }
    };

}// namespace std


using HandlerFunc = std::function<void(boost::beast::http::message<true, boost::beast::http::string_body> &&, HTTPConnection::WriteCallback)>;
using MuxMap = std::unordered_map<Endpoint, HandlerFunc>;


class HTTPServer {
    using error_code = boost::system::error_code;
    boost::asio::io_context ioc;

    std::unique_ptr<boost::asio::signal_set> signals;
    HTTPConnection::MuxFunction muxFunction = [this](boost::beast::http::message<true, boost::beast::http::string_body> &&msg, HTTPConnection::WriteCallback callback) {
        const auto method = msg.method();
        const auto path = msg.target();
        std::shared_lock lock(mapMutex);
        const Endpoint endpoint{method, path};
        if (muxMap.contains(endpoint)) {
            muxMap[endpoint](std::move(msg), callback);
        } else {
            callback(Responses::notFound(msg.version(), path));
        }
    };

    TcpListener<HTTPConnection> tcpListener;
    std::shared_mutex mapMutex;
    MuxMap muxMap;

    std::vector<std::thread> threads;
    int threadsNumber = 1;
    bool serverIsRunning = false;

public:
    HTTPServer();

    error_code bindTo(std::string_view ipAddress, std::string_view port);

    bool setThreadNumber(int number);

    void runBlocking();

    void runNonBlocking();

    void stopNonBlocking();

    bool isRunning() { return serverIsRunning; }

    error_code stopBlockingOnSignals(const std::initializer_list<int> &sigList);

    bool setHandler(const Endpoint &endpoint, const HandlerFunc &handler);

    bool setHandler(const Endpoint &endpoint, HandlerFunc &&handler);
};


#endif//NTECHLAB_TESTTASK_HTTPSERVER_H
