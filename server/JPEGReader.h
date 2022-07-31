#ifndef NTECHLAB_TESTTASK_JPEGREADER_H
#define NTECHLAB_TESTTASK_JPEGREADER_H
#include "JPEGBase.h"


class CorruptedJPEGException : public std::exception {
    const std::string msg;
public:
    explicit CorruptedJPEGException(const std::string str) : msg(str) {}
    const char *what() const noexcept override {
        return msg.c_str();
    }
};


class JPEGReader : public JPEGBase {
    jpeg_decompress_struct decompressStruct{0};
    const char *jpegFileBuffer = nullptr;
    const uint64_t bufferLength = 0;

public:
    JPEGReader(const JPEGReader &) = delete;
    JPEGReader(JPEGReader &&) = delete;
    JPEGReader(const char *jpegFileBuffer, uint64_t bufferLength);
    ~JPEGReader();
    void finishDecompressing();
    void decompress();
    bool allScanLinesRead();
    bool readScanline(unsigned char *buf, uint64_t length);
};


#endif//NTECHLAB_TESTTASK_JPEGREADER_H
