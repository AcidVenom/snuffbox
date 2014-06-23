#include "../../snuffbox/fbx/fbx_model.h"
#include "../../snuffbox/environment.h"

namespace snuffbox
{
	//----------------------------------------------------------------------------------------
	FBXModel::FBXModel(const std::vector<Vertex>& verts)
	{
		vertices_ = environment::render_device().CreateVertexBuffer(verts);
	}

	//----------------------------------------------------------------------------------------
	FBXModel::~FBXModel()
	{
		vertices_->Release();
	}
}