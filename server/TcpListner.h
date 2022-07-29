#ifndef NTECHLAB_TESTTASK_TCPLISTNER_H
#define NTECHLAB_TESTTASK_TCPLISTNER_H
#include <boost/asio.hpp>
#include <iostream>

class TcpListner {
    using tcp = boost::asio::ip::tcp;
    using error_code = boost::system::error_code;

    boost::asio::io_context &ioc;
    tcp::acceptor acceptor;

    void accept();
    void onAccept(error_code ec, tcp::socket socket);

public:
    explicit TcpListner(boost::asio::io_context &ioc) : acceptor(ioc), ioc(ioc) {}

    error_code listen(const tcp::endpoint &endpoint);
    ~TcpListner() { std::cout << "Exiting" << std::endl; }
};


#endif//NTECHLAB_TESTTASK_TCPLISTNER_H