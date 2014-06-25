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
		vertexBuffer_ = model_->vertices();
	}

	//-------------------------------------------------------------------------------------------
	void Mesh::SetBuffers()
	{
		if (vertexBuffer_ == model_->vertices() && model_->shouldReload())
		{
			model_->disableReload();
		}

		if (model_->shouldReload())
		{
			Create();
		}
		environment::render_device().SetVertexBuffer(vertexBuffer_);
		environment::render_device().SetIndexBuffer(nullptr);
	}
}