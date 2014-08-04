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

		/// Loads a model from a given path, returning the list of vertices
		std::vector<Vertex> Load(std::string path);

		/// Loads an FBX scene from a file path
		void LoadScene(std::string path);

		/// Retrieves mesh data
		void GetMeshData(std::vector<Vertex>* vertsOut, FbxMesh* mesh);

	private:
		FbxManager*			fbxManager_; //!< The FBX manager from the SDK
		FbxScene*				fbxScene_; //!< The FBX scene to use
	};
}