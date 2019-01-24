#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>
#include <iterator>
#include "file_io_aux.h"

size_t GetFileSize(std::ifstream& file_stream)
{
	std::streampos sz;

	file_stream.seekg(0, std::ios::end);
	sz = file_stream.tellg();
	file_stream.seekg(0, std::ios::beg);

	return static_cast<size_t>(sz);
}

std::vector<unsigned char> ReadFile(const std::string& filename)
{
	// open the file:
	std::ifstream file(filename, std::ios::binary);
	file.unsetf(std::ios::skipws);

	auto num_bytes = GetFileSize(file);

	std::vector<unsigned char> vec;
	vec.reserve(num_bytes);

	// read the data:
	vec.insert(vec.begin(),
		std::istream_iterator<unsigned char>(file),
		std::istream_iterator<unsigned char>()
	);

	return vec;
}

std::pair<std::string, std::string> SplitFilenameAndExtension(const std::string& fname)
{
	auto last_dot = fname.find_last_of(".");
	if (last_dot == fname.npos)
		return std::pair<std::string, std::string>(fname, "");
	else
		return std::pair<std::string, std::string>(
			fname.substr(0, last_dot),
			fname.substr(last_dot + 1, fname.size() - last_dot)
			);
}

std::tuple<std::string, std::string, ImageFormat> GetOutputInfo(const std::string& out_fname, const std::string& format_str, const std::string& inp_fname)
{
	if (out_fname.empty()) {
		// if there was no output filename provided, the name of the .jb2 file.
		auto input = SplitFilenameAndExtension(inp_fname);
		ImageFormat format = (!format_str.empty()) ? GetFormatFromExtension(format_str) : ImageFormat::PNG;
		return std::tuple<std::string, std::string, ImageFormat>(input.first, GetExtensionFromFormat(format), format);
	}

	if (!format_str.empty()) {
		// if the filename doesnt have an extension use the format string. Otherwise, assume they know what they're doing.
		auto output = SplitFilenameAndExtension(out_fname);
		std::string ext = (output.second.empty()) ? format_str : "";
		return std::tuple<std::string, std::string, ImageFormat>(out_fname, ext, GetFormatFromExtension(format_str));
	}

	// if there is an output filename and no format option, use the extension of the output filename to determine
	// the desired format.
	auto output = SplitFilenameAndExtension(out_fname);
	if (output.second.empty())
		output.second = GetExtensionFromFormat(ImageFormat::PNG);

	return std::tuple<std::string, std::string, ImageFormat>(output.first, output.second, GetFormatFromExtension(output.second));
}

std::string GetImageFileName(const std::string& base, const std::string& ext, int index)
{
	if (index == 0)
		return base + "." + ext;
	return base + "-" + std::to_string(index) + "." + ext;
}