// Based on Dynamic DPI sample from https://github.com/microsoft/Windows-classic-samples/tree/master/Samples/DynamicDPI
#include "DxGraphics.h"

#include <algorithm>

#define NOMINMAX
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

namespace graphics::dx
{

namespace wrl = Microsoft::WRL;

struct CoInit
{
	CoInit();
	~CoInit();
};

class Device : public IDevice
{
private:
	friend class DeviceContext;

	CoInit _coInit;

	// Direct3D
	wrl::ComPtr<ID3D11Device2>        _d3dDevice;
	wrl::ComPtr<ID3D11DeviceContext2> _d3dContext;
	D3D_FEATURE_LEVEL                 _d3dFeatureLevel;

	// Direct2D
	wrl::ComPtr<ID2D1Factory2>       _d2dFactory;
	wrl::ComPtr<ID2D1Device1>        _d2dDevice;

	// DirectWrite + Windows Imaging Component
	wrl::ComPtr<IDWriteFactory2>     _dwriteFactory;
	wrl::ComPtr<IWICImagingFactory2> _wicFactory;

	void CreateIndependent();
	void CreateDevice();
public:
	Device();
	virtual ~Device();

	std::unique_ptr<IDeviceContext> CreateDeviceContext() override;
};

class DeviceContext : public IDeviceContext
{
private:
	Device& _device;
	// DXGI
	wrl::ComPtr<IDXGISwapChain1>		_swapChain;

	// Direct3D
	wrl::ComPtr<ID3D11RenderTargetView> _d3dRenderTargetView;
	wrl::ComPtr<ID3D11DepthStencilView> _d3dDepthStencilView;
	D3D11_VIEWPORT                      _viewport;
	Size                                _d3dRenderTargetSize;

	// Direct2D
	wrl::ComPtr<ID2D1DeviceContext1>	_d2dContext;
	wrl::ComPtr<ID2D1Bitmap1>			_d2dTargetBitmap;

	HWND _hwnd;
	HDC _hdc;
	PAINTSTRUCT _ps;

	bool Present();
public:
	DeviceContext( Device& device );
	virtual ~DeviceContext();

	void Resize( HWND hwnd );
	
	void BeginDraw( directui::Handle windowHandle ) override;
	void EndDraw() override;

	void Clear( Color color ) override;
	void FillSolidRect( Color color, Rect rect ) override;
};

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

CoInit::CoInit()
{
	ThrowIfFailed( ::CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED ) );
}

CoInit::~CoInit()
{
	::CoUninitialize();
}

Device::Device()
	: _d3dFeatureLevel{ D3D_FEATURE_LEVEL_9_1 }
{
	CreateIndependent();
	CreateDevice();
}

Device::~Device()
{

}

std::unique_ptr<IDeviceContext> Device::CreateDeviceContext()
{
	return std::unique_ptr<IDeviceContext>( new DeviceContext( *this ) );
}

void Device::CreateIndependent()
{
	// Initialize Direct2D resources.
	D2D1_FACTORY_OPTIONS options;
	::ZeroMemory( &options, sizeof( D2D1_FACTORY_OPTIONS ) );

#if defined(_DEBUG)
	// If the project is in a debug build, enable Direct2D debugging via SDK Layers.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	// Initialize the Direct2D Factory.
	ThrowIfFailed(
		::D2D1CreateFactory(
			D2D1_FACTORY_TYPE_MULTI_THREADED,
			__uuidof( ID2D1Factory2 ),
			&options,
			&_d2dFactory
		)
	);

	// Initialize the DirectWrite Factory.
	ThrowIfFailed(
		::DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_ISOLATED,
			__uuidof( IDWriteFactory2 ),
			&_dwriteFactory
		)
	);

	// Initialize the Windows Imaging Component (WIC) Factory.
	ThrowIfFailed(
		::CoCreateInstance(
			CLSID_WICImagingFactory2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS( &_wicFactory )
		)
	);
}

#if defined(_DEBUG)
// Check for SDK Layer support.
inline bool SdkLayersAvailable()
{
	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
		0,
		D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
		nullptr,                    // Any feature level will do.
		0,
		D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
		nullptr,                    // No need to keep the D3D device reference.
		nullptr,                    // No need to know the feature level.
		nullptr                     // No need to keep the D3D device context reference.
	);

	return SUCCEEDED( hr );
}
#endif

void Device::CreateDevice()
{
	// This flag adds support for surfaces with a different color channel ordering
	// than the API default. It is required for compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	if ( SdkLayersAvailable() )
	{
		// If the project is in a debug build, enable debugging via SDK Layers with this flag.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	// Don't forget to declare your application's minimum required feature level in its
	// description.  All applications are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Create the Direct3D 11 API device object and a corresponding context.
	wrl::ComPtr<ID3D11Device> device;
	wrl::ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = ::D3D11CreateDevice(
		nullptr,                    // Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
		0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		creationFlags,              // Set debug and Direct2D compatibility flags.
		featureLevels,              // List of feature levels this app can support.
		ARRAYSIZE( featureLevels ), // Size of the list above.
		D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
		&device,                    // Returns the Direct3D device created.
		&_d3dFeatureLevel,          // Returns feature level of device created.
		&context                    // Returns the device immediate context.
	);

	if ( FAILED( hr ) )
	{
		// If the initialization fails, fall back to the WARP device.
		// For more information on WARP, see: 
		// http://go.microsoft.com/fwlink/?LinkId=286690
		ThrowIfFailed(
			::D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
				0,
				creationFlags,
				featureLevels,
				ARRAYSIZE( featureLevels ),
				D3D11_SDK_VERSION,
				&device,
				&_d3dFeatureLevel,
				&context
			)
		);
	}

	// Store pointers to the Direct3D 11.1 API device and immediate context.
	ThrowIfFailed(
		device.As( &_d3dDevice )
	);

	ThrowIfFailed(
		context.As( &_d3dContext )
	);

	// Create the Direct2D device object and a corresponding context.
	wrl::ComPtr<IDXGIDevice3> dxgiDevice;
	ThrowIfFailed(
		_d3dDevice.As( &dxgiDevice )
	);

	ThrowIfFailed(
		_d2dFactory->CreateDevice( dxgiDevice.Get(), &_d2dDevice )
	);
}

DeviceContext::DeviceContext( Device& device )
	: _device{ device }
	, _viewport{}
	, _d3dRenderTargetSize{}
	, _hwnd{ nullptr }
{
	ThrowIfFailed(
		_device._d2dDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&_d2dContext
		)
	);
}

DeviceContext::~DeviceContext()
{

}

void DeviceContext::Resize( HWND hwnd )
{
	RECT rc;
	::GetClientRect( hwnd, &rc );

	auto width = static_cast< float >( std::max( 1L, rc.right ) );
	auto height = static_cast< float >( std::max( 1L, rc.bottom ) );

	if ( _hwnd == hwnd && width == _d3dRenderTargetSize.w && height == _d3dRenderTargetSize.h )
		return;

	_hwnd = hwnd;

	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	_device._d3dContext->OMSetRenderTargets( ARRAYSIZE( nullViews ), nullViews, nullptr );
	_d3dRenderTargetView = nullptr;
	_d2dContext->SetTarget( nullptr );
	_d2dTargetBitmap = nullptr;
	_d3dDepthStencilView = nullptr;
	_device._d3dContext->Flush();
	
	_d3dRenderTargetSize.w = width;
	_d3dRenderTargetSize.h = height;

	if ( _swapChain != nullptr )
	{
		// If the swap chain already exists, resize it.
		HRESULT hr = _swapChain->ResizeBuffers(
			2, // Double-buffered swap chain.
			static_cast< UINT >( _d3dRenderTargetSize.w ),
			static_cast< UINT >( _d3dRenderTargetSize.h ),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
		);

		if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			//HandleDeviceLost();

			// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method
			// and correctly set up the new device.
			return;
		}
		else
		{
			ThrowIfFailed( hr );
		}
	}
	else
	{
		// Otherwise, create a new one using the same adapter as the existing Direct3D device.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = static_cast< UINT >( _d3dRenderTargetSize.w ); // Match the size of the window.
		swapChainDesc.Height = static_cast< UINT >( _d3dRenderTargetSize.h );
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
		swapChainDesc.Flags = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		wrl::ComPtr<IDXGIDevice3> dxgiDevice;
		ThrowIfFailed(
			_device._d3dDevice.As( &dxgiDevice )
		);

		wrl::ComPtr<IDXGIAdapter> dxgiAdapter;
		ThrowIfFailed(
			dxgiDevice->GetAdapter( &dxgiAdapter )
		);

		wrl::ComPtr<IDXGIFactory2> dxgiFactory;
		ThrowIfFailed(
			dxgiAdapter->GetParent( IID_PPV_ARGS( &dxgiFactory ) )
		);

		ThrowIfFailed(
			dxgiFactory->CreateSwapChainForHwnd(
				_device._d3dDevice.Get(),
				_hwnd,
				&swapChainDesc,
				nullptr,
				nullptr,
				&_swapChain
			)
		);

		// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
		// ensures that the application will only render after each VSync, minimizing power consumption.
		ThrowIfFailed(
			dxgiDevice->SetMaximumFrameLatency( 1 )
		);
	}


	// Create a render target view of the swap chain back buffer.
	wrl::ComPtr<ID3D11Texture2D> backBuffer;
	ThrowIfFailed(
		_swapChain->GetBuffer( 0, IID_PPV_ARGS( &backBuffer ) )
	);

	ThrowIfFailed(
		_device._d3dDevice->CreateRenderTargetView(
			backBuffer.Get(),
			nullptr,
			&_d3dRenderTargetView
		)
	);

	// Create a depth stencil view for use with 3D rendering if needed.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast< UINT >( _d3dRenderTargetSize.w ),
		static_cast< UINT >( _d3dRenderTargetSize.h ),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	wrl::ComPtr<ID3D11Texture2D> depthStencil;
	ThrowIfFailed(
		_device._d3dDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			&depthStencil
		)
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc( D3D11_DSV_DIMENSION_TEXTURE2D );
	ThrowIfFailed(
		_device._d3dDevice->CreateDepthStencilView(
			depthStencil.Get(),
			&depthStencilViewDesc,
			&_d3dDepthStencilView
		)
	);

	// Create a Direct2D target bitmap associated with the
	// swap chain back buffer and set it as the current target.
	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat( DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED ),
			96.0f,
			96.0f
		);

	wrl::ComPtr<IDXGISurface2> dxgiBackBuffer;
	ThrowIfFailed(
		_swapChain->GetBuffer( 0, IID_PPV_ARGS( &dxgiBackBuffer ) )
	);

	ThrowIfFailed(
		_d2dContext->CreateBitmapFromDxgiSurface(
			dxgiBackBuffer.Get(),
			&bitmapProperties,
			&_d2dTargetBitmap
		)
	);

	_d2dContext->SetTarget( _d2dTargetBitmap.Get() );
}

bool DeviceContext::Present()
{
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = _swapChain->Present( 1, 0 );

	// Discard the contents of the render target.
	// This is a valid operation only when the existing contents will be entirely
	// overwritten. If dirty or scroll rects are used, this call should be removed.
	_device._d3dContext->DiscardView( _d3dRenderTargetView.Get() );

	// Discard the contents of the depth stencil.
	_device._d3dContext->DiscardView( _d3dDepthStencilView.Get() );

	// If the device was removed either by a disconnection or a driver upgrade, we
	// must recreate all device resources.
	if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
	{
		//HandleDeviceLost();
	}
	else if ( hr == DXGI_STATUS_OCCLUDED )
	{
		return false;
	}
	else
	{
		ThrowIfFailed( hr );
	}
	return true;
}

void DeviceContext::BeginDraw(directui::Handle windowHandle)
{
	Resize( static_cast< HWND >( windowHandle ) );

	_hdc = ::BeginPaint( _hwnd, &_ps );

	// Set the 3D rendering viewport to target the entire window.
	_viewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		_d3dRenderTargetSize.w,
		_d3dRenderTargetSize.h
	);

	_device._d3dContext->RSSetViewports( 1, &_viewport );

	_d2dContext->BeginDraw();
}

void DeviceContext::EndDraw()
{
	auto hr = _d2dContext->EndDraw();
	Present();

	::EndPaint( _hwnd, &_ps );
}

void DeviceContext::Clear( Color color )
{
	_d2dContext->Clear( D2D1::ColorF( color.r, color.g, color.b, color.a ) );
}

void DeviceContext::FillSolidRect( Color color, Rect rect )
{
	wrl::ComPtr<ID2D1SolidColorBrush> brush;
	_d2dContext->CreateSolidColorBrush(
		D2D1::ColorF( color.r, color.g, color.b, color.a ),
		&brush );
	_d2dContext->FillRectangle(
		D2D1::RectF( rect.x, rect.y, rect.x + rect.w, rect.y + rect.h ),
		brush.Get() );
}

std::unique_ptr<IDevice> CreateDevice()
{
	return std::unique_ptr<IDevice>( new Device() );
}

} // namespace graphics::dx
