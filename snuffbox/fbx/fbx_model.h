#pragma once

#include "../../snuffbox/d3d11/d3d11_display_device.h"

namespace snuffbox
{
	/**
	* @class snuffbox::FBXModel
	* @brief Holds information about an FBX model (materials, UVs, normals, vertices, indices)
	* @author Daniël Konings
	*/
	class FBXModel
	{
	public:
		/// Constructs an FBX model from a vector of vertices
		FBXModel(const std::vector<Vertex>& verts);

		/// Destroys the model itself and the buffers
		~FBXModel();

		/// Returns the vertex buffer
		ID3D11Buffer* vertices(){ return vertices_; }

	private:
		ID3D11Buffer* vertices_;
	};
}
