#include "../../snuffbox/d3d11/d3d11_post_processing.h"
#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/js/js_wrapper.h"
#include "../../snuffbox/content/content_manager.h"
#include "../../snuffbox/d3d11/elements/render_element.h"

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

  //-------------------------------------------------------------------------------------------
  void PostProcessing::SetUniform(UniformType type, std::string name, float _1, float _2, float _3, float _4)
  {
    ShaderUniform uniform;

    uniform.name = name;
    uniform.type = type;
    uniform.value[0] = _1;
    uniform.value[1] = _2;
    uniform.value[2] = _3;
    uniform.value[3] = _4;

    uniforms_.emplace(name, uniform);

    auto& it = uniforms_.find(name);
    if (it != uniforms_.end())
    {
      it->second = uniform;
    }
    else
    {
      uniforms_.emplace(name, uniform);
    }
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
  std::vector<float> PostProcessing::uniforms()
  {
    std::vector<float> temp;
    for (std::map<std::string, ShaderUniform>::iterator it = uniforms_.begin(); it != uniforms_.end(); ++it) {
      switch (it->second.type)
      {
      case UniformType::kFloat:
        temp.push_back(it->second.value[0]);
        break;
      case UniformType::kFloat2:
        temp.push_back(it->second.value[0]);
        temp.push_back(it->second.value[1]);
        break;
      case UniformType::kFloat3:
        temp.push_back(it->second.value[0]);
        temp.push_back(it->second.value[1]);
        temp.push_back(it->second.value[2]);
        break;
      case UniformType::kFloat4:
        temp.push_back(it->second.value[0]);
        temp.push_back(it->second.value[1]);
        temp.push_back(it->second.value[2]);
        temp.push_back(it->second.value[3]);
        break;
      }

    }
    return temp;
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
  void PostProcessing::JSSetUniform(JS_ARGS)
  {
    JS_CHECK_PARAMS("SSN");

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
      environment::post_processing().SetUniform(uniformType, name,
        wrapper.GetNumber<float>(2));
      break;
    case UniformType::kFloat2:
      environment::post_processing().SetUniform(uniformType, name,
        wrapper.GetNumber<float>(2),
        wrapper.GetNumber<float>(3));
      break;
    case UniformType::kFloat3:
      environment::post_processing().SetUniform(uniformType, name,
        wrapper.GetNumber<float>(2),
        wrapper.GetNumber<float>(3),
        wrapper.GetNumber<float>(4));
      break;
    case UniformType::kFloat4:
      environment::post_processing().SetUniform(uniformType, name,
        wrapper.GetNumber<float>(2),
        wrapper.GetNumber<float>(3),
        wrapper.GetNumber<float>(4),
        wrapper.GetNumber<float>(5));
      break;
    }
  }

	//---------------------------------------------------------------------------------
	void PostProcessing::RegisterJS(JS_TEMPLATE)
	{
		JSFunctionRegister funcs[] = {
			JSFunctionRegister("setShader", JSSetShader),
			JSFunctionRegister("addPass", JSAddPass),
			JSFunctionRegister("removePass", JSRemovePass),
			JSFunctionRegister("clearPasses", JSClearPasses),
      JSFunctionRegister("setUniform", JSSetUniform)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
	}

	//---------------------------------------------------------------------------------
	PostProcessing::~PostProcessing()
	{
		environment::globalInstance = nullptr;
	}
}