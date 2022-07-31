#ifndef NTECHLAB_TESTTASK_JPEGWRITER_H
#define NTECHLAB_TESTTASK_JPEGWRITER_H
#include "JPEGBase.h"
#include <iostream>
#include <memory>
#include <tuple>


class JPEGWriter : public JPEGBase {
    jpeg_compress_struct compressStruct{0};
    unsigned char *jpegFileBuffer = nullptr;
    uint64_t bufferLength = 0;
    int scanlinesCounter = 0;

public:
    JPEGWriter();
    JPEGWriter(const JPEGWriter &) = delete;
    JPEGWriter(JPEGWriter &&) = delete;
    ~JPEGWriter();
    auto releaseFileBuffer() {
        auto deleter = [](unsigned char *ptr) { free(ptr); };
        std::unique_ptr<unsigned char, decltype(deleter)> ptr(jpegFileBuffer, deleter);
        jpegFileBuffer = nullptr;
        return std::make_tuple<decltype(ptr), uint64_t>(std::move(ptr), std::move(bufferLength));
    }
    void finishCompressing();
    void setQuality(unsigned int);
    bool allScanlinesWritten();
    void setImageInfo(const ImageInfo &imInf);
    void prepareForWrite();
    bool writeScanline(const unsigned char *buf, uint64_t length);
};


#endif//NTECHLAB_TESTTASK_JPEGWRITER_H
