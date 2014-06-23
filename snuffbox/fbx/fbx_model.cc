#include "../../snuffbox/fbx/fbx_model.h"
#include "../../snuffbox/environment.h"

namespace snuffbox
{
	//----------------------------------------------------------------------------------------
	FBXModel::FBXModel(const std::vector<Vertex>& verts)
	{
		vertices_ = environment::render_device().CreateVertexBuffer(verts);
		vertexCount_ = static_cast<unsigned int>(verts.size());
	}

	//----------------------------------------------------------------------------------------
	FBXModel::~FBXModel()
	{
		vertices_->Release();
	}
}