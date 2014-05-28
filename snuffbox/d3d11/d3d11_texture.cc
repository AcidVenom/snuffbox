#include "../../snuffbox/d3d11/d3d11_texture.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"

namespace snuffbox
{
	//------------------------------------------------------------------------------------------
	Texture::Texture(std::string path) : texture_(nullptr)
	{
		std::string file_path = std::string(environment::game().path() + "/" + path).c_str();

		HRESULT result = S_OK;
		result = D3DX11CreateShaderResourceViewFromFileA(
			environment::render_device().device(),
			file_path.c_str(),
			NULL,
			NULL,
			&texture_,
			NULL
			);

		SNUFF_XASSERT(result == S_OK, environment::render_device().HRToString(result, std::string("Textures " + file_path).c_str()).c_str());
	
		environment::file_watcher().AddFile(file_path, path, FileType::kTexture);
	}

	//------------------------------------------------------------------------------------------
	Texture::~Texture()
	{
		Destroy();
	}

	//------------------------------------------------------------------------------------------
	void Texture::Destroy()
	{
		SNUFF_ASSERT_NOTNULL(texture_);
		texture_->Release();
		texture_ = NULL;
	}

	//------------------------------------------------------------------------------------------
	void Texture::Reload(std::string path)
	{
		SNUFF_ASSERT_NOTNULL(texture_);
		texture_->Release();
		texture_ = NULL;

		std::string file_path = std::string(environment::game().path() + "/" + path).c_str();

		HRESULT result = S_OK;
		result = D3DX11CreateShaderResourceViewFromFileA(
			environment::render_device().device(),
			file_path.c_str(),
			NULL,
			NULL,
			&texture_,
			NULL
			);

		SNUFF_XASSERT(result == S_OK, environment::render_device().HRToString(result, std::string("Textures " + file_path).c_str()).c_str());
	}
}