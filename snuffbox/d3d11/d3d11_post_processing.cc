#include "../../snuffbox/d3d11/d3d11_post_processing.h"
#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/js/js_wrapper.h"
#include "../../snuffbox/content/content_manager.h"

namespace snuffbox
{
	namespace environment
	{
		namespace
		{
			PostProcessing* globalInstance = nullptr;
		}

		PostProcessing& post_processing()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}

		bool has_post_processing()
		{
			return globalInstance != nullptr;
		}
	}
}

namespace snuffbox
{
	//---------------------------------------------------------------------------------
	PostProcessing::PostProcessing()
	{
		environment::globalInstance = this;
		shader_ = environment::content_manager().Get<Shader>("shaders/post_processing.fx").get();
	}

	//---------------------------------------------------------------------------------
	void PostProcessing::SetPostProcessingShader(std::string path)
	{
		shader_ = environment::content_manager().Get<Shader>(path).get();
	}

	//---------------------------------------------------------------------------------
	void PostProcessing::AddPostProcessingPass(std::string path)
	{
		passes_.push_back(environment::content_manager().Get<Shader>(path).get());
	}

	//---------------------------------------------------------------------------------
	void PostProcessing::RemovePostProcessingPass(int idx)
	{
		SNUFF_XASSERT(idx >= 0 && idx < passes_.size(),
			std::string("Post processing pass to remove is out of range! Index: " +
			std::to_string(idx) + ", size: " + std::to_string(passes_.size())).c_str());

		passes_.erase(passes_.begin() + idx);
	}

	//---------------------------------------------------------------------------------
	void PostProcessing::ClearPostProcessingPasses()
	{
		passes_.clear();
	}

	//---------------------------------------------------------------------------------
	Shader* PostProcessing::shader()
	{
		return shader_;
	}

	//---------------------------------------------------------------------------------
	std::vector<Shader*>& PostProcessing::passes()
	{
		return passes_;
	}

	//---------------------------------------------------------------------------------
	void PostProcessing::JSSetShader(JS_ARGS)
	{
		JS_CHECK_PARAMS("S");

		if (!validParams)
		{
			return;
		}

		environment::post_processing().SetPostProcessingShader(wrapper.GetString(0));
	}

	//---------------------------------------------------------------------------------
	void PostProcessing::JSAddPass(JS_ARGS)
	{
		JS_CHECK_PARAMS("S");

		if (!validParams)
		{
			return;
		}

		environment::post_processing().AddPostProcessingPass(wrapper.GetString(0));
	}

	//---------------------------------------------------------------------------------
	void PostProcessing::JSRemovePass(JS_ARGS)
	{
		JS_CHECK_PARAMS("N");

		if (!validParams)
		{
			return;
		}

		environment::post_processing().RemovePostProcessingPass(wrapper.GetNumber<int>(0));
	}

	//---------------------------------------------------------------------------------
	void PostProcessing::JSClearPasses(JS_ARGS)
	{
		environment::post_processing().ClearPostProcessingPasses();
	}

	//---------------------------------------------------------------------------------
	void PostProcessing::RegisterJS(JS_TEMPLATE)
	{
		JSFunctionRegister funcs[] = {
			JSFunctionRegister("setShader", JSSetShader),
			JSFunctionRegister("addPass", JSAddPass),
			JSFunctionRegister("removePass", JSRemovePass),
			JSFunctionRegister("clearPasses", JSClearPasses)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
	}

	//---------------------------------------------------------------------------------
	PostProcessing::~PostProcessing()
	{
		environment::globalInstance = nullptr;
	}
}