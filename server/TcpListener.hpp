#ifndef NTECHLAB_TESTTASK_TCPLISTNER_HPP
#define NTECHLAB_TESTTASK_TCPLISTNER_HPP
#include "fail.h"
#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <iostream>


template<class Connection>
class TcpListner {
    const typename Connection::MuxFunction &muxFunction;
    using tcp = boost::asio::ip::tcp;
    using error_code = boost::system::error_code;
    boost::asio::io_context &ioc;
    tcp::acceptor acceptor;

    void accept();
    void onAccept(error_code ec, tcp::socket socket);

public:
    explicit TcpListner(boost::asio::io_context &ioc, const typename Connection::MuxFunction &muxFunction) : acceptor(ioc), ioc(ioc), muxFunction(muxFunction) {}

    [[nodiscard]] error_code bind(const tcp::endpoint &endpoint);
    [[nodiscard]] error_code listen();
    [[nodiscard]] error_code close();
    ~TcpListner() { std::cout << "Exiting" << std::endl; }
};


template<class Connection>
boost::system::error_code TcpListner<Connection>::bind(const tcp::endpoint &endpoint) {
    error_code err;
    acceptor.open(endpoint.protocol(), err);
    if (err) {
        fail(err, "Acceptor.open failed");
        return err;
    }
    acceptor.set_option(boost::asio::socket_base::reuse_address(true), err);
    if (err) {
        fail(err, "Acceptor.setoption reuse_address failed");
        return err;
    }

    acceptor.bind(endpoint, err);
    if (err) {
        fail(err, "Acceptor.bind failed");
        return err;
    }
    return error_code();
}

template<class Connection>
void TcpListner<Connection>::onAccept(error_code err, tcp::socket socket) {
    if (err) {
        fail(err, "Accept failed");
        return;
    } else {
        std::make_shared<Connection>(std::move(socket),muxFunction)->connect();
    }
    accept();
}

template<class Connection>
void TcpListner<Connection>::accept() {
    acceptor.async_accept(boost::asio::make_strand(ioc), boost::beast::bind_front_handler(
                                                                 &TcpListner::onAccept, this));
}

template<class Connection>
boost::system::error_code TcpListner<Connection>::listen() {
    error_code err;
    acceptor.listen(boost::asio::socket_base::max_listen_connections, err);
    if (err) {
        fail(err, "Acceptor.listen failed");
        return err;
    }
    accept();
    return error_code();
}

template<class Connection>
boost::system::error_code TcpListner<Connection>::close() {
    error_code err;
    acceptor.close(err);
    if (err)
        fail(err, "Acceptor.close failed");
    return err;
}


#endif//NTECHLAB_TESTTASK_TCPLISTNER_HPP