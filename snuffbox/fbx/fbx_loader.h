#pragma once

#include <fbxsdk.h>
#include "../../snuffbox/memory/shared_ptr.h"
#include "../../snuffbox/fbx/fbx_model.h"

namespace snuffbox
{
	/**
	* @class snuffbox::FBXLoader
	* @brief A helper class to load FBX models using the Autodesk FBX SDK
	* @author Daniël Konings
	*/
	class FBXLoader
	{
	public:
		/// Default constructor
		FBXLoader();
		/// Default destructor
		~FBXLoader();

		/**
		* @brief Loads a model from a given path, returning the list of vertices
		* @param[in] path (std::string) The path to load from
		* @return (std::vector<Vertex>) The list of vertices of the model
		*/
		std::vector<Vertex> Load(std::string path);

		/**
		* @brief Loads an FBX scene from a file path
		* @param[in] path (std::string) The path to load from
		*/
		void LoadScene(std::string path);

		/**
		* @brief Retrieves all mesh data necessary
		* @param[out] vertsOut (std::vector<Vertex>*) The vertex vector to fill with vertex information
		* @param[in] mesh (FbxMesh*) The mesh to load the information from
		*/
		void GetMeshData(std::vector<Vertex>* vertsOut, FbxMesh* mesh);

	private:
		FbxManager*			fbx_manager_; //!< The FBX manager from the SDK
		FbxScene*				fbx_scene_; //!< The FBX scene to use
	};
}