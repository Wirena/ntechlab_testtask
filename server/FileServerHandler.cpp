#include "FileServerHandler.h"
#include "Responses.h"
#include <tuple>

boost::string_view FileServerHandler::mimeType(boost::string_view path) {
    using boost::beast::iequals;
    auto const ext = [&path] {
        auto const pos = path.rfind(".");
        if (pos == boost::string_view::npos)
            return boost::string_view{};
        return path.substr(pos);
    }();
    if (iequals(ext, ".htm")) return "text/html";
    if (iequals(ext, ".html")) return "text/html";
    if (iequals(ext, ".php")) return "text/html";
    if (iequals(ext, ".css")) return "text/css";
    if (iequals(ext, ".txt")) return "text/plain";
    if (iequals(ext, ".js")) return "application/javascript";
    if (iequals(ext, ".json")) return "application/json";
    if (iequals(ext, ".xml")) return "application/xml";
    if (iequals(ext, ".swf")) return "application/x-shockwave-flash";
    if (iequals(ext, ".flv")) return "video/x-flv";
    if (iequals(ext, ".png")) return "image/png";
    if (iequals(ext, ".jpe")) return "image/jpeg";
    if (iequals(ext, ".jpeg")) return "image/jpeg";
    if (iequals(ext, ".jpg")) return "image/jpeg";
    if (iequals(ext, ".gif")) return "image/gif";
    if (iequals(ext, ".bmp")) return "image/bmp";
    if (iequals(ext, ".ico")) return "image/vnd.microsoft.icon";
    if (iequals(ext, ".tiff")) return "image/tiff";
    if (iequals(ext, ".tif")) return "image/tiff";
    if (iequals(ext, ".svg")) return "image/svg+xml";
    if (iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}


void FileServerHandler::operator()(boost::beast::http::message<true, boost::beast::http::vector_body<char>> &&msg, HTTPConnection::WriteCallback callback) {
    namespace http = boost::beast::http;
    if (msg.method() != http::verb::get) {
        callback(Responses::badRequest(msg.version(), msg.keep_alive(), "Use GET method to access files"));
        return;
    }
    boost::string_view urlView = msg.target();
    urlView.remove_prefix(rootUrl.length() + 1);
    if (urlView.length() == 0) urlView = "index.html";
    else if (urlView.length() == 0 || urlView[0] == '/' || urlView.find("..") != boost::string_view::npos) {
        callback(Responses::badRequest(msg.version(), msg.keep_alive(), "Illegal path"));
        return;
    }
    const std::string fsPath = rootDirectory + std::string(urlView);
    boost::beast::error_code ec;
    http::file_body::value_type body;
    body.open(fsPath.c_str(), boost::beast::file_mode::scan, ec);

    if (ec == boost::beast::errc::no_such_file_or_directory)
        return callback(Responses::notFound(msg.version(), msg.keep_alive(), fsPath));
    if (ec)
        return callback(Responses::serverError(msg.version(), msg.keep_alive(), "Failed to open file"));

    // Respond to GET request
    http::response<http::file_body> res{std::piecewise_construct, std::make_tuple(std::move(body)), std::make_tuple(http::status::ok, msg.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mimeType(fsPath));
    res.prepare_payload();
    res.keep_alive(msg.keep_alive());
    return callback(std::move(res));
}
