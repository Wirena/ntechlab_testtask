#include "HTTPConnection.h"
#include "fail.h"
#include <boost/asio/dispatch.hpp>

using tcp = boost::asio::ip::tcp;
using error_code = boost::system::error_code;
using namespace boost;
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;


void HTTPConnection::connect() {
    asio::dispatch(stream.get_executor(),
                   beast::bind_front_handler(
                           &HTTPConnection::read,
                           shared_from_this()));
}
void HTTPConnection::read() {
    request = {};
    stream.expires_after(std::chrono::seconds(30));

    http::async_read(stream, buffer, parser,
                     beast::bind_front_handler(
                             &HTTPConnection::onRead,
                             shared_from_this()));
}
void HTTPConnection::onRead(error_code err, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (err == http::error::end_of_stream)
        return close();
    if (err) return fail(err, "Async_read failed");
    if(parser.chunked()|| !parser.is_done()){
        fail(error_code(), "Chunked request are not supported");
        http::response<http::string_body> res{http::status::internal_server_error, parser.get().version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(parser.keep_alive());
        res.body() = "Chunked request are not supported";
        res.prepare_payload();
        writeCallback(std::move(res));
        return close();
    }
    muxFunction(parser.release(),writeCallback);
    //handle_request(std::move(request), writeCallback);
}
void HTTPConnection::close() {
    beast::error_code err;
    stream.socket().shutdown(tcp::socket::shutdown_send, err);
}

void HTTPConnection::onWrite(bool close, boost::system::error_code err, std::size_t bytes_transferred) {
    if (err) return fail(err, "write");
    if (close) return this->close();
    res = nullptr;
    read();
}


template<bool isRequest, class Body, class Fields>
void HTTPConnection::WriteCallback::operator()(boost::beast::http::message<isRequest, Body, Fields> &&msg) const {
    auto message = std::make_shared<http::message<isRequest, Body, Fields>>(std::move(msg));
    connection.res = message;
    http::async_write(
            connection.stream,
            *message,
            beast::bind_front_handler(
                    &HTTPConnection::onWrite,
                    connection.shared_from_this(),
                    message->need_eof()));
}