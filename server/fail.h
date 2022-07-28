#ifndef NTECHLAB_TESTTASK_FAIL_H
#define NTECHLAB_TESTTASK_FAIL_H
#include <boost/system/error_code.hpp>
#include <iostream>

inline void fail(boost::system::error_code ec, char const *what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

#endif//NTECHLAB_TESTTASK_FAIL_H
