#pragma once

#include "../../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../../snuffbox/d3d11/elements/render_element.h"
#include "../../../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Terrain
	* @brief Creates a terrain of triangles to work with
	* @author Daniël Konings
	*/
	class Terrain : public RenderElement
	{
	public:
		/// Default constructor
		Terrain();
		/// JS constructor
		Terrain(JS_ARGS);
		/// Default destructor
		~Terrain();

		/// Creates the terrain
		void Create();

		/// Draws the terrain
		void Draw();

	private:
		unsigned int w_, h_; ///< The width and the height of the terrain
		ID3D11Buffer* vertexBuffer_; ///< The vertex buffer
		ID3D11Buffer* indexBuffer_; ///< The index buffer

	public:
		JS_NAME(Terrain);
	};
}