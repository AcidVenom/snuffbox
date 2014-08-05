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

		/**
		* @brief Constructs an FBX model from a vector of vertices
		* @param[in] verts (const std::vector<Vertex>&) The list of vertices to create the model from
		* @param[in] relativePath (std::string) The relative working directory path for reloading
		*/
		FBXModel(const std::vector<Vertex>& verts, std::string relativePath);

		/// Destroys the model itself and the buffers
		~FBXModel();

		/**
		* @return (ID3D11Buffer*) The vertex buffer
		*/
		ID3D11Buffer* vertices(){ return vertices_; }

		/**
		* @return (unsigned int) The vertex count
		*/
		unsigned int vertex_count(){ return vertex_count_; }

		/**
		* @brief Reloads this model
		* @param[in] relativePath (std::string) The relative path from the working directory
		*/
		void Reload(std::string relativePath);

		/**
		* @return (bool) Should this model be reloaded?
		*/
		bool should_reload(){ return should_reload_; }

		/// Stop reloading
		void DisableReload(){ should_reload_ = false; }

	private:
		ID3D11Buffer* vertices_; //!< The vertex buffer
		unsigned int vertex_count_; //!< The vertex count
		bool should_reload_; //!< Should all meshes using this model reload?
	};
}
