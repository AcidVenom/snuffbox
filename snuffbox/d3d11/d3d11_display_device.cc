#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/d3d11/elements/render_element.h"
#include "../../snuffbox/d3d11/elements/quad_element.h"
#include "../../snuffbox/d3d11/d3d11_camera.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"
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
		camPos_(XMVectorSet(0.0f,0.0f,0.0f,0.0f))
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
		vs_ = nullptr;
		ps_ = nullptr;
		CreateDevice();
		GetAdapters();
		CreateBackBuffer();
		CreateViewport();
		LoadShader("shaders/test");
		CreateConstantBuffer();
		CreateLayout();
		CreateDepthStencil();
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
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		result = device_->CreateInputLayout(layout, 3, vsBuffer_->GetBufferPointer(),vsBuffer_->GetBufferSize(), &inputLayout_);
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
	void D3D11DisplayDevice::LoadShader(const char* path, bool reloading)
	{
		if (vs_)
		{
			vs_->Release();
			vs_ = nullptr;
		}
		if (ps_)
		{
			ps_->Release();
			ps_ = nullptr;
		}

		ID3D10Blob* errors = nullptr;
		HRESULT result = S_OK;
		std::string file_path = environment::game().path() + "/" + path + ".fx";
		
		result = D3DX11CompileFromFileA(file_path.c_str(), 0, 0, "VS", "vs_5_0", D3D10_SHADER_PACK_MATRIX_ROW_MAJOR, 0, 0, &vsBuffer_, &errors, 0);
		if (errors != nullptr)
		{
			SNUFF_LOG_ERROR(static_cast<const char*>(errors->GetBufferPointer()));
			return;
		}
		SNUFF_XASSERT(result == S_OK, HRToString(result, (std::string("Shader ") + file_path).c_str()).c_str());
		result = D3DX11CompileFromFileA(file_path.c_str(), 0, 0, "PS", "ps_5_0", D3D10_SHADER_PACK_MATRIX_ROW_MAJOR, 0, 0, &psBuffer_, &errors, 0);
		if (errors != nullptr)
		{
			SNUFF_LOG_ERROR(static_cast<const char*>(errors->GetBufferPointer()));
			return;
		}
		SNUFF_XASSERT(result == S_OK, HRToString(result, (std::string("Shader ") + file_path).c_str()).c_str());

		result = device_->CreateVertexShader(vsBuffer_->GetBufferPointer(), vsBuffer_->GetBufferSize(), NULL, &vs_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());
		result = device_->CreatePixelShader(psBuffer_->GetBufferPointer(), psBuffer_->GetBufferSize(), NULL, &ps_);
		SNUFF_XASSERT(result == S_OK, HRToString(result).c_str());

		context_->VSSetShader(vs_, 0, 0);
		context_->PSSetShader(ps_, 0, 0);

		if (!reloading)
		{
			environment::file_watcher().AddFile(file_path, std::string(path), FileType::kShader);
		}

		if (errors != nullptr)
		{
			errors->Release();
		}
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
	void D3D11DisplayDevice::StartDraw()
	{
		context_->ClearRenderTargetView(renderTargetView_, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
		context_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::Draw()
	{
		for (auto& it : renderElements_)
		{
      VertexBufferType type = it->type();
      if (type != vbType_)
      {
        it->SetBuffers();
        vbType_ = type;
      }
			worldMatrix_ = it->World();

      D3D11_MAPPED_SUBRESOURCE cbData;
      VS_CONSTANT_BUFFER* mappedData;

      context_->Map(vsConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbData);

      mappedData = static_cast<VS_CONSTANT_BUFFER*>(cbData.pData);
      mappedData->Time = time_;
      mappedData->World = worldMatrix_;
      mappedData->View = viewMatrix_;
      mappedData->Projection = projectionMatrix_;
			mappedData->WorldViewProjection = worldMatrix_ * viewMatrix_ * projectionMatrix_;
			mappedData->CamPos = camPos_;

      context_->Unmap(vsConstantBuffer_, 0);

			context_->DrawIndexed(static_cast<UINT>(it->indices().size()), 0, 0);
		}
	}

	//---------------------------------------------------------------------------------
	void D3D11DisplayDevice::UpdateCamera(Camera* camera)
	{
		worldMatrix_ = XMMatrixIdentity();
		viewMatrix_ = camera->view();
		camPos_ = camera->translation();
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

		SNUFF_ASSERT_NOTNULL(vs_);
		vs_->Release();
		vs_ = NULL;

		SNUFF_ASSERT_NOTNULL(ps_);
		ps_->Release();
		ps_ = NULL;

		SNUFF_ASSERT_NOTNULL(rasterizerState_);
		rasterizerState_->Release();
		rasterizerState_ = NULL;

		SNUFF_ASSERT_NOTNULL(depthStencilBuffer_);
		depthStencilBuffer_->Release();
		depthStencilBuffer_ = NULL;

		SNUFF_ASSERT_NOTNULL(depthStencilView_);
		depthStencilView_->Release();
		depthStencilView_ = NULL;

		SNUFF_LOG_INFO("Destroyed the D3D11 display device");
	}
}