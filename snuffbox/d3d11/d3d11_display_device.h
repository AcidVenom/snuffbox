#pragma once

#define _WINSOCKAPI_
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <D3DX10.h>


// Type definitions				|          Type							|        Definition
typedef												IDXGISwapChain									SwapChain;
typedef												DXGI_SWAP_CHAIN_DESC						SwapChainDescription;
typedef												ID3D11Device 										D3DDevice;
typedef												ID3D11DeviceContext							D3DContext;

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

		/// Destroys the device
		void Destroy();

	private:
		SwapChainDescription	swapDesc_; ///< The swap chain description to create the chain
		SwapChain*						swapChain_; ///< The swap chain for this device
		D3DDevice*						device_; ///< The actual device
		D3DContext*						context_; ///< The immediate context
	};
}