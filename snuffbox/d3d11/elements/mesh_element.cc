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
		environment::render_device().SetVertexBuffer(vertexBuffer_);
		environment::render_device().SetIndexBuffer(nullptr);
	}
}