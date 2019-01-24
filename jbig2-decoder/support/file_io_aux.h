#ifndef _FILE_IO_AUX_H_
#define _FILE_IO_AUX_H_

#include <vector>
#include <string>
#include <tuple>
#include "image_write.h"

std::vector<unsigned char> ReadFile(const std::string& filename);
std::tuple<std::string, std::string, ImageFormat> GetOutputInfo(const std::string& out_fname, const std::string& format_str, const std::string& inp_fname);
std::string GetImageFileName(const std::string& base, const std::string& ext, int index);

#endif