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
		VertexShader* vs;
		PixelShader* ps;
	};

	/**
	* @class snuffbox::Shader
	* @brief A wrapped D3D11 shader
	* @author Daniël Konings
	*/
	class Shader
	{
	public:
		/// Default constructor with a path
		Shader(std::string path);

		/// Default destructor
		~Shader();

		/// Destroys the loaded shader
		void Destroy();

		/// Reloads the loaded shader
		void Reload(std::string path);

		/// Returns both shaders
		Shaders shaders(){ return Shaders{ vs_, ps_ }; }
		
	private:
		VertexShader* vs_;
		PixelShader* ps_;
	};
}