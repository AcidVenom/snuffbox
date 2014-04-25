#pragma once

#define _WINSOCKAPI_
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <D3DX10.h>

#include <vector>

// Type definitions				|          Type							|        Definition
typedef												IDXGISwapChain									SwapChain;
typedef												DXGI_SWAP_CHAIN_DESC						SwapChainDescription;
typedef												ID3D11Device 										D3DDevice;
typedef												ID3D11DeviceContext							D3DContext;
typedef												IDXGIAdapter										DisplayAdapter;
typedef												ID3D11Texture2D									D3DTexture2D;
typedef												ID3D11RenderTargetView					D3DRenderTargetView;

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11DisplayDevice
	* @brief The actual display device for Direct3D 11, used for rendering on-screen
	* @author Daniël Konings
	*/
	class D3D11DisplayDevice
	{
	public:
		/// Default constructor
		D3D11DisplayDevice(){};

		/// Default destructor
		~D3D11DisplayDevice(){};

		/// Initialises the device
		void Initialise();

		/// Creates the device
		void CreateDevice();

		/// Finds all display adapters of the computer
		void GetAdapters();

		/// Creates the backbuffer of this device
		void CreateBackBuffer();

		/// Destroys the device
		void Destroy();

		/// Converts a HRESULT to a C string
		std::basic_string<TCHAR> HRToString(HRESULT hr);

	private:
		SwapChainDescription					swapDesc_;					///< The swap chain description to create the chain
		SwapChain*										swapChain_;					///< The swap chain for this device
		D3DDevice*										device_;						///< The actual device
		D3DContext*										context_;						///< The immediate context
		std::vector<DisplayAdapter*>	displayAdapters_;		///< The display adapters this device can use
		DisplayAdapter*								primaryAdapter_;		///< The primary display adapter
		D3DTexture2D*									backBuffer_;				///< The backbuffer of this device
		D3DRenderTargetView*					renderTargetView_;	///< The render target view of this device
	};
}