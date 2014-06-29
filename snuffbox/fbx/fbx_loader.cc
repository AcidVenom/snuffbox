#include "../../snuffbox/fbx/fbx_loader.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"

using namespace fbxsdk_2015_1;

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
	FBXLoader::FBXLoader() : 
		fbxManager_(nullptr),
		fbxScene_(nullptr)
	{
		fbxManager_ = FbxManager::Create();
		SNUFF_ASSERT_NOTNULL(fbxManager_);
		SNUFF_LOG_INFO(std::string("FBX SDK version " + std::string(fbxManager_->GetVersion())).c_str());

		FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager_, IOSROOT);
		fbxManager_->SetIOSettings(ioSettings);

		fbxScene_ = FbxScene::Create(fbxManager_, "FBX_Scene");
		SNUFF_ASSERT_NOTNULL(fbxScene_);

		environment::globalInstance = this;
	}

	//----------------------------------------------------------------------------------------
	std::vector<Vertex> FBXLoader::Load(std::string path)
	{
		SNUFF_ASSERT_NOTNULL(fbxManager_);
		bool result = true;
		std::string full_path = environment::game().path() + "/" + path;
		LoadScene(full_path);
		
		SNUFF_ASSERT_NOTNULL(fbxScene_);
		std::vector<Vertex> temp;

		FbxNode* node = fbxScene_->GetRootNode()->GetChild(0);
		if (node)
		{
			FbxMesh* mesh = node->GetMesh();
			if (mesh)
			{
				GetMeshData(&temp,mesh);
			}
		}

		return temp;
	}

	//----------------------------------------------------------------------------------------
	void FBXLoader::LoadScene(std::string path)
	{
		SNUFF_ASSERT_NOTNULL(fbxManager_);
		int fileMajorVersion, fileMinorVersion, fileRevision;
		bool result = true;

		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager_, "");

		result = fbxImporter->Initialize(path.c_str(), -1, fbxManager_->GetIOSettings());
		SNUFF_XASSERT(result == true, std::string("Failed importing " + path + " into the FBX manager!\n" + std::string(fbxImporter->GetStatus().GetErrorString())).c_str());
		fbxImporter->GetFileVersion(fileMajorVersion, fileMinorVersion, fileRevision);

		if (fbxImporter->IsFBX())
		{
			SNUFF_LOG_DEBUG(std::string("FBX file version " + std::to_string(fileMajorVersion) + "." + std::to_string(fileMinorVersion) + "." + std::to_string(fileRevision)).c_str());
			fbxManager_->GetIOSettings()->SetBoolProp(IMP_FBX_MATERIAL, false);
			fbxManager_->GetIOSettings()->SetBoolProp(IMP_FBX_TEXTURE, true);
			fbxManager_->GetIOSettings()->SetBoolProp(IMP_FBX_LINK, false);
			fbxManager_->GetIOSettings()->SetBoolProp(IMP_FBX_SHAPE, true);
			fbxManager_->GetIOSettings()->SetBoolProp(IMP_FBX_GOBO, false);
			fbxManager_->GetIOSettings()->SetBoolProp(IMP_FBX_ANIMATION, false);
			fbxManager_->GetIOSettings()->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		}

		result = fbxImporter->Import(fbxScene_);
		SNUFF_XASSERT(result == true, "Failed importing the file to the FBX scene!");

		fbxImporter->Destroy();
	}

	//----------------------------------------------------------------------------------------
	void FBXLoader::GetMeshData(std::vector<Vertex>* vertsOut, FbxMesh* mesh)
	{
		FbxStringList uvNames;
		mesh->GetUVSetNames(uvNames);

		for (unsigned int i = 0; i < static_cast<unsigned int>(uvNames.GetCount()); ++i)
		{
			const char* uvName = uvNames.GetStringAt(i);
			const FbxGeometryElementUV* uvElement = mesh->GetElementUV(uvName);

			if (!uvElement)
				continue;

			SNUFF_XASSERT(uvElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex || uvElement->GetMappingMode() == FbxGeometryElement::eByControlPoint, "Invalid UV mapping mode!");

			bool useIndex = uvElement->GetReferenceMode() != FbxGeometryElement::eDirect;
			unsigned int indexCount = useIndex ? uvElement->GetIndexArray().GetCount() : 0;

			unsigned int polyCount = mesh->GetPolygonCount();
			FbxVector4* vertices = mesh->GetControlPoints();

			for (unsigned int polygon = 0; polygon < polyCount; ++polygon)
			{
				unsigned int polySize = mesh->GetPolygonSize(polygon);
				SNUFF_XASSERT(polySize == 3, "You should triangulate the mesh before using it");

				for (unsigned int vertex = 0; vertex < polySize; ++vertex)
				{
					unsigned int controlPoint = mesh->GetPolygonVertex(polygon, vertex);
					Vertex vert;
					FbxVector4 normal;
					mesh->GetPolygonVertexNormal(polygon, vertex, normal);
					vert.x = vertices[controlPoint].mData[0];
					vert.z = vertices[controlPoint].mData[1];
					vert.y = vertices[controlPoint].mData[2];
					vert.w = 1.0f;
					vert.normal.x = normal.mData[0];
					vert.normal.z = normal.mData[1];
					vert.normal.y = normal.mData[2];
					vert.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

					vertsOut->push_back(vert);
				}
			}

			unsigned int count = 0;

			if (uvElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				for (unsigned int polygon = 0; polygon < polyCount; ++polygon)
				{
					unsigned int polySize = mesh->GetPolygonSize(polygon);
					SNUFF_XASSERT(polySize == 3, "You should triangulate the mesh before using it");

					for (unsigned int vertex = 0; vertex < polySize; ++vertex)
					{
						unsigned int controlPoint = mesh->GetPolygonVertex(polygon, vertex);
						unsigned int uvIndex = useIndex ? uvElement->GetIndexArray().GetAt(controlPoint) : controlPoint;

						FbxVector2 uv = uvElement->GetDirectArray().GetAt(uvIndex);
						
						vertsOut->at(count).texCoord.x = uv.mData[0];
						vertsOut->at(count).texCoord.y = -uv.mData[1];

						++count;
					}
				}
			}
			else if (uvElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				unsigned int polyCounter = 0;
				for (unsigned int polygon = 0; polygon < polyCount; ++polygon)
				{
					unsigned int polySize = mesh->GetPolygonSize(polygon);
					SNUFF_XASSERT(polySize == 3, "You should triangulate the mesh before using it");

					for (unsigned int vertex = 0; vertex < polySize; ++vertex)
					{
						if (polyCounter < indexCount)
						{
							unsigned int uvIndex = useIndex ? uvElement->GetIndexArray().GetAt(polyCounter) : polyCounter;

							FbxVector2 uv = uvElement->GetDirectArray().GetAt(uvIndex);

							vertsOut->at(count).texCoord.x = uv.mData[0];
							vertsOut->at(count).texCoord.y = -uv.mData[1];

							++count;
							++polyCounter;
						}
					}
				}
			}
		}

		SNUFF_LOG_DEBUG(std::string("Vertices: " + std::to_string(vertsOut->size())).c_str());
	}

	//----------------------------------------------------------------------------------------
	FBXLoader::~FBXLoader()
	{
		fbxManager_->Destroy();
		environment::globalInstance = nullptr;
	}
}