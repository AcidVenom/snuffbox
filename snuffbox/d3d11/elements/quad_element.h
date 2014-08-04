#pragma once

#include "../../../snuffbox/d3d11/elements/render_element.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Quad
	* @brief A simple quad of 4 vertices
	* @author Dani�l Konings
	*/
	class Quad : public RenderElement
	{
	public:
		/// Default constructor
		Quad();
		/// JS constructor
		Quad(JS_ARGS);
		/// Default destructor
		~Quad();

		/// Creates the quad vertices
		void Create();

		/// Sets the buffers of the quad
		void SetBuffers();

		/**
		* @return (snuffbox::VertexBufferType) The vertex buffer type
		*/
    VertexBufferType type(){ return VertexBufferType::kQuad; }

	private:
		ID3D11Buffer* vertex_buffer_; //!< The vertex buffer
		ID3D11Buffer* index_buffer_; //!< The index buffer
	public:
		JS_NAME(Quad);
	};
}