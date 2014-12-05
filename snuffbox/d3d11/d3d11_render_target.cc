#include "../../snuffbox/d3d11/d3d11_render_target.h"
#include "../../snuffbox/d3d11/elements/render_element.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------------------
	RenderTarget::RenderTarget()
	{
		Create();
		environment::render_device().AddRenderTarget("Default", this);
		post_processing_ = environment::memory().ConstructShared<PostProcessing>();
	}

	//-------------------------------------------------------------------------------------------------------
	RenderTarget::RenderTarget(JS_ARGS)
	{
		JS_CHECK_PARAMS("S");

		if (!validParams)
		{
			return;
		}

		Create();

		environment::render_device().AddRenderTarget(wrapper.GetString(0), this);
		post_processing_ = environment::memory().ConstructShared<PostProcessing>();
	}

	//-------------------------------------------------------------------------------------------------------
	void RenderTarget::Create()
	{
		if (view_ != nullptr)
		{
			SNUFF_SAFE_RELEASE(view_);
			SNUFF_SAFE_RELEASE(target_);
			SNUFF_SAFE_RELEASE(resource_);
		}
		HRESULT result = S_OK;

		D3D11DisplayDevice& dev = environment::render_device();
		D3D11_TEXTURE2D_DESC desc;
		dev.back_buffer()->GetDesc(&desc);
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

		result = dev.device()->CreateTexture2D(&desc, NULL, &target_);

		SNUFF_XASSERT(result == S_OK, dev.HRToString(result, "Render Target"));

		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		viewDesc.Texture2D.MipLevels = 1;
		viewDesc.Texture2D.MostDetailedMip = 0;

		result = dev.device()->CreateShaderResourceView(target_, &viewDesc, &resource_);

		SNUFF_XASSERT(result == S_OK, dev.HRToString(result, "Render Target"));

		result = dev.device()->CreateRenderTargetView(target_, NULL,
			&view_);

		SNUFF_XASSERT(result == S_OK, dev.HRToString(result, "Render Target"));
	}

	//-------------------------------------------------------------------------------------------------------
	void RenderTarget::Clear()
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

		while (!queue_.empty())
		{
			queue_.pop();
		}
	}

	//-------------------------------------------------------------------------------------------------------
	void RenderTarget::RegisterJS(JS_TEMPLATE)
	{
		JSFunctionRegister funcs[] = {
			JSFunctionRegister("clear", JSClear),
			JSFunctionRegister("setShader", JSSetShader),
			JSFunctionRegister("addPass", JSAddPass),
			JSFunctionRegister("removePass", JSRemovePass),
			JSFunctionRegister("clearPasses", JSClearPasses),
			JSFunctionRegister("setUniform", JSSetUniform)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, true);
	}

	//---------------------------------------------------------------------------------
	void RenderTarget::JSSetShader(JS_ARGS)
	{
		JS_SETUP(RenderTarget, "S");

		if (!validParams)
		{
			return;
		}

		self->post_processing()->SetPostProcessingShader(wrapper.GetString(0));
	}

	//---------------------------------------------------------------------------------
	void RenderTarget::JSAddPass(JS_ARGS)
	{
		JS_SETUP(RenderTarget, "S");

		if (!validParams)
		{
			return;
		}

		self->post_processing()->AddPostProcessingPass(wrapper.GetString(0));
	}

	//---------------------------------------------------------------------------------
	void RenderTarget::JSRemovePass(JS_ARGS)
	{
		JS_SETUP(RenderTarget, "N");

		if (!validParams)
		{
			return;
		}

		self->post_processing()->RemovePostProcessingPass(wrapper.GetNumber<int>(0));
	}

	//---------------------------------------------------------------------------------
	void RenderTarget::JSClearPasses(JS_ARGS)
	{
		JS_SETUP(RenderTarget, "V");

		self->post_processing()->ClearPostProcessingPasses();
	}

	//---------------------------------------------------------------------------------
	void RenderTarget::JSSetUniform(JS_ARGS)
	{
		JS_SETUP(RenderTarget, "SSN");

		if (!validParams)
		{
			return;
		}

		float* value = 0;
		std::string type = wrapper.GetString(0);
		std::string name = wrapper.GetString(1);

		UniformType uniformType = RenderElement::TypeNameToUniformType(type);

		switch (uniformType)
		{
		case UniformType::kFloat:
			self->post_processing()->SetUniform(uniformType, name,
				wrapper.GetNumber<float>(2));
			break;
		case UniformType::kFloat2:
			self->post_processing()->SetUniform(uniformType, name,
				wrapper.GetNumber<float>(2),
				wrapper.GetNumber<float>(3));
			break;
		case UniformType::kFloat3:
			self->post_processing()->SetUniform(uniformType, name,
				wrapper.GetNumber<float>(2),
				wrapper.GetNumber<float>(3),
				wrapper.GetNumber<float>(4));
			break;
		case UniformType::kFloat4:
			self->post_processing()->SetUniform(uniformType, name,
				wrapper.GetNumber<float>(2),
				wrapper.GetNumber<float>(3),
				wrapper.GetNumber<float>(4),
				wrapper.GetNumber<float>(5));
			break;
		}
	}

	//-------------------------------------------------------------------------------------------------------
	void RenderTarget::JSClear(JS_ARGS)
	{
		JS_SETUP(RenderTarget, "V");

		self->Clear();
	}

	//-------------------------------------------------------------------------------------------------------
	RenderTarget::~RenderTarget()
	{
		SNUFF_SAFE_RELEASE(target_);
		SNUFF_SAFE_RELEASE(view_);
		SNUFF_SAFE_RELEASE(resource_);
	}
}