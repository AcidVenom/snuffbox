#pragma once

#include "../../../snuffbox/d3d11/elements/render_element.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Quad
	* @brief A simple quad of 4 vertices
	* @author Daniël Konings
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

		/// Draws the quad
		void Draw();

	private:
		ID3D11Buffer* vertexBuffer_; ///< The vertex buffer
		ID3D11Buffer* indexBuffer_; ///< The index buffer
	public:
		JS_NAME(Quad);
	};
}