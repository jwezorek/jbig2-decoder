#ifndef _IMAGE_WRITE_H_
#define _IMAGE_WRITE_H_

#include <string>

class CJBig2_Image;

enum class ImageFormat {
	Unknown,
	PNG,
	BMP,
	JPEG
};

std::string GetExtensionFromFormat(ImageFormat format);
ImageFormat GetFormatFromExtension(const std::string& ext);
void WriteImage(const std::string& filename, ImageFormat format, const CJBig2_Image* img);

#endif