#include "../../snuffbox/d3d11/d3d11_shader.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"

namespace snuffbox
{
	//------------------------------------------------------------------------------------------
	Shader::Shader(std::string path)
	{
		std::string file_path = std::string(environment::game().path() + "/" + path).c_str();
		Shaders shaders = environment::render_device().LoadShader(path.c_str());

		if (shaders.vs == nullptr || shaders.ps == nullptr)
			SNUFF_LOG_ERROR(std::string("Errors while opening shader '" + path + "'").c_str());

		vs_ = shaders.vs;
		ps_ = shaders.ps;

		environment::file_watcher().AddFile(file_path, std::string(path), FileType::kShader);
	}

	//------------------------------------------------------------------------------------------
	Shader::~Shader()
	{
		Destroy();
	}

	//------------------------------------------------------------------------------------------
	void Shader::Reload(std::string path)
	{

		std::string file_path = std::string(environment::game().path() + "/" + path).c_str();
		Shaders shaders = environment::render_device().LoadShader(path.c_str());

		if (shaders.vs == nullptr || shaders.ps == nullptr)
		{
			SNUFF_LOG_ERROR(std::string("Errors while opening shader '" + path + "'").c_str());
			return;
		}

		vs_->Release();
		vs_ = NULL;

		ps_->Release();
		ps_ = NULL;

		vs_ = shaders.vs;
		ps_ = shaders.ps;
	}

	//------------------------------------------------------------------------------------------
	void Shader::Destroy()
	{
		SNUFF_ASSERT_NOTNULL(vs_);
		vs_->Release();
		vs_ = NULL;

		SNUFF_ASSERT_NOTNULL(ps_);
		ps_->Release();
		ps_ = NULL;
	}
}