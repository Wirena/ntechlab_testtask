#ifndef NTECHLAB_TESTTASK_JPEGREADER_H
#define NTECHLAB_TESTTASK_JPEGREADER_H
#include "JPEGBase.h"


class CorruptedJPEGException : public std::exception {
public:
    char *what() {
        return "File does not look like JPEG image";
    }
};


class SmallBufferException : public std::exception {
public:
    char *what() {
        return "ReadScanline buffer is too small";
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

    void decompress();
    bool allScanLinesRead();
    bool readScanline(unsigned char *buf, uint64_t length);
};


#endif//NTECHLAB_TESTTASK_JPEGREADER_H
