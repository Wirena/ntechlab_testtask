#ifndef NTECHLAB_TESTTASK_HTTPCONNECTION_H
#define NTECHLAB_TESTTASK_HTTPCONNECTION_H
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/system.hpp>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <string>


class HTTPConnection : public std::enable_shared_from_this<HTTPConnection> {
public:
    class WriteCallback {
        HTTPConnection &connection;

    public:
        explicit WriteCallback(HTTPConnection &connection) : connection(connection) {}
        template<bool isRequest, class Body, class Fields>
        void operator()(boost::beast::http::message<isRequest, Body, Fields> &&msg) const {
            auto message = std::make_shared<boost::beast::http::message<isRequest, Body, Fields>>(std::move(msg));
            connection.res = message;
            boost::beast::http::async_write(connection.stream, *message, boost::beast::bind_front_handler(&HTTPConnection::onWrite, connection.shared_from_this(), message->need_eof()));
        }
    };

private:
    WriteCallback writeCallback;
    boost::beast::tcp_stream stream;
    boost::beast::flat_buffer buffer;
    std::shared_ptr<void> res;
    boost::beast::http::request_parser<boost::beast::http::vector_body<char>> parser;

    void read();
    void onRead(boost::system::error_code err, std::size_t bytes_transferred);
    void close();
    void onWrite(bool close, boost::system::error_code err, std::size_t bytesTransferred);

public:
    using MuxFunction = std::function<void(boost::beast::http::message<true, boost::beast::http::vector_body<char>> &&, HTTPConnection::WriteCallback)>;

    explicit HTTPConnection(boost::asio::ip::tcp::socket &&socket, const MuxFunction &muxFunction) : stream(std::move(socket)),
                                                                                                     writeCallback(*this), muxFunction(muxFunction) {
        parser.body_limit((std::numeric_limits<std::uint64_t>::max)());
    }
    void connect();
    ~HTTPConnection(){close();}
private:
    MuxFunction muxFunction;
};


#endif//NTECHLAB_TESTTASK_HTTPCONNECTION_H
