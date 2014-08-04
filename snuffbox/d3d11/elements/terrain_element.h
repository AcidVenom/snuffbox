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

		/// Sets the buffers of the terrain
		void SetBuffers();

		/**
		* @return (snuffbox::VertexBufferType) The vertex buffer type
		*/
    VertexBufferType type(){ return VertexBufferType::kTerrain; }

	private:
		unsigned int w_; //!< The height of the terrain
		unsigned int h_; //!< The width of the terrain
		ID3D11Buffer* vertex_buffer_; //!< The vertex buffer
		ID3D11Buffer* index_buffer_; //!< The index buffer

	public:
		JS_NAME(Terrain);
	};
}