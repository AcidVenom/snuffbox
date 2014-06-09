#pragma once

#include "../../../snuffbox/d3d11/elements/render_element.h"

namespace snuffbox
{
	class Camera;

	/**
	* @class snuffbox::Billboard
	* @brief A simple billboard that always looks towards the camera
	* @author Daniël Konings
	*/
	class Billboard : public RenderElement
	{
	public:
		/// Default constructor
		Billboard();
		/// JS constructor
		Billboard(JS_ARGS);
		/// Default destructor
		~Billboard();

		/// Creates the quad vertices
		void Create();

		/// Sets the buffers of the quad
		void SetBuffers();

		/// Override the world function, to use our own billboard matrix
		XMMATRIX WorldFromCamera(Camera* camera);

		/// Returns the vertex buffer type
		VertexBufferType type(){ return VertexBufferType::kQuad; }

	private:
		ID3D11Buffer* vertexBuffer_; ///< The vertex buffer
		ID3D11Buffer* indexBuffer_; ///< The index buffer
		XMMATRIX previous_; ///< The previous transformation
	public:
		JS_NAME(Billboard);
	};
}