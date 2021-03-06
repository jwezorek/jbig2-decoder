// jbig2-decoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <tuple>
#include <memory>
#include <optional>
#include "jbig2/JBig2_Context.h"
#include "jbig2/JBig2_Image.h"
#include "support/module.h"
#include "support/getopt_pp.h" // https://code.google.com/archive/p/getoptpp/
#include "support/image_write.h"
#include "support/file_io_aux.h"

/*-------------------------------------------------------------------------------------------------------------*/

struct OptionsAndArguments
{
	std::string output_file;
	std::string output_format;
	std::string input_file;
	std::string input_global_stream_file;
	bool should_output_processing_time;

	OptionsAndArguments() : should_output_processing_time(false)
	{}
};

struct Context
{
	JBig2Module module;
	CJBig2_Context* jbig2_cntxt;
	std::vector<unsigned char> data;
	std::vector<unsigned char> global_data;

	Context() : jbig2_cntxt(nullptr)
	{}
};

std::optional<OptionsAndArguments> ParseCmdLine(int argc, char *argv[]);
std::unique_ptr<Context> CreateJBig2Context(const std::string& input_file, const std::string& global_stream_file);
std::vector<CJBig2_Image*> DecodeJBig2(Context& cntxt, const OptionsAndArguments& options);
bool GenerateOutputImages(const OptionsAndArguments& options, const std::vector<CJBig2_Image*>& pages);

/*-------------------------------------------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
	auto parse_results = ParseCmdLine(argc, argv);
	if (!parse_results.has_value()) {
		std::cerr << "Error parsing cmd line arguments" << std::endl;
		return -1;
	}
	auto options = parse_results.value();

	auto context = CreateJBig2Context(options.input_file, options.input_global_stream_file);
	if (!context) {
		std::cerr << "Error creating jbig2 context" << std::endl;
		return -1;
	}

	auto pages = DecodeJBig2( *context, options);
	if (pages.empty()) {
		std::cerr << "Error decoding jbig2 data" << std::endl;
		return -1;
	}

	if (!GenerateOutputImages(options, pages)) {
		std::cerr << "Error generating output images" << std::endl;
			return -1;
	}

	return 0;
}

std::unique_ptr<Context> CreateJBig2Context(const std::string& input_file, const std::string& global_stream_file)
{
	std::unique_ptr<Context> cntxt = std::make_unique<Context>();
	cntxt->data = ReadFile(input_file);
	FX_DWORD sz = static_cast<FX_DWORD>(cntxt->data.size());

	try {
		if (global_stream_file.empty()) {
			cntxt->jbig2_cntxt = CJBig2_Context::CreateContext(&(cntxt->module), nullptr, 0,
				reinterpret_cast<FX_BYTE*>(&(cntxt->data[0])), sz, JBIG2_FILE_STREAM);
		}
		else {
			cntxt->global_data = ReadFile(global_stream_file);
			cntxt->jbig2_cntxt = CJBig2_Context::CreateContext(
				&(cntxt->module),
				reinterpret_cast<FX_BYTE*>(&(cntxt->global_data[0])), static_cast<FX_DWORD>(cntxt->global_data.size()),
				reinterpret_cast<FX_BYTE*>(&(cntxt->data[0])), sz,
				JBIG2_EMBED_STREAM
			);
		}
	} catch (...) {
		return nullptr;
	}

	return cntxt;
}

std::optional<OptionsAndArguments> ParseCmdLine(int argc, char *argv[])
{
	OptionsAndArguments output;
	std::vector<std::string> args;
	GetOpt::GetOpt_pp ops(argc, argv);
	ops.exceptions(std::ios::failbit | std::ios::eofbit);

	output.should_output_processing_time = false;
	try {
		ops >> GetOpt::Option('o', "output-file", output.output_file, "");
		ops >> GetOpt::Option('f', "format", output.output_format, "");
		ops >> GetOpt::OptionPresent('t', "time", output.should_output_processing_time);
		ops >> GetOpt::GlobalOption(args);
	}
	catch (GetOpt::GetOptEx ex) {
		return std::nullopt;
	}

	if (args.size() < 1 || args.size() > 2) {
		return std::nullopt;
	}

	if (args.size() == 1) {
		output.input_global_stream_file = "";
		output.input_file = args[0];
	}
	else {
		output.input_global_stream_file = args[0];
		output.input_file = args[1];
	}

	return output;
}

std::vector<CJBig2_Image*> DecodeJBig2(Context& cntxt, const OptionsAndArguments& options)
{
	std::vector<CJBig2_Image*> pages;
	try
	{
		std::chrono::duration<double> elapsed_time;
		FX_INT32 result = JBIG2_SUCCESS;

		std::cout << "begin decoding...\n";

		std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
		auto jbig2_cntxt = cntxt.jbig2_cntxt;
		while (jbig2_cntxt->GetProcessiveStatus() == FXCODEC_STATUS_FRAME_READY && result == JBIG2_SUCCESS) {
			if (jbig2_cntxt->GetProcessiveStatus() == FXCODEC_STATUS_ERROR)
				throw std::runtime_error("cntxt->GetProcessiveStatus() == FXCODEC_STATUS_ERROR");
			CJBig2_Image* img = nullptr;
			result = jbig2_cntxt->getNextPage(&img, nullptr);
			if (img)
				pages.push_back(img);
		}
		std::cout << "decoding complete";

		elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
		if (options.should_output_processing_time)
			std::cout << " in " <<
			std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count() << " ms.\n";
		else
			std::cout << ".\n";
	}
	catch (...) {
		return std::vector<CJBig2_Image*>();
	}

	return pages;
}

bool GenerateOutputImages(const OptionsAndArguments& options, const std::vector<CJBig2_Image*>& pages)
{
	try {
		auto output_file_info = GetOutputInfo(options.output_file, options.output_format, options.input_file);
		auto fname = std::get<0>(output_file_info);
		auto ext = std::get<1>(output_file_info);
		auto format = std::get<2>(output_file_info);

		for (int i = 0; i < pages.size(); i++) {
			auto out_file = GetImageFileName(fname, ext, (pages.size() == 1) ? 0 : i + 1);
			std::cout << "  writing " << out_file << " ...\n";
			WriteImage(out_file, format, pages[i]);
		}
	} catch (...) {
		return false;
	}
	return true;
}