#pragma once

#define _WINSOCKAPI_
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <D3DX10.h>
#include <D3DX11.h>
#include <xnamath.h>

#include <vector>

// Type definitions				|          Type							|        Definition
typedef												IDXGISwapChain									SwapChain;
typedef												DXGI_SWAP_CHAIN_DESC						SwapChainDescription;
typedef												ID3D11Device 										D3DDevice;
typedef												ID3D11DeviceContext							D3DContext;
typedef												IDXGIAdapter										DisplayAdapter;
typedef												ID3D11Texture2D									D3DTexture2D;
typedef												ID3D11RenderTargetView					D3DRenderTargetView;
typedef												ID3D11VertexShader							VertexShader;
typedef												ID3D11PixelShader								PixelShader;

namespace snuffbox
{

	class Camera;
	class RenderElement;
	class Texture;

  /**
  * @enum snuffbox::VertexBufferType
  * @brief The vertex buffer types for checking if a vertex buffer should be swapped
  * @author Daniël Konings
  */
  enum VertexBufferType
  {
    kNone,
    kQuad,
    kTerrain
  };

	/**
	* @struct snuffbox::Vertex
	* @brief A vertex structure for use with vertex buffers
	* @author Daniël Konings
	*/
	struct Vertex
	{
		float x, y, z;
		D3DXCOLOR colour;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		const static UINT stride_size = static_cast<UINT>(sizeof(float)* 3 + sizeof(D3DXCOLOR)+sizeof(XMFLOAT3)+sizeof(XMFLOAT2));
	};

	struct VS_CONSTANT_BUFFER
	{
		float Time;
		XMMATRIX Projection;
		XMMATRIX View;
		XMMATRIX World;
		XMMATRIX WorldViewProjection;
		XMVECTOR CamPos;
	};

	/**
	* @class snuffbox::D3D11DisplayDevice
	* @brief The actual display device for Direct3D 11, used for rendering on-screen
	* @author Daniël Konings
	*/
	class D3D11DisplayDevice
	{
	public:
		/// Default constructor
		D3D11DisplayDevice();

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
		
		/// Creates a vertex buffer for use with this device
		void CreateLayout();

		/// Loads a specific shader
		void LoadShader(const char* path, bool reloading = false);

		/// Creates the constant buffer
		void CreateConstantBuffer();

		/// Creates the viewport
		void CreateViewport();

		/// Starts the draw
		void StartDraw();

		/// Draws every render element
		void Draw();

		/// Updates the constant buffer
		void UpdateCamera(Camera* camera);

		/// Ends the draw
		void EndDraw();

		/// Increases the elapsed time
		void IncrementTime(){ ++time_; }

		/// Creates the depth stencil view/buffer
		void CreateDepthStencil();

		/// Creates the sampler state
		void CreateSamplerState();

		/// Destroys the device
		void Destroy();

		/// Creates a vertex buffer and returns it
		ID3D11Buffer* CreateVertexBuffer(const std::vector<Vertex>& vertices);

		/// Creates an index buffer and returns it
		ID3D11Buffer* CreateIndexBuffer(const std::vector<unsigned int>& indices);

		/// Converts a HRESULT to a C string
		std::basic_string<TCHAR> HRToString(HRESULT hr, const char* subGroup);

		/// Returns the default vertex shader
		VertexShader* vs(){ return vs_; }

		/// Returns the default pixel shader
		PixelShader* ps(){ return ps_; }

		/// Returns the input layout
		ID3D11InputLayout* layout(){ return inputLayout_; }

		/// Sets the vertex buffer
		void SetVertexBuffer(ID3D11Buffer* vertexBuffer){ UINT offset = 0; context_->IASetVertexBuffers(0, 1, &vertexBuffer, &Vertex::stride_size, &offset); }

		/// Sets the index buffer
		void SetIndexBuffer(ID3D11Buffer* indexBuffer){ context_->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0); }

		/// Returns the list of render elements
		std::vector<RenderElement*>& renderElements(){ return renderElements_; }

    /// Returns the current vertex buffer type
    VertexBufferType& vbType(){ return vbType_; }

		/// Returns the device
		ID3D11Device* device(){ return device_; }

		/// Creates the default texture to use in shaders
		void CreateDefaultTexture();

	private:
		SwapChainDescription					swapDesc_;					///< The swap chain description to create the chain
		SwapChain*										swapChain_;					///< The swap chain for this device
		D3DDevice*										device_;						///< The actual device
		D3DContext*										context_;						///< The immediate context
		std::vector<DisplayAdapter*>	displayAdapters_;		///< The display adapters this device can use
		DisplayAdapter*								primaryAdapter_;		///< The primary display adapter
		D3DTexture2D*									backBuffer_;				///< The backbuffer of this device
		D3DRenderTargetView*					renderTargetView_;	///< The render target view of this device
		ID3D11Buffer*									vsConstantBuffer_;	///< The vertex buffer
		ID3D11InputLayout*						inputLayout_;				///< The vertex input layout
		ID3D10Blob*										vsBuffer_;					///< Vertex shader buffer
		ID3D10Blob*										psBuffer_;					///< Pixel shader buffer
		VertexShader*									vs_;								///< The vertex shader
		PixelShader*									ps_;								///< The pixel shader
		float													time_;							///< The game time
		XMMATRIX											worldMatrix_;				///< The world matrix
		XMMATRIX											viewMatrix_;				///< The view matrix
		XMMATRIX											projectionMatrix_;	///< The projection matrix
		ID3D11RasterizerState*				rasterizerState_;	  ///< The rasterizer state
		std::vector<RenderElement*>		renderElements_;		///< The list of render elements
    VertexBufferType              vbType_;            ///< The vertex buffer type
		XMVECTOR											camPos_;						///< The current camera position
		ID3D11DepthStencilView*				depthStencilView_;	///< The depth stencil view
		ID3D11Texture2D*							depthStencilBuffer_;///< The buffer of the depth stencil
		ID3D11Texture2D*							noTexture_;					///< A white rectangular texture as default
		ID3D11ShaderResourceView*			defaultResource_;		///< The default shader resource
		ID3D11SamplerState*						samplerState_;			///< The texture sampler state
		Texture*											currentTexture_;		///< The current texture being used
		Camera*												camera_;						///< The current camera being used
	};
}