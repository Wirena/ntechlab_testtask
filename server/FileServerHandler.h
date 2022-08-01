#ifndef NTECHLAB_TESTTASK_FILESERVERHANDLER_H
#define NTECHLAB_TESTTASK_FILESERVERHANDLER_H
#include "HTTPConnection.h"
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/utility/string_view.hpp>
#include <cassert>
#include <string>
#include <utility>

class FileServerHandler {
    const std::string rootDirectory;
    const std::string rootUrl;

    static boost::string_view mimeType(boost::string_view path);

public:
    FileServerHandler(std::string rootDir, std::string rootUrl) : rootDirectory(std::move(rootDir)), rootUrl(std::move(rootUrl)) {
        assert(rootDirectory.ends_with('/'));
    }

    void operator()(boost::beast::http::message<true, boost::beast::http::vector_body<char>> &&msg, HTTPConnection::WriteCallback callback);
};


#endif//NTECHLAB_TESTTASK_FILESERVERHANDLER_H
