#ifndef NTECHLAB_TESTTASK_RESPONSES_H
#define NTECHLAB_TESTTASK_RESPONSES_H
#include <boost/beast.hpp>

class Responses {
public:
    void badRequest(boost::beast::string_view what);
    void notFound(boost::beast::string_view what);
    void serverError(boost::beast::string_view what);
};


#endif//NTECHLAB_TESTTASK_RESPONSES_H
