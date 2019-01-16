#include "module.h"

void* JBig2Module::JBig2_Malloc(FX_DWORD dwSize)
{
	return malloc(dwSize);
}

void* JBig2Module::JBig2_Malloc2(FX_DWORD num, FX_DWORD dwSize)
{
	return  malloc(num * dwSize);
}

void* JBig2Module::JBig2_Malloc3(FX_DWORD num, FX_DWORD dwSize, FX_DWORD dwSize2)
{
	return malloc(num * dwSize * dwSize2);
}

void* JBig2Module::JBig2_Realloc(FX_LPVOID pMem, FX_DWORD dwSize)
{
	return realloc(pMem, dwSize);
}

void JBig2Module::JBig2_Free(FX_LPVOID pMem)
{
	free(pMem);
}

void JBig2Module::JBig2_Assert(FX_INT32 nExpression)
{
	assert(nExpression);
}

void JBig2Module::JBig2_Error(FX_LPCSTR format, ...)
{
}

void JBig2Module::JBig2_Warn(FX_LPCSTR format, ...)
{
}

void JBig2Module::JBig2_Log(FX_LPCSTR format, ...)
{
}
