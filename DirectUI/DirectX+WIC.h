// Based on Dynamic DPI sample from https://github.com/microsoft/Windows-classic-samples/tree/master/Samples/DynamicDPI

#pragma once

#include <windows.h> // for Win32 API

#include <wrl.h> // for ComPtr
#include <wrl/client.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <DirectXMath.h>

#pragma comment(lib, "shcore.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dwrite.lib")

namespace DX
{

inline void ThrowIfFailed( HRESULT hr )
{
	if ( FAILED( hr ) )
	{
		struct ComException
		{
			HRESULT const hr;
			ComException( HRESULT const value ) : hr( value ) {}
		};

		if ( S_OK != hr )
		{
			throw ComException( hr );
		}
	}
}


// Setup Auto initializer for COM for the WIC Factory used in the DeviceResources.
struct AutoCoInitialize
{
	AutoCoInitialize()
	{
		DX::ThrowIfFailed( ::CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED ) );
	}

	~AutoCoInitialize()
	{
		::CoUninitialize();
	}
};

class Device
{
private:
	AutoCoInitialize _comHelper;

	// Direct3D
	Microsoft::WRL::ComPtr<ID3D11Device2>        _d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext2> _d3dContext;

	// Direct2D
	Microsoft::WRL::ComPtr<ID2D1Factory2>       _d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device1>        _d2dDevice;

	// DirectWrite + Windows Imaging Codec
	Microsoft::WRL::ComPtr<IDWriteFactory2>     _dwriteFactory;
	Microsoft::WRL::ComPtr<IWICImagingFactory2> _wicFactory;
};

}