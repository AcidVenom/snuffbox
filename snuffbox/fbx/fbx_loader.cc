#include "../../snuffbox/fbx/fbx_loader.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"

namespace snuffbox
{
	namespace environment
	{
		namespace
		{
			FBXLoader* globalInstance = nullptr;
		}

		bool has_fbx_loader(){ return globalInstance != nullptr; }

		FBXLoader& fbx_loader()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}
}

namespace snuffbox
{
	//----------------------------------------------------------------------------------------
	FBXLoader::FBXLoader()
		: fbxManager_(FbxManager::Create())
	{
		ioSettings_ = FbxIOSettings::Create(fbxManager_, IOSROOT);
		fbxManager_->SetIOSettings(ioSettings_);
		environment::globalInstance = this;
	}

	//----------------------------------------------------------------------------------------
	SharedPtr<FBXModel> FBXLoader::Load(std::string path)
	{
		std::string full_path = environment::game().path() + "/" + path;
		SNUFF_ASSERT_NOTNULL(fbxManager_);

		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager_, "");
		FbxScene* fbxScene = FbxScene::Create(fbxManager_, "");

		bool success = fbxImporter->Initialize(full_path.c_str(), -1, fbxManager_->GetIOSettings());
		SNUFF_XASSERT(success == true, "Failed initialising the FBX loader importer!");

		success = fbxImporter->Import(fbxScene);
		SNUFF_XASSERT(success == true, "Failed importing the model into the FBX scene!");

		fbxImporter->Destroy();

		FbxNode* rootNode = fbxScene->GetRootNode();

		SNUFF_ASSERT_NOTNULL(rootNode);

		std::vector<Vertex> temp;

		for (int i = 0; i < rootNode->GetChildCount(); ++i)
		{
			FbxNode* childNode = rootNode->GetChild(i);

			if (childNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType AttributeType = childNode->GetNodeAttribute()->GetAttributeType();

			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* mesh = static_cast<FbxMesh*>(childNode->GetNodeAttribute());

			FbxVector4* vertices = mesh->GetControlPoints();

			for (int j = 0; j < mesh->GetPolygonCount(); j++)
			{
				unsigned int numVerts = mesh->GetPolygonSize(j);
				SNUFF_XASSERT(numVerts == 3,"As for currently, you cannot have polygons in your model with more than 3 vertices");

				for (unsigned int k = 0; k < numVerts; k++)
				{
					unsigned int controlPointIdx = mesh->GetPolygonVertex(j, k);

					Vertex vertex;
					vertex.x = static_cast<float>(vertices[controlPointIdx].mData[0]);
					vertex.z = static_cast<float>(vertices[controlPointIdx].mData[1]);
					vertex.y = static_cast<float>(vertices[controlPointIdx].mData[2]);
					temp.push_back(vertex);
				}
			}
		}

		fbxScene->Destroy();

		SNUFF_XASSERT(temp.size() > 0, "Cannot create a model from an empty list of vertices!");
		return environment::memory().ConstructShared<FBXModel>(temp);
	}

	//----------------------------------------------------------------------------------------
	FBXLoader::~FBXLoader()
	{
		fbxManager_->Destroy();
		environment::globalInstance = nullptr;
	}
}