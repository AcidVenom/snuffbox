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
		/**
		* @brief Construct through a path
		* @param[in] path (std::string) The path
		*/
		Texture(std::string path);

		Texture(ID3D11ShaderResourceView* texture);

		/// Default destructor
		~Texture();

		/// Destroy the texture object
		void Destroy();

		/**
		* @brief Reload through a path
		* @param[in] path (std::string) The path
		*/
		void Reload(std::string path);

		/**
		* @return (ID3D11ShaderResourceView*) The shader resource
		*/
		ID3D11ShaderResourceView* resource(){ return texture_; }

		int width(){ return width_; }

		int height(){ return height_; }

	private:
		ID3D11ShaderResourceView* texture_; //!< The actual texture
		int	width_; //!< The width of the texture
		int height_; //!< The height of the texture
	};
}