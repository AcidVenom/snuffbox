#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/d3d11/elements/render_element.h"
#include "../../snuffbox/d3d11/elements/quad_element.h"
#include "../../snuffbox/d3d11/elements/billboard_element.h"
#include "../../snuffbox/d3d11/elements/mesh_element.h"
#include "../../snuffbox/d3d11/elements/text_element.h"
#include "../../snuffbox/d3d11/d3d11_camera.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/d3d11/d3d11_shader.h"
#include "../../snuffbox/d3d11/d3d11_settings.h"
#include "../../snuffbox/memory/allocated_memory.h"
#include "../../snuffbox/content/content_manager.h"
#include "../../snuffbox/win32/win32_window.h"
#include "../../snuffbox/freetype/freetype_font.h"
#include "../../snuffbox/freetype/freetype_font_atlas.h"
#include "../../snuffbox/freetype/freetype_font_manager.h"

#include <fstream>
#include <comdef.h>

#define SNUFF_BASE_SHADER "cbuffer ConstantBuffer : register(b0)\n\
{\n\
\tfloat Time;\n\
\tfloat4x4 World;\n\
\tfloat4x4 View;\n\
\tfloat4x4 Projection;\n\
\tfloat4x4 WorldViewProjection;\n\
\tfloat Alpha;\n\
\tfloat3 Blend;\n\
\tfloat4x4 InvWorld;\n\
}\n\
\n\
cbuffer Uniforms : register(b1)\n\
{\n\
\n\
}\n\
\n\
struct VOut\n\
{\n\
\tfloat4 position : SV_POSITION;\n\
\tfloat4 colour : COLOUR;\n\
\tfloat3 normal : NORMAL;\n\
\tfloat2 texcoord : TEXCOORD0;\n\
};\n\
\n\
VOut VS(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD0, float4 colour : COLOUR)\n\
{\n\
\tVOut output;\n\
\toutput.position = mul(position, WorldViewProjection);\n\
\toutput.normal = normalize(mul(float4(normal, 0), InvWorld).xyz);\n\
\toutput.texcoord = texcoord;\n\
\toutput.colour = colour;\n\
\treturn output;\n\
}\n\
\n\
Texture2D textures[2];\n\
SamplerState SampleType;\n\
\n\
float4 PS(VOut input) : SV_TARGET\n\
{\n\
\tfloat4 textureColour = textures[0].Sample(SampleType, input.texcoord);\n\
\tfloat4 colour = float4(textureColour.rgb * Blend * input.colour.rgb, textureColour.a);\n\
\tcolour.a *= Alpha;\n\
\treturn colour;\n\
}"

namespace snuffbox
{
	namespace environment
	{
		namespace
		{
			D3D11DisplayDevice* globalInstance = nullptr;
		}

		bool has_render_device(){ return globalInstance != nullptr; }

		D3D11DisplayDevice& render_device()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}
}

namespace snuffbox
{
	//---------------------------------------------------------------------------------
	D3D11DisplayDevice::D3D11DisplayDevice() : 
		time_(0.0f), 
		vb_type_(VertexBufferType::kNone), 
		camera_(nullptr),
    current_model_(nullptr),
    topology_(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
	{
		environment::globalInstance = this;
	}

	//---------------------------------------------------------------------------------
  std::string D3D11DisplayDevice::HRToString(HRESULT hr, const char* subGroup = "")
	{
		_com_error error(hr);

		int size = WideCharToMultiByte(CP_UTF8, 0, error.ErrorMessage(), -1, NULL, 0, 0, 0);

		char* multistr = new char[size];
		WideCharToMultiByte(CP_UTF8, 0, error.ErrorMessage(), -1, multistr, size, 0, 0);

		std::string str = std::string("[D3D11] ") + subGroup + ": " + multistr;
		delete multistr;
		return str;
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::Initialise()
	{
		CreateDevice();
		GetAdapters();
		CreateBackBuffer();
		CreateViewport();
		CreateBaseShader();
		CreateConstantBuffer();
		CreateLayout();
		CreateDepthStencil();
		CreateSamplerState();
		CreateDefaultTexture();
		CreateBlendState();

		SNUFF_LOG_SUCCESS("Succesfully initialised the D3D11 display device");
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateDevice()
	{
		ZeroMemory(&swap_chain_, sizeof(SwapChain));
		ZeroMemory(&swap_desc_, sizeof(SwapChainDescription));

		swap_desc_.BufferCount = 1;
		swap_desc_.BufferDesc.Width = environment::game().window()->params().w;
		swap_desc_.BufferDesc.Height = environment::game().window()->params().h;
		swap_desc_.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_desc_.BufferDesc.RefreshRate.Numerator = 60;
		swap_desc_.BufferDesc.RefreshRate.Denominator = 1;
		swap_desc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_desc_.OutputWindow = environment::game().window()->handle();
		swap_desc_.SampleDesc.Count = 1;
		swap_desc_.SampleDesc.Quality = 0;
		swap_desc_.Windowed = !environment::render_settings().settings().fullscreen;
		swap_desc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swap_desc_.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap_desc_.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swap_desc_.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		HRESULT result = S_OK;

		D3D_FEATURE_LEVEL featureLevelsRequested = D3D_FEATURE_LEVEL_10_0;
		D3D_FEATURE_LEVEL featureLevelsSupported;

		UINT deviceFlags = 0;

#ifdef _DEBUG
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		if (FAILED(result = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			deviceFlags,
			&featureLevelsRequested,
			1,
			D3D11_SDK_VERSION,
			&swap_desc_,
			&swap_chain_,
			&device_,
			&featureLevelsSupported,
			&context_
			)))
		{
			SNUFF_ASSERT(HRToString(result));
		}

		SNUFF_LOG_INFO("Created the D3D11 display device");
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::GetAdapters()
	{
		DisplayAdapter* adapter;
		IDXGIFactory* factory = NULL;

		HRESULT hr = S_OK;

		if (FAILED(hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
		{
			SNUFF_ASSERT(HRToString(hr));
		}

		DXGI_ADAPTER_DESC bestAdapter;
		int highestVram = 0;

		for (UINT it = 0; factory->EnumAdapters(it, &adapter) != DXGI_ERROR_NOT_FOUND; ++it)
		{
			display_adapters_.push_back(adapter);
			DXGI_ADAPTER_DESC description;
			if (FAILED(hr = adapter->GetDesc(&description)))
			{
				SNUFF_ASSERT(HRToString(hr));
			}

			int vram = static_cast<int>(description.DedicatedVideoMemory / 1024 / 1024);
			
			if (vram > highestVram)
			{
				highestVram = vram;
				bestAdapter = description;
				primary_adapter_ = adapter;
			}
		}

		int vram = static_cast<int>(bestAdapter.DedicatedVideoMemory / 1024 / 1024);
		int sharedmemory = static_cast<int>(bestAdapter.SharedSystemMemory / 1024 / 1024);
		WCHAR* adapterName = bestAdapter.Description;

		_bstr_t name(adapterName);

		std::string result = "Picked a video card to use for the display device";

		result += "\n\tVideo card name: " + name + "\n";
		result += "\tVirtual memory: " + std::to_string(vram) + "\n";
		result += "\tShared system memory: " + std::to_string(sharedmemory);

		SNUFF_LOG_INFO(result.c_str());

		if (factory)
		{
			factory->Release();
		}
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateBackBuffer()
	{
		HRESULT result = S_OK;

		result = swap_chain_->GetBuffer(0, __uuidof(D3DTexture2D),
			(LPVOID*)&back_buffer_);

		SNUFF_XASSERT(result == S_OK, HRToString(result));

		result = device_->CreateRenderTargetView(back_buffer_, NULL,
			&render_target_view_);

		SNUFF_XASSERT(result == S_OK, HRToString(result))
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateLayout()
	{
		HRESULT result = S_OK;

		D3D11_INPUT_ELEMENT_DESC layout[] = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		result = device_->CreateInputLayout(layout, 4, vs_buffer_->GetBufferPointer(),vs_buffer_->GetBufferSize(), &input_layout_);
		SNUFF_XASSERT(result == S_OK, HRToString(result, "Input Layout").c_str());

		context_->IASetInputLayout(input_layout_);
		context_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		D3D11_RASTERIZER_DESC rasterizer;
		ZeroMemory(&rasterizer, sizeof(D3D11_RASTERIZER_DESC));
		rasterizer.CullMode = D3D11_CULL_FRONT;
		rasterizer.FillMode = D3D11_FILL_SOLID;
    rasterizer.MultisampleEnable = TRUE;
    rasterizer.AntialiasedLineEnable = TRUE;

		result = device_->CreateRasterizerState(&rasterizer, &rasterizer_state_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

    context_->RSSetState(rasterizer_state_);
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::SetCullMode(D3D11_CULL_MODE mode)
	{
		context_->RSSetState(0);
		HRESULT result = S_OK;

		D3D11_RASTERIZER_DESC rasterizer;
		ZeroMemory(&rasterizer, sizeof(D3D11_RASTERIZER_DESC));
		rasterizer.CullMode = mode;
		rasterizer.FillMode = D3D11_FILL_SOLID;

		SNUFF_SAFE_RELEASE(rasterizer_state_);
		result = device_->CreateRasterizerState(&rasterizer, &rasterizer_state_);
		context_->RSSetState(rasterizer_state_);
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::SetFullscreen(bool mode)
	{
		swap_chain_->SetFullscreenState(mode, NULL);
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateBaseShader()
	{
		std::fstream fin;

		fin.open(environment::game().path() + "/shaders/base.fx");

		if (!fin)
		{
			std::ofstream out;
			out.open(environment::game().path() + "/shaders/base.fx");

			if (!out)
			{
				CreateDirectoryA(std::string(environment::game().path() + "/shaders").c_str(), 0);
				out.open(environment::game().path() + "/shaders/base.fx");

				SNUFF_XASSERT(out, "Unknown error in creating of the base shader");
			}

			out << SNUFF_BASE_SHADER;
			out.close();
		}
		
		fin.close();

		environment::content_manager().Load<Shader>("shaders/base.fx");
	}

	//---------------------------------------------------------------------------------
	Shaders D3D11DisplayDevice::LoadShader(const char* path)
	{
		VertexShader* vs;
		PixelShader* ps;

		if (vs_buffer_ != NULL)
		{
			vs_buffer_->Release();
			vs_buffer_ = NULL;
		}

		if (ps_buffer_ != NULL)
		{
			ps_buffer_->Release();
			ps_buffer_ = NULL;
		}

		ID3D10Blob* errors = nullptr;
		HRESULT result = S_OK;
		std::string file_path = environment::game().path() + "/" + path;
		
		result = D3DX10CompileFromFileA(file_path.c_str(), 0, 0, "VS", "vs_4_0", D3D10_SHADER_PACK_MATRIX_ROW_MAJOR, 0, 0, &vs_buffer_, &errors, 0);
		if (errors != nullptr)
		{
			SNUFF_LOG_ERROR(static_cast<const char*>(errors->GetBufferPointer()));
			return Shaders{ nullptr, nullptr };
		}

		SNUFF_XASSERT(result == S_OK, HRToString(result, (std::string("Vertex Shader ") + file_path).c_str()).c_str());
		result = D3DX10CompileFromFileA(file_path.c_str(), 0, 0, "PS", "ps_4_0", D3D10_SHADER_PACK_MATRIX_ROW_MAJOR, 0, 0, &ps_buffer_, &errors, 0);
		if (errors != nullptr)
		{
			SNUFF_LOG_ERROR(static_cast<const char*>(errors->GetBufferPointer()));
			return Shaders{ nullptr, nullptr };
		}
		SNUFF_XASSERT(result == S_OK, HRToString(result, (std::string("Pixel Shader ") + file_path).c_str()).c_str());

		result = device_->CreateVertexShader(vs_buffer_->GetBufferPointer(), vs_buffer_->GetBufferSize(), NULL, &vs);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
		result = device_->CreatePixelShader(ps_buffer_->GetBufferPointer(), ps_buffer_->GetBufferSize(), NULL, &ps);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		if (errors != nullptr)
		{
			errors->Release();
		}

		return Shaders{ vs, ps };
	}

	void D3D11DisplayDevice::CreateConstantBuffer()
	{
		HRESULT result = S_OK;
		ShaderConstantBuffer vsConstantBuffer;
		float uniforms[4096];

		D3D11_BUFFER_DESC constantBufferDesc;
		constantBufferDesc.ByteWidth = sizeof(ShaderConstantBuffer)* 4;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;
		constantBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vsConstantBuffer;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		result = device_->CreateBuffer(&constantBufferDesc, &initData, &constant_buffer_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		constantBufferDesc.ByteWidth = sizeof(float)* 4096;
		initData.pSysMem = &uniforms;

		result = device_->CreateBuffer(&constantBufferDesc, &initData, &uniform_buffer_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
	}

	void D3D11DisplayDevice::CreateViewport()
	{
		D3D11_VIEWPORT viewport;
		DXGI_SWAP_CHAIN_DESC scDesc;

		swap_chain_->GetDesc(&scDesc);

		float windowWidth = environment::game().window()->params().w;
		float windowHeight = environment::game().window()->params().h;

		float rw = environment::render_settings().settings().resolution.w;
		float rh = environment::render_settings().settings().resolution.h;

		float targetAspectRatio =  rw / rh;
		float currentAspectRatio = windowWidth / windowHeight;

		float w = windowWidth;
		float h = windowHeight;

		if (targetAspectRatio > currentAspectRatio) {
			w = windowWidth;
			h = w / targetAspectRatio;
		}
		else {
			h = windowHeight;
			w = h * targetAspectRatio;
		}

		float topLeftX = windowWidth / 2 - w / 2;
		float topLeftY = windowHeight / 2 - h / 2;

		viewport.TopLeftX = topLeftX;
		viewport.TopLeftY = topLeftY;
		viewport.Width = w;
		viewport.Height = h;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		context_->RSSetViewports(1, &viewport);

		viewport_ = viewport;
	}

	//---------------------------------------------------------------------------------
	D3D11_VIEWPORT& D3D11DisplayDevice::viewport()
	{
		return viewport_;
	}

	//---------------------------------------------------------------------------------
	ID3D11Buffer* D3D11DisplayDevice::CreateVertexBuffer(const std::vector<Vertex>& vertices)
	{
		HRESULT result = S_OK;
		ID3D11Buffer* vertexBuffer;
		D3D11_BUFFER_DESC vbDesc;

		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex)* vertices.size());
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = 0;
		vbDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA inputData;
		inputData.pSysMem = &vertices[0];

		device_->CreateBuffer(&vbDesc, &inputData, &vertexBuffer);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		return vertexBuffer;
	}

	//---------------------------------------------------------------------------------
	ID3D11Buffer* D3D11DisplayDevice::CreateIndexBuffer(const std::vector<unsigned int>& indices)
	{
		HRESULT result = S_OK;
		ID3D11Buffer* indexBuffer;
		D3D11_BUFFER_DESC ibDesc;

		ibDesc.Usage = D3D11_USAGE_DEFAULT;
		ibDesc.ByteWidth = static_cast<UINT>(sizeof(unsigned int)* indices.size());
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.CPUAccessFlags = 0;
		ibDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA inputData;
		inputData.pSysMem = &indices[0];

		device_->CreateBuffer(&ibDesc, &inputData, &indexBuffer);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		return indexBuffer;
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateDepthStencil()
	{
		HRESULT result = S_OK;

		D3D11_TEXTURE2D_DESC dsvDesc;
		DXGI_SWAP_CHAIN_DESC scDesc;
		swap_chain_->GetDesc(&scDesc);

		dsvDesc.Width = scDesc.BufferDesc.Width;
		dsvDesc.Height = scDesc.BufferDesc.Height;
		dsvDesc.MipLevels = 1;
		dsvDesc.ArraySize = 1;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		dsvDesc.SampleDesc.Count = 1;
		dsvDesc.SampleDesc.Quality = 0;
		dsvDesc.Usage = D3D11_USAGE_DEFAULT;
		dsvDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsvDesc.CPUAccessFlags = 0;
		dsvDesc.MiscFlags = 0;

		result = device_->CreateTexture2D(&dsvDesc, NULL, &depth_stencil_buffer_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
		result = device_->CreateDepthStencilView(depth_stencil_buffer_, NULL, &depth_stencil_view_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		D3D11_DEPTH_STENCIL_DESC dsDesc;

		// Depth test parameters
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		result = device_->CreateDepthStencilState(&dsDesc, &depth_state_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		context_->OMSetDepthStencilState(depth_state_,1);
	}
	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateSamplerState()
	{
		HRESULT result = S_OK;
		D3D11_SAMPLER_DESC sDesc;

		ZeroMemory(&sDesc, sizeof(D3D11_SAMPLER_DESC));
    sDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.MipLODBias = 0.0f;
		sDesc.MaxAnisotropy = 4;
    sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sDesc.BorderColor[0] = 0;
		sDesc.BorderColor[1] = 0;
		sDesc.BorderColor[2] = 0;
		sDesc.BorderColor[3] = 0;
		sDesc.MinLOD = 0;
		sDesc.MaxLOD = D3D11_FLOAT32_MAX;

		result = device_->CreateSamplerState(&sDesc, &sampler_state_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		context_->PSSetSamplers(0, 1, &sampler_state_);
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateDefaultTexture()
	{
		HRESULT result = S_OK;
		D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);

		D3D11_TEXTURE2D_DESC whiteTexture;
		ZeroMemory(&whiteTexture, sizeof(D3D11_TEXTURE2D_DESC));
		whiteTexture.ArraySize = 1;
		whiteTexture.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		whiteTexture.CPUAccessFlags = 0;
    whiteTexture.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		whiteTexture.Height = 1;
		whiteTexture.MipLevels = 1;
		whiteTexture.MiscFlags = 0;
		whiteTexture.SampleDesc.Count = 1;
		whiteTexture.SampleDesc.Quality = 0;
		whiteTexture.Usage = D3D11_USAGE_DEFAULT;
		whiteTexture.Width = 1;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = &color;
		initData.SysMemPitch = sizeof(color);
		initData.SysMemSlicePitch = sizeof(color);

		result = device_->CreateTexture2D(&whiteTexture, &initData, &no_texture_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
		result = device_->CreateShaderResourceView(no_texture_, NULL, &default_resource_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		D3DXCOLOR col(0.51f, 0.54f, 0.98f, 1.0f);

		D3D11_TEXTURE2D_DESC normalTexture;
		ZeroMemory(&normalTexture, sizeof(D3D11_TEXTURE2D_DESC));
		normalTexture.ArraySize = 1;
		normalTexture.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		normalTexture.CPUAccessFlags = 0;
    normalTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		normalTexture.Height = 1;
		normalTexture.MipLevels = 1;
		normalTexture.MiscFlags = 0;
		normalTexture.SampleDesc.Count = 1;
		normalTexture.SampleDesc.Quality = 0;
		normalTexture.Usage = D3D11_USAGE_DEFAULT;
		normalTexture.Width = 1;

		D3D11_SUBRESOURCE_DATA iData;
		ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
		iData.pSysMem = &col;
		iData.SysMemPitch = sizeof(col);
		iData.SysMemSlicePitch = sizeof(col);

		result = device_->CreateTexture2D(&normalTexture, &iData, &no_normal_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
		result = device_->CreateShaderResourceView(no_normal_, NULL, &default_normal_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateBlendState()
	{
		HRESULT result = S_OK;

		D3D11_BLEND_DESC bDesc;

		ZeroMemory(&bDesc, sizeof(D3D11_BLEND_DESC));
		bDesc.AlphaToCoverageEnable = false;
		bDesc.IndependentBlendEnable = false;
		bDesc.RenderTarget[0].BlendEnable = true;
		bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		result = device_->CreateBlendState(&bDesc, &blend_state_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		float factor[4] = { 1, 1, 1, 1 };
		context_->OMSetBlendState(blend_state_,factor,0xFFFFFFFF);
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::DrawLine(float x1, float y1, float z1, float r1, float g1, float b1, float x2, float y2, float z2, float r2, float g2, float b2)
	{
		Vertex vert;
		vert.x = x1;
		vert.y = y1;
		vert.z = z1;
		vert.w = 1.0f;
		vert.tex_coord.x = 0;
		vert.tex_coord.y = 0;
		vert.normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vert.colour.x = r1;
		vert.colour.y = g1;
		vert.colour.z = b1;
		vert.colour.w = 1.0f;

		lines_.push_back(vert);

		vert.x = x2;
		vert.y = y2;
		vert.z = z2;
		vert.w = 1.0f;
		vert.tex_coord.x = 0;
		vert.tex_coord.y = 0;
		vert.normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vert.colour.x = r2;
		vert.colour.y = g2;
		vert.colour.z = b2;
		vert.colour.w = 1.0f;

		lines_.push_back(vert);
	}
	
	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::StartDraw()
	{
		if (camera_ && camera_->type() == Camera::CameraType::kPerspective)
		{
			context_->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);
		}
		else
		{
			context_->OMSetRenderTargets(1, &render_target_view_, NULL);
		}
		context_->ClearRenderTargetView(render_target_view_, environment::render_settings().settings().buffer_colour);
		context_->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		struct RenderSorterByZ
		{
			inline bool operator()(RenderElement* a, RenderElement* b)
			{
				return (a->z() > b->z());
			}
		} RenderSorterByZ;

		struct RenderSorterDistance
		{
			inline bool operator()(RenderElement* a, RenderElement* b)
			{
				return (a->distance_from_camera() < b->distance_from_camera());
			}
		} RenderSorterDistance;

		if (camera_)
		{
			if (camera_->type() == Camera::CameraType::kOrthographic)
			{
				std::sort(render_elements_.begin(), render_elements_.end(), RenderSorterByZ);
			}
			else
			{
				std::sort(render_elements_.begin(), render_elements_.end(), RenderSorterDistance);
			}
		}
	}

  //---------------------------------------------------------------------------------
  void D3D11DisplayDevice::Clear()
  {
    for (auto it : render_elements_)
    {
      it->set_destroyed(true);
    }
    render_elements_.clear();
    
    for (auto it : opaque_elements_)
    {
      it->set_destroyed(true);
    }
    opaque_elements_.clear();

    for (auto it : ui_elements_)
    {
      it->set_destroyed(true);
    }
    ui_elements_.clear();

    while (!render_queue_.empty())
    {
      render_queue_.pop();
    }
  }

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::DrawRenderElement(RenderElement* it)
	{
		if (it && it->visible())
		{
			RenderElement::ElementTypes elementType = it->element_type();
			VertexBufferType type = it->type();
			if (type != vb_type_ || type == VertexBufferType::kMesh || type == VertexBufferType::kText)
			{
        if (type == VertexBufferType::kMesh)
        {
          Mesh* mesh = static_cast<Mesh*>(it);
          FBXModel* model = mesh->model();
          if (model != current_model_ || type != vb_type_)
          {
            it->SetBuffers();
            current_model_ = model;
          }
        }
        else
        {
          it->SetBuffers();
        }
				vb_type_ = type;
			}

			world_matrix_ = it->world_matrix(camera_);

			D3D11_MAPPED_SUBRESOURCE cbData;
			ShaderConstantBuffer* mappedData;
			float* uniforms;

			context_->VSSetConstantBuffers(0, 1, &constant_buffer_);
			context_->PSSetConstantBuffers(0, 1, &constant_buffer_);

			context_->Map(constant_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbData);
      XMVECTOR deter;
			mappedData = static_cast<ShaderConstantBuffer*>(cbData.pData);
			mappedData->Time = time_;
			mappedData->World = world_matrix_;
			mappedData->View = view_matrix_;
			mappedData->Projection = projection_matrix_;
			mappedData->WorldViewProjection = world_matrix_ * view_matrix_ * projection_matrix_;
      mappedData->InvWorld = XMMatrixTranspose(XMMatrixInverse(&deter, world_matrix_));
			mappedData->Alpha = it->alpha();
			mappedData->Blend = it->blend();

			context_->Unmap(constant_buffer_, 0);

			context_->VSSetConstantBuffers(1, 1, &uniform_buffer_);
			context_->PSSetConstantBuffers(1, 1, &uniform_buffer_);

			context_->Map(uniform_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbData);

			uniforms = static_cast<float*>(cbData.pData);
			auto vec = it->uniforms();
			for (unsigned int i = 0; i < vec.size(); ++i)
			{
				uniforms[i] = vec[i];
			}

			context_->Unmap(uniform_buffer_, 0);

			ID3D11ShaderResourceView* textures[2];
			textures[0] = default_resource_;
			textures[1] = default_normal_;

			if (it->normal_map())
			{
				textures[1] = it->normal_map()->resource();
			}

			if (it->texture())
			{
				textures[0] = it->texture()->resource();
			}

			if (current_texture_ != textures[0] || current_normal_ != textures[1])
			{
				context_->PSSetShaderResources(0, 2, textures);
			}

			current_texture_ = textures[0];
			current_normal_ = textures[1];

			if (it->shader())
			{
				if (current_shader_ != it->shader())
				{
					auto shaders = it->shader()->shaders();
					context_->PSSetShader(shaders.ps, 0, 0);
					context_->VSSetShader(shaders.vs, 0, 0);
					current_shader_ = it->shader();
				}
			}

			if (elementType != RenderElement::ElementTypes::kMesh)
			{
				if (topology_ != D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
				{
					context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				}
				 
				context_->DrawIndexed(static_cast<UINT>(it->indices().size()), 0, 0);

        topology_ = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			}
			else
			{
				if (topology_ != D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
				{
					context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				}

				Mesh* mesh = static_cast<Mesh*>(it);
        context_->Draw(mesh->model()->vertex_count(), 0);
        topology_ = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			}
		}
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::Draw()
	{
		if (!camera_) return;

		RenderElement* it = nullptr;

		for (int idx = static_cast<int>(opaque_elements_.size()-1); idx >= 0; --idx)
		{
			it = opaque_elements_[idx];
			DrawRenderElement(it);

			if (it->destroyed())
			{
				opaque_elements_.erase(opaque_elements_.begin() + idx);
			}
		}

		XMVECTOR camTranslation = camera_->target();
		XMVECTOR translation;
		XMVECTOR delta;
		float distance;

		for (int idx = static_cast<int>(render_elements_.size() - 1); idx >= 0; --idx)
		{
			it = render_elements_[idx];

			if (camera_->type() == Camera::CameraType::kPerspective)
			{
				translation = it->translation();
				delta = translation - camTranslation;
				distance = sqrt(XMVectorGetX(delta)*XMVectorGetX(delta) + XMVectorGetY(delta)*XMVectorGetY(delta) + XMVectorGetZ(delta)*XMVectorGetZ(delta));

				it->set_distance_from_camera(distance);
			}
			DrawRenderElement(it);

			if (it->destroyed())
			{
				render_elements_.erase(render_elements_.begin() + idx);
			}
		}

		if (lines_.size() > 0)
		{
			if (line_buffer_)
			{
				line_buffer_->Release();
				line_buffer_ = nullptr;
			}
			line_buffer_ = CreateVertexBuffer(lines_);
			if (topology_ != D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST)
			{
				context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
			}
			SetVertexBuffer(line_buffer_);

			vb_type_ = VertexBufferType::kLine;

			world_matrix_ = XMMatrixIdentity();

			D3D11_MAPPED_SUBRESOURCE cbData;
			ShaderConstantBuffer* mappedData;

			context_->VSSetConstantBuffers(0, 1, &constant_buffer_);
			context_->PSSetConstantBuffers(0, 1, &constant_buffer_);

			context_->Map(constant_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbData);
			XMVECTOR deter;
			mappedData = static_cast<ShaderConstantBuffer*>(cbData.pData);
			mappedData->Time = time_;
			mappedData->World = world_matrix_;
			mappedData->View = view_matrix_;
			mappedData->Projection = projection_matrix_;
			mappedData->WorldViewProjection = world_matrix_ * view_matrix_ * projection_matrix_;
			mappedData->InvWorld = XMMatrixTranspose(XMMatrixInverse(&deter, world_matrix_));
			mappedData->Alpha = 1;
			mappedData->Blend = XMFLOAT3(1, 1, 1);

			context_->Unmap(constant_buffer_, 0);
			Shaders shaders = environment::content_manager().Get<Shader>("shaders/base.fx").get()->shaders();

			ID3D11ShaderResourceView* textures[2];
			textures[0] = default_resource_;
			textures[1] = default_normal_;

			context_->PSSetShaderResources(0, 2, textures);

			current_texture_ = nullptr;
			current_normal_ = nullptr;

			context_->PSSetShader(shaders.ps, 0, 0);
			context_->VSSetShader(shaders.vs, 0, 0);

			topology_ = D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
			context_->Draw(static_cast<UINT>(lines_.size()), static_cast<UINT>(0));
		}

		context_->OMSetRenderTargets(1, &render_target_view_, NULL);

		SwapChainDescription swapDesc;
		swap_chain_->GetDesc(&swapDesc);

    projection_matrix_ = XMMatrixOrthographicRH(environment::render_settings().settings().resolution.w, environment::render_settings().settings().resolution.h, 1.0f, 1000.0f);
		view_matrix_ = XMMatrixIdentity();

		struct RenderSorterByZ
		{
			inline bool operator()(RenderElement* a, RenderElement* b)
			{
				return (a->z() > b->z());
			}
		} RenderSorterByZ;

		std::sort(ui_elements_.begin(), ui_elements_.end(), RenderSorterByZ);

		for (int idx = static_cast<int>(ui_elements_.size() - 1); idx >= 0; --idx)
		{
			it = ui_elements_[idx];
			if (it->type() == RenderElement::ElementTypes::kText)
			{
				Text* ptr = static_cast<Text*>(it);
				
				ptr->set_texture(environment::font_manager().atlas()->texture());

				if (ptr->shadow_set() == true)
				{
					XMFLOAT3 pre_blend = ptr->blend();
					float pre_alpha = ptr->alpha();

					ptr->PrepareShadow();
					DrawRenderElement(ptr);
					ptr->Reset(pre_blend, pre_alpha);
				}

				DrawRenderElement(ptr);
			}
			else
			{
				DrawRenderElement(it);
			}

			if (it->destroyed())
			{
				ui_elements_.erase(ui_elements_.begin() + idx);
			}
		}

		camera_ = nullptr;
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::SetCamera(Camera* camera)
	{
		camera_ = camera;
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::UpdateCamera(Camera* camera)
	{
		world_matrix_ = XMMatrixIdentity();
		view_matrix_ = camera->view_matrix();
		camera_ = camera;

		SwapChainDescription swapDesc;
		swap_chain_->GetDesc(&swapDesc);
    unsigned int w = environment::render_settings().settings().resolution.w;
    unsigned int h = environment::render_settings().settings().resolution.h;

		if (camera->type() == Camera::CameraType::kOrthographic)
		{
			projection_matrix_ = XMMatrixOrthographicRH(w*(1/camera->zoom()), h*(1/camera->zoom()), 1.0f, 1000.0f);
		}
		else
		{
      projection_matrix_ = XMMatrixPerspectiveFovRH(camera->fov(), static_cast<float>(w / h), 1.0f, 1000.0f);
		}
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::EndDraw()
	{
		swap_chain_->Present(environment::render_settings().settings().vsync, 0);
		std::map<RenderElement*, bool> exists;
		while (!render_queue_.empty())
		{
			RenderElement* it = render_queue_.front();
			render_queue_.pop();

			if (exists.find(it) != exists.end() || !it->spawned())
			{
				continue;
			}

			exists.emplace(it, true);

			if (it->element_type() == RenderElement::ElementTypes::kTerrain)
			{
				environment::render_device().opaque_elements().push_back(it);
			}
      else if (it->element_type() == RenderElement::ElementTypes::kWidget || it->element_type() == RenderElement::ElementTypes::kText)
			{
				environment::render_device().ui_elements().push_back(it);
			}
			else
			{
				environment::render_device().render_elements().push_back(it);
			}

			it->set_destroyed(false);
		}
		lines_.clear();
		if (line_buffer_)
		{
			line_buffer_->Release();
			line_buffer_ = nullptr;
		}
	}

  //---------------------------------------------------------------------------------
  ID3D11ShaderResourceView* D3D11DisplayDevice::CreateTexture2D(int width, int height, DXGI_FORMAT pixelFormat, const void* buffer, const unsigned short stride)
  {
    ID3D11Texture2D* texture = nullptr;
    ID3D11ShaderResourceView* resource = nullptr;

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = static_cast<UINT>(width);
    desc.Height = static_cast<UINT>(height);
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = pixelFormat;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;

    initData.pSysMem = buffer;
    initData.SysMemPitch = stride;
    initData.SysMemSlicePitch = stride;

    HRESULT hr = S_OK;

    hr = device_->CreateTexture2D(&desc, &initData, &texture);
    SNUFF_XASSERT(hr == S_OK, "Failed creating a texture!");

    hr = device_->CreateShaderResourceView(texture, NULL, &resource);
    SNUFF_XASSERT(hr == S_OK, "Failed creating a texture!");

    SNUFF_SAFE_RELEASE(texture);

    return resource;
  }

	//---------------------------------------------------------------------------------
	std::queue<RenderElement*>& D3D11DisplayDevice::render_queue()
	{
		return render_queue_;
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::ResizeBuffers()
	{
		HRESULT result = S_OK;

		SNUFF_SAFE_RELEASE(depth_stencil_buffer_);
		SNUFF_SAFE_RELEASE(render_target_view_);
		SNUFF_SAFE_RELEASE(depth_stencil_view_);
		SNUFF_SAFE_RELEASE(depth_state_);
		SNUFF_SAFE_RELEASE(sampler_state_);
		SNUFF_SAFE_RELEASE(blend_state_);
		SNUFF_SAFE_RELEASE(rasterizer_state_);
		SNUFF_SAFE_RELEASE(back_buffer_);
		SNUFF_SAFE_RELEASE(input_layout_);

		unsigned int w = environment::game().window()->params().w;
		unsigned int h = environment::game().window()->params().h;

		result = swap_chain_->ResizeBuffers(1, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		CreateBackBuffer();
		CreateViewport();
		CreateLayout();
		CreateDepthStencil();
		CreateSamplerState();
		CreateBlendState();
    SetCullMode(environment::render_settings().settings().cull_mode);

		context_->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::Destroy()
	{
		swap_chain_->SetFullscreenState(FALSE, NULL);

		SNUFF_SAFE_RELEASE(device_);
		SNUFF_SAFE_RELEASE(context_);
		SNUFF_SAFE_RELEASE(swap_chain_);
		for (auto it : display_adapters_)
		{
			SNUFF_SAFE_RELEASE(it);
		}
		SNUFF_SAFE_RELEASE(render_target_view_);
		SNUFF_SAFE_RELEASE(back_buffer_);
		SNUFF_SAFE_RELEASE(input_layout_);
		SNUFF_SAFE_RELEASE(vs_buffer_);
		SNUFF_SAFE_RELEASE(ps_buffer_);
		SNUFF_SAFE_RELEASE(constant_buffer_);
		SNUFF_SAFE_RELEASE(uniform_buffer_);
		SNUFF_SAFE_RELEASE(rasterizer_state_);
		SNUFF_SAFE_RELEASE(depth_stencil_buffer_);
		SNUFF_SAFE_RELEASE(depth_stencil_view_);
		SNUFF_SAFE_RELEASE(depth_state_);
		SNUFF_SAFE_RELEASE(sampler_state_);
		SNUFF_SAFE_RELEASE(no_texture_);
		SNUFF_SAFE_RELEASE(default_resource_);
		SNUFF_SAFE_RELEASE(no_normal_);
		SNUFF_SAFE_RELEASE(default_normal_);
		SNUFF_SAFE_RELEASE(blend_state_);

		if (line_buffer_)
		{
			line_buffer_->Release();
			line_buffer_ = nullptr;
		}

		SNUFF_LOG_INFO("Destroyed the D3D11 display device");
		environment::globalInstance = nullptr;
	}
}