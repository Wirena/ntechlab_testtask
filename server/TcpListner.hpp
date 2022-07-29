#ifndef NTECHLAB_TESTTASK_TCPLISTNER_HPP
#define NTECHLAB_TESTTASK_TCPLISTNER_HPP
#include <boost/asio.hpp>
#include <iostream>
#include "fail.h"
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <iostream>


template<class Connection>
class TcpListner {
    const Connection::MuxFunction muxFunction;
    using tcp = boost::asio::ip::tcp;
    using error_code = boost::system::error_code;
    boost::asio::io_context &ioc;
    tcp::acceptor acceptor;

    void accept();
    void onAccept(error_code ec, tcp::socket socket);
public:
    explicit TcpListner(boost::asio::io_context &ioc, const Connection::MuxFunction&muxFunction) : acceptor(ioc), ioc(ioc),muxFunction(muxFunction)  {}

    error_code listen(const tcp::endpoint &endpoint);
    ~TcpListner() { std::cout << "Exiting" << std::endl; }
};


template<class Connection>
boost::system::error_code TcpListner<Connection>::listen(const tcp::endpoint &endpoint) {
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
    acceptor.listen(boost::asio::socket_base::max_listen_connections, err);
    if (err) {
        fail(err, "Acceptor.listen failed");
        return err;
    }
    accept();
    return error_code();
}

template<class Connection>
void TcpListner<Connection>::onAccept(error_code err, tcp::socket socket) {
    std::cout << "onAccept" << std::endl;
    if (err) {
        fail(err, "Accept failed");
        return;
    } else {
        std::make_shared<Connection>(std::move(socket))->connect();
    }
    accept();
}

template<class Connection>
void TcpListner<Connection>::accept() {
    acceptor.async_accept(boost::asio::make_strand(ioc), boost::beast::bind_front_handler(
            &TcpListner::onAccept, this));
    std::cout << "Accept" << std::endl;
}


#endif//NTECHLAB_TESTTASK_TCPLISTNER_HPP