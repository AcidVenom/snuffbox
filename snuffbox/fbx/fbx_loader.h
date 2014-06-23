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
		SharedPtr<FBXModel> Load(std::string path);

	private:
		FbxManager*			fbxManager_; ///< The FBX manager from the SDK
		FbxIOSettings*	ioSettings_; ///< The IO settings for the FBX manager to use
	};
}