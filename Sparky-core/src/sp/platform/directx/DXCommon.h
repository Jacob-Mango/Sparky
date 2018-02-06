#pragma once

#ifndef DX_COMMON_H
#define DX_COMMON_H

#include <d3d11.h>
#include <d3dcompiler.h>

#include "../../utils/Log.h"

#include <comdef.h>

#include <stdarg.h>
#include <memory>


std::string string_format(const std::string fmt_str, ...);

static bool CheckD3DError(HRESULT result)
{
	if (result == S_OK)
		return true;

	_com_error error(result);
	
	SP_ERROR("Error ", result, ": ", error.ErrorMessage());

	switch (result)
	{
	case D3D11_ERROR_FILE_NOT_FOUND:
		break;
	case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
		break;
	case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
		break;
	case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
		break;
	case DXGI_ERROR_INVALID_CALL:
		break;
	case DXGI_ERROR_WAS_STILL_DRAWING:
		break;
	case E_FAIL:
		break;
	case E_INVALIDARG:
		// return true;
		break;
	case E_OUTOFMEMORY:
		break;
	case E_NOTIMPL:
		break;
	case S_FALSE:
		break;
	}

	return false;
}

#ifdef SP_DEBUG
	#define DXCall(x) do { \
		HRESULT __hr = x; \
		SP_ASSERT(CheckD3DError(__hr)); \
		} while(false)
#else
	#define DXCall(x) x
#endif

#endif