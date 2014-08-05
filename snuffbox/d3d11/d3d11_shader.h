#pragma once

#include "../../snuffbox/d3d11/d3d11_display_device.h"

namespace snuffbox
{
	/**
	* @struct snuffbox::Shaders
	* @brief Contains both a vertex shader and a pixel shader
	* @author Daniël Konings
	*/
	struct Shaders
	{
		VertexShader* vs; //!< The vertex shader
		PixelShader* ps; //!< The pixel shader
	};

	/**
	* @class snuffbox::Shader
	* @brief A wrapped D3D11 shader
	* @author Daniël Konings
	*/
	class Shader
	{
	public:
		/**
		* @brief Construct through a path
		* @param[in] path (std::string) The path
		*/
		Shader(std::string path);

		/// Default destructor
		~Shader();

		/// Destroys the loaded shader
		void Destroy();

		/**
		* @brief Reloads the loaded shader
		* @param[in] path (std::string) The path to reload from
		*/
		void Reload(std::string path);

		/**
		* @return (snuffbox::Shaders) Both the vertex and the pixel shader in a structure
		*/
		Shaders shaders(){ return Shaders{ vs_, ps_ }; }
		
	private:
		VertexShader* vs_; //!< The vertex shader
		PixelShader* ps_; //!< The pixel shader
	};
}