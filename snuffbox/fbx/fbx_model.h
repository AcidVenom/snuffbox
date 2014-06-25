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
		FBXModel(const std::vector<Vertex>& verts, std::string relativePath);

		/// Destroys the model itself and the buffers
		~FBXModel();

		/// Returns the vertex buffer
		ID3D11Buffer* vertices(){ return vertices_; }

		/// Returns the vertex count
		unsigned int vertexCount(){ return vertexCount_; }

		/// Reloads this model
		void Reload(std::string relativePath);

		/// Should all meshes using this model reload?
		bool shouldReload(){ return shouldReload_; }

		/// Stop reloading
		void disableReload(){ shouldReload_ = false; }

	private:
		ID3D11Buffer* vertices_;
		unsigned int vertexCount_;
		bool shouldReload_;
	};
}
