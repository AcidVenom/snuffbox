#include "../../../snuffbox/d3d11/elements/mesh_element.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Mesh::Mesh() : RenderElement(RenderElement::ElementTypes::kMesh)
	{

	}

	//-------------------------------------------------------------------------------------------
	Mesh::Mesh(JS_ARGS) : RenderElement(RenderElement::ElementTypes::kMesh)
	{
		JS_CHECK_PARAMS("S");
		model_ = environment::content_manager().Get<FBXModel>(wrapper.GetString(0)).get();
		Create();
	}

	//-------------------------------------------------------------------------------------------
	Mesh::~Mesh()
	{
		
	}

	//-------------------------------------------------------------------------------------------
	void Mesh::Create()
	{
		vertex_buffer_ = model_->vertices();
	}

	//-------------------------------------------------------------------------------------------
	void Mesh::SetBuffers()
	{
		if (vertex_buffer_ == model_->vertices() && model_->should_reload())
		{
			model_->DisableReload();
		}

		if (model_->should_reload())
		{
			Create();
		}
		environment::render_device().SetVertexBuffer(vertex_buffer_);
		environment::render_device().SetIndexBuffer(nullptr);
	}
}