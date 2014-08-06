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

		/**
		* @brief Override the world function, to use our own billboard matrix
		* @param[in] camera (snuffbox::Camera*) The camera to use and to billboard to
		* @return (XMMATRIX&) The ouput world matrix
		*/
    XMMATRIX& world_matrix(Camera* camera);

		/**
		* @return (snuffbox::VertexBufferType) The vertex buffer type
		*/
		VertexBufferType type(){ return VertexBufferType::kQuad; }

	private:
		ID3D11Buffer* vertex_buffer_; //!< The vertex buffer
		ID3D11Buffer* index_buffer_; //!< The index buffer
		XMMATRIX world_; //!< The previous transformation
	public:
		JS_NAME(Billboard);
	};
}