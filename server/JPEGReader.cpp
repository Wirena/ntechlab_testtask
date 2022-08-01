#include "JPEGReader.h"
#include <iostream>


JPEGReader::JPEGReader(const char *jpegFileBuffer, uint64_t bufferLength) : jpegFileBuffer(jpegFileBuffer), bufferLength(bufferLength) {
    decompressStruct.err = jpeg_std_error(&errMgr);
    errMgr.error_exit = JPEGBase::onError;
    jpeg_create_decompress(&decompressStruct);
}

void JPEGReader::decompress() {
    jpeg_mem_src(&decompressStruct, reinterpret_cast<const unsigned char *>(jpegFileBuffer), bufferLength);
    auto err = jpeg_read_header(&decompressStruct, TRUE);
    if (err != JPEG_HEADER_OK) throw CorruptedJPEGException(err);
    jpeg_start_decompress(&decompressStruct);
    imageInfo.width = decompressStruct.output_width;
    imageInfo.height = decompressStruct.output_height;
    imageInfo.colorSpace = decompressStruct.out_color_space;
    imageInfo.pixelSize = decompressStruct.num_components;
}
bool JPEGReader::readScanline(unsigned char *buf, uint64_t length) {
    if (decompressStruct.output_scanline >= imageInfo.height) return false;
    if (length < imageInfo.width * imageInfo.pixelSize)
        throw SmallBufferException("JPEGReader::readScanline: provided scanline buffer is smaller than an actual scanline");
    jpeg_read_scanlines(&decompressStruct, &buf, 1);
    return true;
}
bool JPEGReader::allScanLinesRead() {
    return decompressStruct.output_scanline >= imageInfo.height;
}
JPEGReader::~JPEGReader() {
    jpeg_destroy_decompress(&decompressStruct);
}
void JPEGReader::finishDecompressing() {
    jpeg_finish_decompress(&decompressStruct);
}
