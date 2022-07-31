#ifndef NTECHLAB_TESTTASK_JPEGBASE_H
#define NTECHLAB_TESTTASK_JPEGBASE_H
#include <cstdint>
#include <exception>
#include <stdexcept>


extern "C" {
#include <jpeglib.h>
#include <stdio.h>// Required by jpeglib somewhy
};

class SmallBufferException : public std::exception {
    const std::string msg;

public:
    explicit SmallBufferException(const std::string str) : msg(str) {}
    const char *what() const noexcept override{
        return msg.c_str();
    }
};


class JPEGBase {
protected:
    jpeg_error_mgr errMgr;
    [[ noreturn ]] static void onError(j_common_ptr info) {
        char jpegLastErrorMsg[JMSG_LENGTH_MAX];
        (*(info->err->format_message))(info, jpegLastErrorMsg);
        throw std::runtime_error(jpegLastErrorMsg);
    }
    struct ImageInfo {
        uint32_t width;
        uint32_t height;
        J_COLOR_SPACE colorSpace;
        int pixelSize;
    } imageInfo;


    JPEGBase() = default;

public:
    void setImageInfo(const ImageInfo &imInf) { imageInfo = imInf; };
    const ImageInfo &getImageInfo() { return imageInfo; }
};


#endif//NTECHLAB_TESTTASK_JPEGBASE_H
