#include "Responses.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

Responses::Response Responses::badRequest(unsigned int version, boost::beast::string_view what) {
    http::response<http::string_body> res{http::status::bad_request, version};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(false);
    res.body() = "Bad Request " + std::string(what);
    res.prepare_payload();
    return res;
}

Responses::Response Responses::notFound(unsigned int version, boost::beast::string_view what) {
    http::response<http::string_body> res{http::status::not_found, version};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(false);
    res.body() = "Resource " + std::string(what) + " not found.";
    res.prepare_payload();
    return res;
}


Responses::Response Responses::serverError(unsigned int version, boost::beast::string_view what) {
    http::response<boost::beast::http::string_body> res{http::status::internal_server_error, version};
    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "text/html");
    res.keep_alive(false);
    res.body() = "An error occurred: '" + std::string(what) + "'";
    res.prepare_payload();
    return res;
}
