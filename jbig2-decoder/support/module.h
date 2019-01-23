#pragma once

#include "../jbig2/JBig2_Module.h"

class JBig2Module : public CJBig2_Module
{
	virtual void *JBig2_Malloc(FX_DWORD dwSize) override;
	virtual void *JBig2_Malloc2(FX_DWORD num, FX_DWORD dwSize) override;
	virtual void *JBig2_Malloc3(FX_DWORD num, FX_DWORD dwSize, FX_DWORD dwSize2) override;
	virtual void *JBig2_Realloc(FX_LPVOID pMem, FX_DWORD dwSize) override;
	virtual void JBig2_Free(FX_LPVOID pMem) override;
	virtual void JBig2_Assert(FX_INT32 nExpression) override;
	virtual	void JBig2_Error(FX_LPCSTR format, ...) override;
	virtual void JBig2_Warn(FX_LPCSTR format, ...) override;
	virtual void JBig2_Log(FX_LPCSTR format, ...) override;
};