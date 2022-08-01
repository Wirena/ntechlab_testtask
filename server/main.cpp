#include "FileServerHandler.h"
#include "HTTPServer.h"
#include "JPEGReader.h"
#include "JPEGWriter.h"
#include "MuxMap.hpp"
#include "Responses.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <limits>
#include <vector>


using tcp = boost::asio::ip::tcp;
using error_code = boost::system::error_code;

// These functions have to be defined by user if BOOST_NO_EXCEPTIONS is defined
void boost::throw_exception(std::exception const &e) {
    std::cerr << "Aborting " << e.what() << std::endl;
    abort();
}

void boost::throw_exception(std::exception const &e, boost::source_location const &) {
    std::cerr << "Aborting " << e.what() << std::endl;
    abort();
}

void mirrorScanline(std::vector<unsigned char> *vector, int pixelSize) {
    std::vector<unsigned char> buf(pixelSize, 0);
    for (unsigned i = 0; i < vector->size() / 2; i += pixelSize) {
        std::copy(vector->begin() + i, vector->begin() + i + pixelSize, buf.begin());
        std::copy(vector->end() - i - pixelSize, vector->end() - i, vector->begin() + i);
        std::copy(buf.begin(), buf.end(), vector->end() - i - pixelSize);
    }
}

std::vector<unsigned char> mirrorImage(const std::vector<char> &fileIn) {
    JPEGReader reader(fileIn.data(), fileIn.size());
    JPEGWriter writer;
    reader.decompress();
    const auto imageInfo = reader.getImageInfo();
    writer.setImageInfo(imageInfo);
    writer.setQuality(100);
    writer.prepareForWrite();
    std::vector<unsigned char> scanline;
    scanline.resize(imageInfo.width * imageInfo.pixelSize);
    while (reader.readScanline(scanline.data(), scanline.size())) {
        mirrorScanline(&scanline, imageInfo.pixelSize);
        writer.writeScanline(scanline.data(), scanline.size());
    }
    writer.finishCompressing();
    reader.finishDecompressing();
    assert(writer.allScanlinesWritten() && reader.allScanLinesRead());
    const auto [ptr, len] = writer.releaseFileBuffer();
    std::vector<unsigned char> file;
    file.resize(len);
    std::copy(ptr.get(), ptr.get() + len, file.begin());
    return file;
}

const auto imageMirrorHandler = [](boost::beast::http::message<true, boost::beast::http::vector_body<char>> &&msg, HTTPConnection::WriteCallback callback) {
    namespace http = boost::beast::http;
    if (msg.method() != http::verb::post) {
        callback(Responses::badRequest(msg.version(), msg.keep_alive(), "Use POST method to mirror image"));
        return;
    }
    std::vector<unsigned char> responseBody;
    try {
        responseBody = mirrorImage(msg.body());
    } catch (const SmallBufferException &e) {
        callback(Responses::serverError(msg.version(), msg.keep_alive(), e.what()));
        return;
    } catch (const std::runtime_error &e) {
        callback(Responses::badRequest(msg.version(), msg.keep_alive(), e.what()));
        return;
    } catch (const std::exception &e) {
        callback(Responses::serverError(msg.version(), msg.keep_alive(), "Something went wrong while parsing image"));
        return;
    }
    http::response<http::vector_body<unsigned char>> res{http::status::ok, msg.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/octet-stream");
    res.keep_alive(msg.keep_alive());
    res.body() = std::move(responseBody);
    res.prepare_payload();
    callback(std::move(res));
};


void printHelp() {
    std::cout << "Options:" << std::endl
              << "-j arg - set number of threads" << std::endl
              << "-p arg - set port number" << std::endl
              << "-b arg - set ipv4 address" << std::endl
              << "-i     - interactive mode" << std::endl
              << "-h     - display help" << std::endl;
};


bool parseArguments(int argc, char *argv[], int *threadsNumber, std::string *ip, std::string *port, bool *help, bool *interactive) {
    int c;
    char *endPtr;
    long argNumber;
    while ((c = getopt(argc, argv, "hib:p:j:")) != -1)
        switch (c) {
            case 'b':
                *ip = optarg;
                break;
            case 'p':
                *port = optarg;
                break;
            case 'i':
                *interactive = true;
                break;
            case 'j':
                argNumber = std::strtol(optarg, &endPtr, 10);
                if (endPtr == optarg || *endPtr != '\0' ||
                    argNumber > std::numeric_limits<int>::max() || argNumber <= 0) {
                    std::cerr << "Invalid argument for -j option" << std::endl;
                    return false;
                }
                *threadsNumber = static_cast<int>(argNumber);
                break;
            case 'h':
                *help = true;
                break;
            case '?':
                std::cerr << "Invalid option " << static_cast<char>(optopt) << std::endl;
                return false;
            default:
                std::cerr << "Invalid option " << c << std::endl;
                return false;
        }
    return true;
}


int main(int argc, char *argv[]) {
    std::string ipAddress{"0.0.0.0"};
    std::string port{"8080"};
    int threadsNumber = 1;
    bool interactive = true, help = false;
    if (!parseArguments(argc, argv, &threadsNumber, &ipAddress, &port, &help, &interactive))
        return 1;
    if (help) {
        printHelp();
        return 1;
    }

    std::cout << "Starting server on " << ipAddress << ':' << port <<std::endl<< "Number of threads: " << threadsNumber <<std::endl;
    HTTPServer httpServer;

    if (httpServer.bindTo(ipAddress, port)) {
        std::cerr << "Failed to bind" << std::endl;
        return 1;
    }
    if (httpServer.stopBlockingOnSignals({SIGINT, SIGTERM})) {
        std::cerr << "Failed to set signal handler" << std::endl;
        return 1;
    }
    httpServer.setThreadNumber(threadsNumber);
    if (interactive) {
        const char url[]{"/interactive"};
        const char directory[]{"../www/"};
        if (!httpServer.setHandler(url, true, FileServerHandler(directory, url))) {
            std::cerr << "Failed to set handler fileserver" << std::endl;
            return 1;
        }
        std::cout << "Using interactive mode, visit page at " << url << std::endl;
    }

    if (!httpServer.setHandler("/mirror", false, imageMirrorHandler)) {
        std::cerr << "Failed to set handler for mirroring image" << std::endl;
        return 1;
    }
    std::cout<<"Mirror image endpoint at /mirror"<<std::endl;


    httpServer.runBlocking();
    return 0;
}