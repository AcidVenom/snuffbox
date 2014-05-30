#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/d3d11/elements/render_element.h"
#include "../../snuffbox/d3d11/elements/quad_element.h"
#include "../../snuffbox/d3d11/elements/billboard_element.h"
#include "../../snuffbox/d3d11/d3d11_camera.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/d3d11/d3d11_shader.h"
#include "../../snuffbox/memory/allocated_memory.h"
#include "../../snuffbox/content/content_manager.h"
#include <comdef.h>

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
		vbType_(VertexBufferType::kNone), 
		camera_(nullptr)
	{
		environment::globalInstance = this;
	}

	//---------------------------------------------------------------------------------
	std::basic_string<TCHAR> D3D11DisplayDevice::HRToString(HRESULT hr, const char* subGroup = "")
	{
		_com_error error(hr);
		std::basic_string<TCHAR> str = std::string("[D3D11] ") + subGroup + ": " + error.ErrorMessage();
		return str;
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::Initialise()
	{
		CreateDevice();
		GetAdapters();
		CreateBackBuffer();
		CreateViewport();
		environment::content_manager().Load<Shader>("shaders/base.fx");
		CreateConstantBuffer();
		CreateLayout();
		CreateDepthStencil();
		CreateSamplerState();
		CreateDefaultTexture();
		CreateBlendState();
		context_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateDevice()
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
		swapDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT result = S_OK;

		D3D_FEATURE_LEVEL featureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
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
			&swapDesc_,
			&swapChain_,
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
			displayAdapters_.push_back(adapter);
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
				primaryAdapter_ = adapter;
			}
		}

		int vram = static_cast<int>(bestAdapter.DedicatedVideoMemory / 1024 / 1024);
		int sharedmemory = static_cast<int>(bestAdapter.SharedSystemMemory / 1024 / 1024);
		WCHAR* adapterName = bestAdapter.Description;

		_bstr_t name(adapterName);

		std::string result = "";

		result += "\n\nVideo card name: " + name + "\n";
		result += "Virtual memory: " + std::to_string(vram) + "\n";
		result += "Shared system memory: " + std::to_string(sharedmemory) + "\n";

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

		result = swapChain_->GetBuffer(0, __uuidof(D3DTexture2D),
			(LPVOID*)&backBuffer_);

		SNUFF_XASSERT(result == S_OK, HRToString(result));

		result = device_->CreateRenderTargetView(backBuffer_, NULL,
			&renderTargetView_);

		SNUFF_XASSERT(result == S_OK, HRToString(result))
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateLayout()
	{
		HRESULT result = S_OK;

		D3D11_INPUT_ELEMENT_DESC layout[] = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		result = device_->CreateInputLayout(layout, 4, vsBuffer_->GetBufferPointer(),vsBuffer_->GetBufferSize(), &inputLayout_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		context_->IASetInputLayout(inputLayout_);
		context_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		D3D11_RASTERIZER_DESC rasterizer;
		ZeroMemory(&rasterizer, sizeof(D3D11_RASTERIZER_DESC));
		rasterizer.CullMode = D3D11_CULL_NONE;
		rasterizer.FillMode = D3D11_FILL_SOLID;

		result = device_->CreateRasterizerState(&rasterizer, &rasterizerState_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

    context_->RSSetState(rasterizerState_);
	}

	//---------------------------------------------------------------------------------
	Shaders D3D11DisplayDevice::LoadShader(const char* path)
	{
		VertexShader* vs;
		PixelShader* ps;

		if (vsBuffer_ != NULL)
		{
			vsBuffer_->Release();
			vsBuffer_ = NULL;
		}

		if (psBuffer_ != NULL)
		{
			psBuffer_->Release();
			psBuffer_ = NULL;
		}

		ID3D10Blob* errors = nullptr;
		HRESULT result = S_OK;
		std::string file_path = environment::game().path() + "/" + path;
		
		result = D3DX11CompileFromFileA(file_path.c_str(), 0, 0, "VS", "vs_5_0", D3D10_SHADER_PACK_MATRIX_ROW_MAJOR, 0, 0, &vsBuffer_, &errors, 0);
		if (errors != nullptr)
		{
			SNUFF_LOG_ERROR(static_cast<const char*>(errors->GetBufferPointer()));
			return Shaders{ nullptr, nullptr };
		}
		SNUFF_XASSERT(result == S_OK, HRToString(result, (std::string("Shader ") + file_path).c_str()).c_str());
		result = D3DX11CompileFromFileA(file_path.c_str(), 0, 0, "PS", "ps_5_0", D3D10_SHADER_PACK_MATRIX_ROW_MAJOR, 0, 0, &psBuffer_, &errors, 0);
		if (errors != nullptr)
		{
			SNUFF_LOG_ERROR(static_cast<const char*>(errors->GetBufferPointer()));
			return Shaders{ nullptr, nullptr };
		}
		SNUFF_XASSERT(result == S_OK, HRToString(result, (std::string("Shader ") + file_path).c_str()).c_str());

		result = device_->CreateVertexShader(vsBuffer_->GetBufferPointer(), vsBuffer_->GetBufferSize(), NULL, &vs);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
		result = device_->CreatePixelShader(psBuffer_->GetBufferPointer(), psBuffer_->GetBufferSize(), NULL, &ps);
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
		VS_CONSTANT_BUFFER vsConstantBuffer;

		D3D11_BUFFER_DESC constantBufferDesc;
		constantBufferDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER)* 4;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;
		constantBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vsConstantBuffer;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		result = device_->CreateBuffer(&constantBufferDesc, &initData, &vsConstantBuffer_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		context_->VSSetConstantBuffers(0, 1, &vsConstantBuffer_);
		context_->PSSetConstantBuffers(0, 1, &vsConstantBuffer_);
	}

	void D3D11DisplayDevice::CreateViewport()
	{
		D3D11_VIEWPORT viewport;
		DXGI_SWAP_CHAIN_DESC scDesc;

		swapChain_->GetDesc(&scDesc);

		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(scDesc.BufferDesc.Width);
		viewport.Height = static_cast<float>(scDesc.BufferDesc.Height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		context_->RSSetViewports(1, &viewport);

		SwapChainDescription swapDesc;
		swapChain_->GetDesc(&swapDesc);
		projectionMatrix_ = XMMatrixPerspectiveFovLH(80.0f*3.14f / 180.0f, static_cast<float>(swapDesc.BufferDesc.Width / swapDesc.BufferDesc.Height), 1.0f, 1000.0f);
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

		D3D11_TEXTURE2D_DESC dsDesc;
		DXGI_SWAP_CHAIN_DESC scDesc;
		swapChain_->GetDesc(&scDesc);

		dsDesc.Width = scDesc.BufferDesc.Width;
		dsDesc.Height = scDesc.BufferDesc.Height;
		dsDesc.MipLevels = 1;
		dsDesc.ArraySize = 1;
		dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsDesc.SampleDesc.Count = 1;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.Usage = D3D11_USAGE_DEFAULT;
		dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsDesc.CPUAccessFlags = 0;
		dsDesc.MiscFlags = 0;

		result = device_->CreateTexture2D(&dsDesc, NULL, &depthStencilBuffer_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
		result = device_->CreateDepthStencilView(depthStencilBuffer_, NULL, &depthStencilView_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateSamplerState()
	{
		HRESULT result = S_OK;
		D3D11_SAMPLER_DESC sDesc;

		ZeroMemory(&sDesc, sizeof(D3D11_SAMPLER_DESC));
		sDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.MipLODBias = 0.0f;
		sDesc.MaxAnisotropy = 1;
		sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sDesc.BorderColor[0] = 0;
		sDesc.BorderColor[1] = 0;
		sDesc.BorderColor[2] = 0;
		sDesc.BorderColor[3] = 0;
		sDesc.MinLOD = 0;
		sDesc.MaxLOD = D3D11_FLOAT32_MAX;

		result = device_->CreateSamplerState(&sDesc, &samplerState_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		context_->PSSetSamplers(0, 1, &samplerState_);
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::CreateDefaultTexture()
	{
		HRESULT result = S_OK;
		uint32_t color = 0xffffffff;

		D3D11_TEXTURE2D_DESC whiteTexture;
		ZeroMemory(&whiteTexture, sizeof(D3D11_TEXTURE2D_DESC));
		whiteTexture.ArraySize = 1;
		whiteTexture.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		whiteTexture.CPUAccessFlags = 0;
		whiteTexture.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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

		result = device_->CreateTexture2D(&whiteTexture, &initData, &noTexture_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
		result = device_->CreateShaderResourceView(noTexture_, NULL, &defaultResource_);
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
		bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
		bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		result = device_->CreateBlendState(&bDesc, &blendState_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
	}
	
	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::StartDraw()
	{
		context_->ClearRenderTargetView(renderTargetView_, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
		context_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		context_->OMSetBlendState(NULL, 0, 0xffffffff);

		struct RenderElementSorter
		{
			inline bool operator() (RenderElement* a, RenderElement* b)
			{
				return (a->distanceToCamera() > b->distanceToCamera());
			}
		};

		std::sort(renderElements_.begin(), renderElements_.end(), RenderElementSorter());
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::Draw()
	{
		float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		context_->OMSetBlendState(blendState_, blendFactor, 0xffffffff);

		XMVECTOR camTranslation = camera_->translation();
		XMVECTOR translation;

		float distance = 0;

		for (auto& it : renderElements_)
		{
      RenderElement::ElementTypes elementType = it->element_type();
			translation = it->translation();
			XMVECTOR delta = camTranslation - translation;
			distance = abs(XMVectorGetX(delta)*XMVectorGetX(delta) + XMVectorGetZ(delta)*XMVectorGetZ(delta));

      if (elementType != RenderElement::ElementTypes::kTerrain)
      {
        it->setDistanceToCamera(distance);
      }
      else
      {
        it->setDistanceToCamera(1000000000);
      }

      VertexBufferType type = it->type();
      if (type != vbType_)
      {
        it->SetBuffers();
        vbType_ = type;
      }

			

			if (elementType == RenderElement::ElementTypes::kBillboard)
			{
				Billboard* ptr = static_cast<Billboard*>(it);
				if (camera_)
				{
					worldMatrix_ = ptr->WorldFromCamera(camera_);
				}	
			}
			else
			{
				worldMatrix_ = it->World();
			}
				
      D3D11_MAPPED_SUBRESOURCE cbData;
      VS_CONSTANT_BUFFER* mappedData;

      context_->Map(vsConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbData);

      mappedData = static_cast<VS_CONSTANT_BUFFER*>(cbData.pData);
      mappedData->Time = time_;
      mappedData->World = worldMatrix_;
      mappedData->View = viewMatrix_;
      mappedData->Projection = projectionMatrix_;
			mappedData->WorldViewProjection = worldMatrix_ * viewMatrix_ * projectionMatrix_;

      context_->Unmap(vsConstantBuffer_, 0);

			if (it->texture())
			{
				if (currentTexture_ != it->texture())
				{
					auto tex = it->texture()->resource();
					context_->PSSetShaderResources(0, 1, &tex);
					currentTexture_ = it->texture();
				}
			}
			else
			{
				context_->PSSetShaderResources(0, 1, &defaultResource_);
				currentTexture_ = nullptr;
			}

			if (it->shader())
			{
				if (currentShader_ != it->shader())
				{
					auto shaders = it->shader()->shaders();
					context_->PSSetShader(shaders.ps, 0, 0);
					context_->VSSetShader(shaders.vs, 0, 0);
					currentShader_ = it->shader();
				}
			}
			

			context_->DrawIndexed(static_cast<UINT>(it->indices().size()), 0, 0);
		}
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::UpdateCamera(Camera* camera)
	{
		worldMatrix_ = XMMatrixIdentity();
		viewMatrix_ = camera->view();
		camera_ = camera;
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::EndDraw()
	{
		swapChain_->Present(0, 0);
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::Destroy()
	{
		swapChain_->SetFullscreenState(FALSE, NULL);

		SNUFF_ASSERT_NOTNULL(device_);
		device_->Release();
		device_ = NULL;

		SNUFF_ASSERT_NOTNULL(context_);
		context_->Release();
		context_ = NULL;

		SNUFF_ASSERT_NOTNULL(swapChain_);
		swapChain_->Release();
		swapChain_ = NULL;

		for (auto it : displayAdapters_)
		{
			SNUFF_ASSERT_NOTNULL(it);
			it->Release();
			it = NULL;
		}

		SNUFF_ASSERT_NOTNULL(renderTargetView_);
		renderTargetView_->Release();
		renderTargetView_ = NULL;

		SNUFF_ASSERT_NOTNULL(backBuffer_);
		backBuffer_->Release();
		backBuffer_ = NULL;

		SNUFF_ASSERT_NOTNULL(inputLayout_);
		inputLayout_->Release();
		inputLayout_ = NULL;
		
		SNUFF_ASSERT_NOTNULL(vsBuffer_);
		vsBuffer_->Release();
		vsBuffer_ = NULL;

		SNUFF_ASSERT_NOTNULL(psBuffer_);
		psBuffer_->Release();
		psBuffer_ = NULL;

		SNUFF_ASSERT_NOTNULL(vsConstantBuffer_);
		vsConstantBuffer_->Release();
		vsConstantBuffer_ = NULL;

		SNUFF_ASSERT_NOTNULL(rasterizerState_);
		rasterizerState_->Release();
		rasterizerState_ = NULL;

		SNUFF_ASSERT_NOTNULL(depthStencilBuffer_);
		depthStencilBuffer_->Release();
		depthStencilBuffer_ = NULL;

		SNUFF_ASSERT_NOTNULL(depthStencilView_);
		depthStencilView_->Release();
		depthStencilView_ = NULL;

		SNUFF_ASSERT_NOTNULL(samplerState_);
		samplerState_->Release();
		samplerState_ = NULL;

		SNUFF_ASSERT_NOTNULL(noTexture_);
		noTexture_->Release();
		noTexture_ = NULL;

		SNUFF_ASSERT_NOTNULL(defaultResource_);
		defaultResource_->Release();
		defaultResource_ = NULL;

		SNUFF_ASSERT_NOTNULL(blendState_);
		blendState_->Release();
		blendState_ = NULL;

		SNUFF_LOG_INFO("Destroyed the D3D11 display device");
	}
}