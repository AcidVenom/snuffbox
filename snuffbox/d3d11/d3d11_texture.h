#pragma once

#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Texture
	*	@brief Texture class used to wrap D3D11 textures
	* @author Daniël Konings
	*/
	class Texture
	{
	public:
		/// Default constructor
		Texture(std::string path);
		/// Default destructor
		~Texture();

		/// Destroy the texture object
		void Destroy();

		/// Reloads the texture
		void Reload(std::string path);

		/// Returns the shader resource
		ID3D11ShaderResourceView* resource(){ return texture_; }

	private:
		ID3D11ShaderResourceView* texture_; //!< The actual texture
	};
}