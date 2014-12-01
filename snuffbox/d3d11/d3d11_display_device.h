#pragma once

#define SNUFF_SAFE_RELEASE(ptr) SNUFF_ASSERT_NOTNULL(ptr); ptr->Release(); ptr = NULL;

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <D3DX10.h>
#include <D3DX11.h>
#include <xnamath.h>
#include <array>
#include <queue>

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
	class Shader;
  class FBXModel;
	struct Shaders;

  /**
  * @enum snuffbox::VertexBufferType
  * @brief The vertex buffer types for checking if a vertex buffer should be swapped
  * @author Daniël Konings
  */
  enum VertexBufferType
  {
    kNone,
    kQuad,
    kQuad2D,
    kTerrain,
		kMesh,
		kLine,
    kText,
		kPolygon
  };

	/**
	* @struct snuffbox::Vertex
	* @brief A vertex structure for use with vertex buffers
	* @author Daniël Konings
	*/
	struct Vertex
	{
		float x; //!< The x position of the vertex
		float y; //!< The y position of the vertex
		float z; //!< The z position of the vertex
		float w; //!< The w position of the vertex
		XMFLOAT3 normal; //!< The vertex normal
		XMFLOAT2 tex_coord; //!< The vertex texture coordinates
		XMFLOAT4 colour; //!< The vertex colour
		const static UINT stride_size = static_cast<UINT>(sizeof(float) * 4 + sizeof(XMFLOAT3) + sizeof(XMFLOAT2) + sizeof(XMFLOAT4)); //!< The stride size
	};

	/**
	* @struct snuffbox::ShaderConstantBuffer
	* @brief The constant buffer used to pass variables to the shader
	* @author Daniël Konings
	*/
	struct ShaderConstantBuffer
	{
		float Time; //!< The elapsed time
		XMMATRIX Projection; //!< The projection matrix
		XMMATRIX View; //!< The view matrix
		XMMATRIX World; //!< The world matrix
		XMMATRIX WorldViewProjection; //!< The World/View/Projection matrix
		float Alpha; //!< The alpha of a render element
		XMFLOAT3 Blend; //!< The blend of a render element
    XMMATRIX InvWorld; //!< The inverse world, used for lighting etc.
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

		/**
		* @brief Loads a shader text file and compiles it
		* @param[in] path (const char*) The path to the shader file
		* @return (snuffbox::Shaders) The vertex and the pixel shader
		*/
		Shaders LoadShader(const char* path);

		/// Creates the constant buffer
		void CreateConstantBuffer();

		/// Creates the viewport
		void CreateViewport();

		/// Starts the draw
		void StartDraw();

		/// Draws every render element
		void Draw();

		/** 
		* @brief Sets the camera
		* @param[in] camera (snuffbox::Camera*) The camera to set
		*/
		void SetCamera(Camera* camera);

		/**
		* @brief Updates the camera before the draw loop, so that it is in sync with the rest
		* @param[in] camera (snuffbox::Camera*) The camera to update
		*/
		void UpdateCamera(Camera* camera);

		/**
		* @return (snuffbox::Camera*) The current camera in use
		*/
		Camera* camera(){ return camera_; }

		/// Ends the draw
		void EndDraw();

		/**
		* @brief Increases the elapsed time by a given value
		* @param[in] dt (float) The delta time, or any value to increment with
		*/
		void IncrementTime(float dt){ time_ += dt; }

		/// Creates the depth stencil view/buffer
		void CreateDepthStencil();

		/// Creates the sampler state
		void CreateSamplerState();

		/// Creates the base shader
		void CreateBaseShader();

		/// Destroys the device
		void Destroy();

		/**
		* @brief Creates a vertex buffer from a vector of vertices
		* @param[in] vertices (const std::vector<snuffbox::Vertex>&) The list of vertices
		* @return (ID3D11Buffer*) The corresponding vertex buffer
		*/
		ID3D11Buffer* CreateVertexBuffer(const std::vector<Vertex>& vertices);

		/**
		* @brief Creates an index buffer from a vector of indices
		* @param[in] vertices (const std::vector<unsigned int>&) The list of indices
		* @return (ID3D11Buffer*) The corresponding index buffer
		*/
		ID3D11Buffer* CreateIndexBuffer(const std::vector<unsigned int>& indices);

		/** 
		* @brief Converts a HRESULT to a C string
		* @param[in] hr (HRESULT) The HRESULT to convert
		* @param[in] sub (const char*) The subgroup to add to the string (used for error logging)
		* @return (std::basic_string<TCHAR>) The actual string
		*/
    std::string HRToString(HRESULT hr, const char* subGroup);

		/**
		* @return (ID3D11InputLayout*) The input layout
		*/
		ID3D11InputLayout* layout(){ return input_layout_; }

		/**
		* @brief Sets the vertex buffer to currently use
		* @param[in] vertexBuffer (ID3D11Buffer*) The vertex buffer to set
		*/
		void SetVertexBuffer(ID3D11Buffer* vertexBuffer){ UINT offset = 0; context_->IASetVertexBuffers(0, 1, &vertexBuffer, &Vertex::stride_size, &offset); }

		/**
		* @brief Sets the index buffer to currently use
		* @param[in] indexBuffer (ID3D11Buffer*) The index buffer to set
		*/
		void SetIndexBuffer(ID3D11Buffer* indexBuffer){ context_->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0); }

		/**
		* @return (std::vector<RenderElement*>&) The list of normal render elements
		*/
		std::vector<RenderElement*>& render_elements(){ 
      return render_elements_; 
    }

		/**
		* @return (std::vector<RenderElement*>&) The list of opaque render elements
		*/
		std::vector<RenderElement*>& opaque_elements(){ return opaque_elements_; }

		/**
		* @return (std::vector<RenderElement*>&) The list of ui render elements
		*/
		std::vector<RenderElement*>& ui_elements(){ return ui_elements_; }

		/**
		* @return (snuffbox::VertexBufferType&) The vertex buffer type currently in use
		*/
    VertexBufferType& vb_type(){ return vb_type_; }

		/**
		* @return (ID3D11Device*) The device itself
		*/
		ID3D11Device* device(){ return device_; }

		/// Creates the default texture to use in shaders
		void CreateDefaultTexture();

		/// Resets the current texture for reloading
		void ResetCurrentTexture(){ current_texture_ = nullptr; }

		/// Resets the current shader for reloading
		void ResetCurrentShader(){ current_shader_ = nullptr; }

		/// Resets the current model for reloading
		void ResetCurrentModel(){ current_model_ = nullptr; }
		
		/// Creates the context blend state
		void CreateBlendState();

		/// Clears the renderer for new use
    void Clear();

		/**
		* @brief Draws a given render element
		* @param[in] it (snuffbox::RenderElement*) The render element to draw
		*/
		void DrawRenderElement(RenderElement* it);

		/// Changes the buffer size after a window resize
		void ResizeBuffers();

		/**
		* @brief Sets the cull mode
		* @param[in] mode (D3D11_CULL_MODE) The cull mode to switch to
		*/
		void SetCullMode(D3D11_CULL_MODE mode);

		/**
		* @brief Sets the fullscreen mode
		* @param[in] mode (bool) The mode to switch to (true = fullscreen, false = windowed)
		*/
		void SetFullscreen(bool mode);

		/** 
		* @brief Adds a line to the line queue, to be drawn in the draw loop
		* @param[in] x1 (float) The starting x of the line segment
		* @param[in] y1 (float) The starting y of the line segment
		* @param[in] r1 (float) The starting red colour value to interpolate from
		* @param[in] g1 (float) The starting green colour value to interpolate from
		* @param[in] b1 (float) The starting blue colour value to interpolate from
		* @param[in] x2 (float) The ending x of the line segment
		* @param[in] y2 (float) The ending y of the line segment
		* @param[in] r2 (float) The ending red colour value to interpolate to
		* @param[in] g2 (float) The ending green colour value to interpolate to
		* @param[in] b2 (float) The ending blue colour value to interpolate to 
		*/
		void DrawLine(float x1, float y1, float z1, float r1, float g1, float b1, float x2, float y2, float z2, float r2, float g2, float b2);

		/**
		* @return (D3D11_VIEWPORT&) The current viewport
		*/
		D3D11_VIEWPORT& viewport();

		/**
		* @return (std::queue<RenderElement*>) Returns the render queue
		*/
		std::queue<RenderElement*>& render_queue();

    ID3D11ShaderResourceView* CreateTexture2D(int width, int height, DXGI_FORMAT pixelFormat, const void* buffer, const unsigned short stride);

		void DrawCurrent(int indices);

		void SetCurrentTexture(Texture* texture);
	private:
		SwapChainDescription					swap_desc_;							//!< The swap chain description to create the chain
		SwapChain*										swap_chain_;						//!< The swap chain for this device
		D3DDevice*										device_;								//!< The actual device
		D3DContext*										context_;								//!< The immediate context
		std::vector<DisplayAdapter*>	display_adapters_;			//!< The display adapters this device can use
		DisplayAdapter*								primary_adapter_;				//!< The primary display adapter
		D3DTexture2D*									back_buffer_;						//!< The backbuffer of this device
		D3DRenderTargetView*					render_target_view_;		//!< The render target view of this device
		ID3D11Buffer*									constant_buffer_;				//!< The constant buffer
		ID3D11Buffer*									uniform_buffer_;				//!< The constant buffer for uniforms
		ID3D11InputLayout*						input_layout_;					//!< The vertex input layout
		ID3D10Blob*										vs_buffer_;							//!< Vertex shader buffer
		ID3D10Blob*										ps_buffer_;							//!< Pixel shader buffer
		float													time_;									//!< The game time
		XMMATRIX											world_matrix_;					//!< The world matrix
		XMMATRIX											view_matrix_;						//!< The view matrix
		XMMATRIX											projection_matrix_;			//!< The projection matrix
		ID3D11RasterizerState*				rasterizer_state_;			//!< The rasterizer state
		std::vector<RenderElement*>		render_elements_;				//!< The list of render elements
		std::vector<RenderElement*>		opaque_elements_;				//!< The list of opaque render elements
		std::vector<RenderElement*>		ui_elements_;						//!< The list of ui render elements
    VertexBufferType              vb_type_;								//!< The vertex buffer type
		ID3D11DepthStencilView*				depth_stencil_view_;		//!< The depth stencil view
		ID3D11Texture2D*							depth_stencil_buffer_;	//!< The buffer of the depth stencil
		ID3D11DepthStencilState*			depth_state_;						//!< The depth stencil state
		ID3D11Texture2D*							no_texture_;						//!< A white rectangular texture as default
		ID3D11Texture2D*							no_normal_;							//!< A green rectangular texture as default normal
		ID3D11ShaderResourceView*			default_resource_;			//!< The default shader resource
		ID3D11ShaderResourceView*     default_normal_;				//!< The default shader normal map
		ID3D11SamplerState*						sampler_state_;					//!< The texture sampler state
		ID3D11ShaderResourceView*			current_texture_;				//!< The current texture being used
		ID3D11ShaderResourceView*			current_normal_;				//!< The current normal map being used
		Shader*												current_shader_;				//!< The current shader being used
		Camera*												camera_;								//!< The current camera being used
		ID3D11BlendState*							blend_state_;						//!< The blend state being used
    FBXModel*                     current_model_;					//!< The current model being used
    D3D11_PRIMITIVE_TOPOLOGY      topology_;							//!< The current primitive topology
		std::vector<Vertex>						lines_;									//!< The vector for lines to draw
		ID3D11Buffer*									line_buffer_;						//!< The line vertex buffer
		D3D11_VIEWPORT								viewport_;							//!< The viewport of the device
		std::queue<RenderElement*>		render_queue_;						//!< The render queue
	};
}