#include "HTTPConnection.h"
#include "Responses.h"
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
    stream.expires_after(std::chrono::seconds(30));
    http::async_read(stream, buffer, parser,
                     beast::bind_front_handler(
                             &HTTPConnection::onRead,
                             shared_from_this()));
}

void HTTPConnection::onRead(error_code err, std::size_t bytesTransferred) {
    if (err == http::error::end_of_stream)
        return close();
    if (err) return fail(err, "Async_read failed");
    if (parser.chunked() || !parser.is_done()) {
        writeCallback(Responses::serverError(parser.get().version(), false, "Chunked requests are not supported"));
        return close();
    }
    muxFunction(parser.release(), writeCallback);
}

void HTTPConnection::close() {
    beast::error_code err;
    stream.socket().shutdown(tcp::socket::shutdown_send, err);
}

void HTTPConnection::onWrite(bool close, boost::system::error_code err, std::size_t bytesTransferred) {
    if (err)
        return fail(err, "Write Failed");
    if (close) return this->close();
    res = nullptr;
    read();
}
