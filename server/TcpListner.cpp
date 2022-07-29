#include "TcpListner.h"
#include "fail.h"
#include <boost/beast.hpp>
#include <boost/bind.hpp>
#include <iostream>

using tcp = boost::asio::ip::tcp;
using error_code = boost::system::error_code;

error_code TcpListner::listen(const tcp::endpoint &endpoint) {
    error_code err;
    acceptor.open(endpoint.protocol(), err);
    if (err) {
        fail(err, "Acceptor.open failed");
        return err;
    }

    acceptor.bind(endpoint, err);
    if (err) {
        fail(err, "Acceptor.bind failed");
        return err;
    }
    acceptor.listen(
            boost::asio::socket_base::max_listen_connections, err);
    if (err) {
        fail(err, "Acceptor.listen failed");
    }
    accept();
    return error_code();
}


void TcpListner::onAccept(error_code err, tcp::socket socket) {
    std::cout << "onAccept" << std::endl;
    if (err) {
        fail(err, "Accept failed");
        return;
    } else {
        socket.close();
    }
    accept();
}


void TcpListner::accept() {
    acceptor.async_accept(ioc, boost::beast::bind_front_handler(
                                       &TcpListner::onAccept, this));
    std::cout << "Accept" << std::endl;
}
