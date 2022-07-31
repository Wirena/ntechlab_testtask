#include "JPEGWriter.h"

JPEGWriter::JPEGWriter() {
    compressStruct.err = jpeg_std_error(&errMgr);
    jpeg_create_compress(&compressStruct);
    compressStruct.err = &errMgr;
    jpeg_mem_dest(&compressStruct, &jpegFileBuffer, &bufferLength);
}
void JPEGWriter::setQuality(unsigned int quality) {
    if (quality > 100) quality = 100;
    jpeg_set_quality(&compressStruct, static_cast<int>(quality), TRUE);
}
bool JPEGWriter::allScanlinesWritten() {
    return scanlinesCounter >= imageInfo.height;
}
void JPEGWriter::setImageInfo(const JPEGBase::ImageInfo &imInf) {
    JPEGBase::setImageInfo(imInf);
    compressStruct.image_width = imageInfo.width;
    compressStruct.image_height = imageInfo.height;
    compressStruct.input_components = imageInfo.pixelSize;
    compressStruct.in_color_space = imageInfo.colorSpace;
    jpeg_set_defaults(&compressStruct);
}
void JPEGWriter::startCompress() {
    jpeg_start_compress(&compressStruct, TRUE);
}
bool JPEGWriter::writeScanline(const unsigned char *buf, uint64_t length) {
    if (scanlinesCounter >= imageInfo.height) return false;
    if (length != imageInfo.width * imageInfo.pixelSize) throw WrongBufferSize();
    jpeg_write_scanlines(&compressStruct, const_cast<unsigned char**>(&buf), 1);
    scanlinesCounter++;
    return true;
}
