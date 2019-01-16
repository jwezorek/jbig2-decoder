// jbig2-decoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <vector>
#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>
#include <iterator>
#include "module.h"

#include "jbig2/JBig2_Context.h"
#include "jbig2/JBig2_Image.h"

std::vector<unsigned char> readFile(const char* filename);
void WritePbm(CJBig2_Image* jbig2_img, const char* fname);

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		return;

	PCFreq = (double)(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}
double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (double)(li.QuadPart - CounterStart) / PCFreq;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return -1;

	JBig2Module module;

	auto data = readFile(argv[1]);
	size_t sz = data.size();

	CJBig2_Context* cntxt = CJBig2_Context::CreateContext(&module, nullptr, 0,
		reinterpret_cast<FX_BYTE*>(&(data[0])), sz, JBIG2_FILE_STREAM);

	double duration = 0;

	std::vector<CJBig2_Image*> pages;
	FX_INT32 result = JBIG2_SUCCESS;
	while (cntxt->GetProcessiveStatus() == FXCODEC_STATUS_FRAME_READY && result == JBIG2_SUCCESS) {
		if (cntxt->GetProcessiveStatus() == FXCODEC_STATUS_ERROR)
			throw std::runtime_error("cntxt->GetProcessiveStatus() == FXCODEC_STATUS_ERROR");
		CJBig2_Image* img = nullptr;
		StartCounter();
		result = cntxt->getNextPage(&img, nullptr);
		if (!duration)
			duration = GetCounter();
		if (img)
			pages.push_back(img);
	}

	std::cout << duration << "\n";
	WritePbm(pages[0], "foo.pbm");
}

void WritePbm(CJBig2_Image* jbig2_img, const char* fname)
{
	std::fstream file;
	file = std::fstream(fname, std::ios::out | std::ios::binary);
	file << "P4\n" << jbig2_img->m_nWidth << " " << jbig2_img->m_nHeight << "\n";
	//file.write((const char*)jbig2_img->m_pData, jbig2_img->m_nStride * jbig2_img->m_nHeight);
	const char* data = (const char*)jbig2_img->m_pData;
	int stride = jbig2_img->m_nStride - 1;
	for(int i = 0; i < jbig2_img->m_nHeight; i++, data += stride+1)
		file.write(data, stride);
}

std::vector<unsigned char> readFile(const char* filename)
{
	// open the file:
	std::ifstream file(filename, std::ios::binary);

	// Stop eating new lines in binary mode!!!
	file.unsetf(std::ios::skipws);

	// get its size:
	std::streampos fileSize;

	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	
	// reserve capacity
	std::vector<unsigned char> vec;
	vec.reserve(fileSize);
	
	// read the data:
	vec.insert(vec.begin(),
		std::istream_iterator<unsigned char>(file),
		std::istream_iterator<unsigned char>()
	);
	
	return vec;
}
