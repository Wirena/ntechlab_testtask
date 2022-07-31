#ifndef NTECHLAB_TESTTASK_JPEGBASE_H
#define NTECHLAB_TESTTASK_JPEGBASE_H
#include <cstdint>
#include <exception>
#include <stdexcept>


extern "C" {
#include <jpeglib.h>
#include <stdio.h>// Required by jpeglib somewhy
};


class JPEGBase {
protected:
    jpeg_error_mgr errMgr;
    struct ImageInfo {
        uint32_t width;
        uint32_t height;
        J_COLOR_SPACE colorSpace;
        uint8_t pixelSize;
    } imageInfo;

    void onError(j_common_ptr info) {
        char jpegLastErrorMsg[JMSG_LENGTH_MAX];
        (*(info->err->format_message))(info, jpegLastErrorMsg);
        throw std::runtime_error(jpegLastErrorMsg);
    }

    JPEGBase() = default;
    const ImageInfo &getImageInfo() { return imageInfo; }
    void setImageInfo(const ImageInfo &imInf) { imageInfo = imInf; };
};


#endif//NTECHLAB_TESTTASK_JPEGBASE_H
