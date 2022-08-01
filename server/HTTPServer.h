#ifndef NTECHLAB_TESTTASK_HTTPSERVER_H
#define NTECHLAB_TESTTASK_HTTPSERVER_H
#include "HTTPConnection.h"
#include "MuxMap.hpp"
#include "Responses.h"
#include "TcpListener.hpp"
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/utility/string_view.hpp>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>


using HandlerFunc = std::function<void(boost::beast::http::message<true, boost::beast::http::vector_body<char>> &&, HTTPConnection::WriteCallback)>;

class HTTPServer {
    using error_code = boost::system::error_code;
    boost::asio::io_context ioc;

    std::unique_ptr<boost::asio::signal_set> signals;
    HTTPConnection::MuxFunction muxFunction = [this](boost::beast::http::message<true, boost::beast::http::vector_body<char>> &&msg, HTTPConnection::WriteCallback callback) {
        std::shared_lock lock(mapMutex);
        if (muxMap.containsHandler(msg.target())) {
            muxMap.callHandler(msg.target(), std::move(msg), callback);
        } else {
            callback(Responses::notFound(msg.version(), msg.keep_alive(), msg.target()));
        }
    };

    TcpListener<HTTPConnection> tcpListener;
    std::shared_mutex mapMutex;
    MuxMap<HandlerFunc> muxMap;

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

    bool setHandler(boost::string_view path, bool handleChildren, const HandlerFunc &handler);

    bool setHandler(boost::string_view path, bool handleChildren, HandlerFunc &&handler);

    bool deleteHandler(boost::string_view path);
};


#endif//NTECHLAB_TESTTASK_HTTPSERVER_H
