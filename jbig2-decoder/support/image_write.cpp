#include "image_write.h"
#include "../jbig2/JBig2_Image.h"
#include <unordered_map>
#include <algorithm>
#include <string> 
#include <vector> 
#include <array> 

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" // http://nothings.org/stb/stb_image_write.h 

std::vector<unsigned char> BitPerPixelToBytePerPixel(const CJBig2_Image* img)
{
	static std::vector<std::array<unsigned char, 8>> lookup_table;
	if (lookup_table.empty()) {
		lookup_table.resize(256);
		unsigned char byte = 0;
		do {
			for (int bit = 0; bit < 8; bit++) {
				lookup_table[byte][bit] = ((byte & (128 >> bit)) != 0) ? 0x00 : 0xff;
			}
		} while (++byte);
	}

	std::vector<unsigned char> output;
	output.reserve(img->m_nWidth * img->m_nHeight);
	auto bytes_per_row = (img->m_nWidth % 8 == 0) ? img->m_nWidth/8 : img->m_nWidth/8 + 1;
	auto data = reinterpret_cast<const char*>( img->m_pData );
	for (int row = 0; row < img->m_nHeight; row++, data += img->m_nStride) {
		for (int byte_index = 0; byte_index < bytes_per_row; byte_index++) {
			unsigned char byte = data[byte_index];
			int num_bytes_from_table = (byte_index < bytes_per_row - 1 || img->m_nWidth % 8 == 0) ? 8 : img->m_nWidth % 8;
			auto src = &(lookup_table[byte][0]);
			std::copy(src, src + num_bytes_from_table, std::back_insert_iterator(output));
		}
	}

	return output;
}

std::string GetExtensionFromFormat(ImageFormat format)
{
	static std::unordered_map<ImageFormat, std::string> format_to_extension {
		{ImageFormat::PNG, "png"},
		{ImageFormat::BMP, "bmp"},
		{ImageFormat::JPEG, "jpg"},
		{ImageFormat::Unknown, ""}
	};
	auto iter = format_to_extension.find(format);
	if (iter != format_to_extension.end())
		return iter->second;
	else
		return "";
}

std::string ToLower(const std::string& str)
{
	std::string temp = str;
	std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
	return temp;
}

ImageFormat GetFormatFromExtension(const std::string& ext) {
	std::string extension = ToLower(ext);
	static std::unordered_map<std::string, ImageFormat> extension_to_format{
		{"png", ImageFormat::PNG},
		{"bmp", ImageFormat::BMP},
		{"jpg", ImageFormat::JPEG},
		{"jpeg", ImageFormat::JPEG}
	};
	auto iter = extension_to_format.find(extension);
	if (iter != extension_to_format.end())
		return iter->second;
	else
		return ImageFormat::Unknown;
}

void WriteImage(const std::string & filename, ImageFormat format, const CJBig2_Image* jbig2_img)
{
	auto img = BitPerPixelToBytePerPixel(jbig2_img);
	switch (format) {
		case ImageFormat::PNG:
			stbi_write_png(filename.c_str(), jbig2_img->m_nWidth, jbig2_img->m_nHeight, 1, &(img[0]), jbig2_img->m_nWidth);
			break;
		case ImageFormat::BMP:
			stbi_write_bmp(filename.c_str(), jbig2_img->m_nWidth, jbig2_img->m_nHeight, 1, &(img[0]));
			break;
		case ImageFormat::JPEG:
			stbi_write_jpg(filename.c_str(), jbig2_img->m_nWidth, jbig2_img->m_nHeight, 1, &(img[0]), 50);
			break;
		case ImageFormat::Unknown:
			throw std::runtime_error("unknown ouput image format.");
	}
}
