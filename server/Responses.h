#ifndef NTECHLAB_TESTTASK_RESPONSES_H
#define NTECHLAB_TESTTASK_RESPONSES_H
#include <boost/beast.hpp>

class Responses {
    using Response = boost::beast::http::response<boost::beast::http::string_body>;

public:
    static Response badRequest(unsigned int version, bool keepAlive, boost::beast::string_view what);
    static Response notFound(unsigned int version, bool keepAlive, boost::beast::string_view what);
    static Response serverError(unsigned int version, bool keepAlive, boost::beast::string_view what);
};


#endif//NTECHLAB_TESTTASK_RESPONSES_H
