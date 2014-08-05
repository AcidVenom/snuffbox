#include "../../snuffbox/fbx/fbx_model.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/win32/win32_file_watch.h"
#include "../../snuffbox/fbx/fbx_loader.h"
#include "../../snuffbox/game.h"

namespace snuffbox
{
	//----------------------------------------------------------------------------------------
	FBXModel::FBXModel(const std::vector<Vertex>& verts, std::string relativePath) :
		should_reload_(false)
	{
		vertices_ = environment::render_device().CreateVertexBuffer(verts);
		vertex_count_ = static_cast<unsigned int>(verts.size());

		std::string path = environment::game().path() + "/" + relativePath;
		environment::file_watcher().AddFile(path, relativePath, FileType::kModel);
	}

	//----------------------------------------------------------------------------------------
	void FBXModel::Reload(std::string relativePath)
	{
		SNUFF_ASSERT_NOTNULL(vertices_);
		SNUFF_SAFE_RELEASE(vertices_);

		std::vector<Vertex> verts = environment::fbx_loader().Load(relativePath);
		vertices_ = environment::render_device().CreateVertexBuffer(verts);
		vertex_count_ = static_cast<unsigned int>(verts.size());

		should_reload_ = true;
	}

	//----------------------------------------------------------------------------------------
	FBXModel::~FBXModel()
	{
		SNUFF_SAFE_RELEASE(vertices_);
	}
}