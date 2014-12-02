#include "../../snuffbox/d3d11/d3d11_texture.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/win32/win32_file_watch.h"

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

		ID3D11Resource* res;
		texture_->GetResource(&res);

		ID3D11Texture2D* underlying = static_cast<ID3D11Texture2D*>(res);

		D3D11_TEXTURE2D_DESC texDesc;

		underlying->GetDesc(&texDesc);

		width_ = texDesc.Width;
		height_ = texDesc.Width;

		underlying->Release();

		environment::file_watcher().AddFile(file_path, path, FileType::kTexture);
	}

	//------------------------------------------------------------------------------------------
	Texture::Texture(ID3D11ShaderResourceView* texture) : texture_(texture)
	{

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
		
		ID3D11Resource* res;
		texture_->GetResource(&res);

		ID3D11Texture2D* underlying = static_cast<ID3D11Texture2D*>(res);

		D3D11_TEXTURE2D_DESC texDesc;

		underlying->GetDesc(&texDesc);

		underlying->Release();

		width_ = texDesc.Width;
		height_ = texDesc.Width;
	}
}