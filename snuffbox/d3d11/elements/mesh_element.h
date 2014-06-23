#pragma once

#include "../../../snuffbox/d3d11/elements/render_element.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Mesh
	* @brief An FBX model loaded as a mesh
	* @author Dani�l Konings
	*/
	class Mesh : public RenderElement
	{
	public:
		/// Default constructor
		Mesh();
		/// JS constructor
		Mesh(JS_ARGS);
		/// Default destructor
		~Mesh();

		/// Retrieve the model vertices
		void Create();

		/// Sets the buffers of the mesh
		void SetBuffers();

		/// Returns the vertex buffer type
		VertexBufferType type(){ return VertexBufferType::kMesh; }

		/// Returns the 3D model
		FBXModel* model(){ return model_; }

	private:
		ID3D11Buffer* vertexBuffer_; ///< The vertex buffer
		FBXModel* model_;	 ///< The model to use
	public:
		JS_NAME(Mesh);
	};
}