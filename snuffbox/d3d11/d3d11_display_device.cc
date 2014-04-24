#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::Initialise()
	{
		ZeroMemory(&swapChain_, sizeof(SwapChain));
		ZeroMemory(&swapDesc_, sizeof(SwapChainDescription));

		swapDesc_.BufferCount = 1;
		swapDesc_.BufferDesc.Width = environment::game().window()->params().w;
		swapDesc_.BufferDesc.Height = environment::game().window()->params().h;
		swapDesc_.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc_.BufferDesc.RefreshRate.Numerator = 60;
		swapDesc_.BufferDesc.RefreshRate.Denominator = 1;
		swapDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc_.OutputWindow = environment::game().window()->handle();
		swapDesc_.SampleDesc.Count = 1;
		swapDesc_.SampleDesc.Quality = 0;
		swapDesc_.Windowed = TRUE;

		HRESULT result = S_OK;

		D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL featureLevel;

		if (FAILED(result = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_REFERENCE,
			NULL,
			0,
			&featureLevels,
			1,
			D3D11_SDK_VERSION,
			&swapDesc_,
			&swapChain_,
			&device_,
			&featureLevel,
			&context_
			)))
		{
			SNUFF_ASSERT("Could not create D3D11 device!");
		}

		SNUFF_LOG_INFO("Created the D3D11 display device");
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::Destroy()
	{
		SNUFF_ASSERT_NOTNULL(device_);
		device_->Release();
		device_ = NULL;

		SNUFF_ASSERT_NOTNULL(context_);
		context_->Release();
		context_ = NULL;

		SNUFF_ASSERT_NOTNULL(swapChain_);
		swapChain_->Release();
		swapChain_ = NULL;

		SNUFF_LOG_INFO("Destroyed the D3D11 display device");
	}
}