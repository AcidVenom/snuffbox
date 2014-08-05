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
		fbx_manager_(nullptr),
		fbx_scene_(nullptr)
	{
		fbx_manager_ = FbxManager::Create();
		SNUFF_ASSERT_NOTNULL(fbx_manager_);
		SNUFF_LOG_INFO(std::string("FBX SDK version " + std::string(fbx_manager_->GetVersion())).c_str());

		FbxIOSettings* ioSettings = FbxIOSettings::Create(fbx_manager_, IOSROOT);
		fbx_manager_->SetIOSettings(ioSettings);

		fbx_scene_ = FbxScene::Create(fbx_manager_, "FBX_Scene");
		SNUFF_ASSERT_NOTNULL(fbx_scene_);

		environment::globalInstance = this;
	}

	//----------------------------------------------------------------------------------------
	std::vector<Vertex> FBXLoader::Load(std::string path)
	{
		SNUFF_ASSERT_NOTNULL(fbx_manager_);
		bool result = true;
		std::string full_path = environment::game().path() + "/" + path;
		LoadScene(full_path);
		
		SNUFF_ASSERT_NOTNULL(fbx_scene_);
		std::vector<Vertex> temp;

		FbxNode* node = fbx_scene_->GetRootNode()->GetChild(0);
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
		SNUFF_ASSERT_NOTNULL(fbx_manager_);
		int fileMajorVersion, fileMinorVersion, fileRevision;
		bool result = true;

		FbxImporter* fbxImporter = FbxImporter::Create(fbx_manager_, "");

		result = fbxImporter->Initialize(path.c_str(), -1, fbx_manager_->GetIOSettings());
		SNUFF_XASSERT(result == true, std::string("Failed importing " + path + " into the FBX manager!\n" + std::string(fbxImporter->GetStatus().GetErrorString())).c_str());
		fbxImporter->GetFileVersion(fileMajorVersion, fileMinorVersion, fileRevision);

		if (fbxImporter->IsFBX())
		{
			SNUFF_LOG_DEBUG(std::string("FBX file version " + std::to_string(fileMajorVersion) + "." + std::to_string(fileMinorVersion) + "." + std::to_string(fileRevision)).c_str());
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_MATERIAL, false);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_TEXTURE, true);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_LINK, false);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_SHAPE, true);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_GOBO, false);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_ANIMATION, false);
			fbx_manager_->GetIOSettings()->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		}

		result = fbxImporter->Import(fbx_scene_);
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

			FbxLayerElementArrayTemplate<FbxVector4>* binormals;
			mesh->GetBinormals(&binormals);

			FbxLayerElementArrayTemplate<FbxVector4>* tangents;
			mesh->GetTangents(&tangents);

			bool doBinormals = true;
			bool doTangents = true;

			if (!binormals)
				doBinormals = false;
			
			if (!tangents)
				doTangents = false;

			for (unsigned int polygon = 0; polygon < polyCount; ++polygon)
			{
				unsigned int polySize = mesh->GetPolygonSize(polygon);
				SNUFF_XASSERT(polySize == 3, "You should triangulate the mesh before using it (Is actually an export option in any Autodesk FBX exporter)");

				for (unsigned int vertex = 0; vertex < polySize; ++vertex)
				{

					unsigned int controlPoint = mesh->GetPolygonVertex(polygon, vertex);
					Vertex vert;
					FbxVector4 normal;
					FbxVector4 tangent = doTangents ? tangents->GetAt(controlPoint) : FbxVector4(0, 0, 0);
					FbxVector4 binormal = doBinormals ? binormals->GetAt(controlPoint) : FbxVector4(0, 0, 0);
					mesh->GetPolygonVertexNormal(polygon, vertex, normal);
					vert.x = vertices[controlPoint].mData[0];
					vert.z = -vertices[controlPoint].mData[1];
					vert.y = vertices[controlPoint].mData[2];
					vert.w = 1.0f;
					vert.normal.x = normal.mData[0];
					vert.normal.z = -normal.mData[1];
					vert.normal.y = normal.mData[2];
					vert.tangent.x = tangent.mData[0];
					vert.tangent.y = -tangent.mData[1];
					vert.tangent.z = tangent.mData[2];
					vert.binormal.x = binormal.mData[0];
					vert.binormal.z = -binormal.mData[1];
					vert.binormal.y = binormal.mData[2];
					vert.colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

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
						
						vertsOut->at(count).tex_coord.x = uv.mData[0];
						vertsOut->at(count).tex_coord.y = -uv.mData[1];

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

							vertsOut->at(count).tex_coord.x = uv.mData[0];
							vertsOut->at(count).tex_coord.y = -uv.mData[1];

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
		fbx_manager_->Destroy();
		environment::globalInstance = nullptr;
	}
}