#ifndef NTECHLAB_TESTTASK_JPEGWRITER_H
#define NTECHLAB_TESTTASK_JPEGWRITER_H
#include "JPEGBase.h"

class WrongBufferSize : public std::exception {
public:
    char *what() {
        return "Wrong buffer size for writing scanline";
    }
};



class JPEGWriter : public JPEGBase {
    jpeg_compress_struct compressStruct{0};
    unsigned char *jpegFileBuffer = nullptr;
    uint64_t bufferLength = 0;
    int scanlinesCounter = 0;
public:
    JPEGWriter();
    void setQuality(unsigned int);
    bool allScanlinesWritten();
    void setImageInfo(const ImageInfo &imInf);
    void startCompress();
    bool writeScanline(const unsigned char *buf, uint64_t length);

};


#endif//NTECHLAB_TESTTASK_JPEGWRITER_H
