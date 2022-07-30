#include "HTTPServer.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <charconv>
#include <utility>

HTTPServer::HTTPServer() : tcpListener(ioc, muxFunction) {
}

HTTPServer::error_code HTTPServer::bindTo(std::string_view ipAddress, std::string_view port) {
    error_code err;
    const auto address = boost::asio::ip::make_address(ipAddress, err);
    if (err) {
        fail(err, "Failed to make address");
        return err;
    }
    unsigned short iPort;
    const auto res = std::from_chars(port.cbegin(), port.cend(), iPort);
    if (res.ec == std::errc::invalid_argument) {
        fail(error_code(), "Failed to make address");
        return error_code();
    }

    err = tcpListener.bind({address, iPort});
    if (err) fail(error_code(), "Failed to bind address and start listening");
    return err;
}

void HTTPServer::runBlocking() {
    threads.clear();
    threads.reserve(threadsNumber - 1);
    tcpListener.listen();
    for (auto i = 0; i <= threadsNumber - 1; ++i)
        threads.emplace_back([this] { ioc.run(); });
    serverIsRunning = true;
    ioc.run();
    for (auto &thread : threads) thread.join();
    serverIsRunning = false;
    error_code err;
    signals->cancel(err);
    if (err)
        fail(err, "Signal_set.cancel failed");
    signals->clear(err);
    if (err)
        fail(err, "Signal_set.clear failed");
}

void HTTPServer::runNonBlocking() {
    threads.clear();
    threads.reserve(threadsNumber);
    tcpListener.listen();
    serverIsRunning = true;
    for (auto i = 0; i < threadsNumber; ++i)
        threads.emplace_back([this] { ioc.run(); });
}

bool HTTPServer::setHandler(const Endpoint &endpoint, const HandlerFunc &handler) {
    std::unique_lock lock(mapMutex);
    if (muxMap.contains(endpoint)) return false;
    muxMap.emplace(std::pair{endpoint, handler});
    return true;
}

bool HTTPServer::setHandler(const Endpoint &endpoint, HandlerFunc &&handler) {
    std::unique_lock lock(mapMutex);
    if (muxMap.contains(endpoint)) return false;
    muxMap.emplace(std::pair{endpoint, std::move(handler)});
    return true;
}


boost::system::error_code HTTPServer::stopBlockingOnSignals(const std::initializer_list<int> &sigList) {
    if (!signals) {
        signals = std::make_unique<boost::asio::signal_set>(ioc);
    }
    error_code err;
    signals->clear(err);
    if (err) {
        fail(err, "Failed to clear signal_set");
        return err;
    }

    for (const int signal : sigList) {
        signals->add(signal, err);
        if (err) {
            fail(err, "Failed to add signal");
            return err;
        }
    }
    signals->async_wait(boost::bind(&boost::asio::io_service::stop, &ioc));
    return err;
}

bool HTTPServer::setThreadNumber(int number) {
    if (number <= 1) return false;
    threadsNumber = number;
    return true;
}

void HTTPServer::stopNonBlocking() {
    ioc.stop();
    for (auto &thread : threads) thread.join();
    serverIsRunning = false;
}
