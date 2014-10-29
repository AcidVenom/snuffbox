#include "../../../snuffbox/d3d11/elements/billboard_element.h"
#include "../../../snuffbox/d3d11/d3d11_camera.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Billboard::Billboard() : RenderElement(RenderElement::ElementTypes::kBillboard)
	{

	}

	//-------------------------------------------------------------------------------------------
	Billboard::Billboard(JS_ARGS) : RenderElement(RenderElement::ElementTypes::kBillboard)
	{
		Create();
	}

	//-------------------------------------------------------------------------------------------
	Billboard::~Billboard()
	{
		vertex_buffer_->Release();
		index_buffer_->Release();
	}

	//-------------------------------------------------------------------------------------------
	void Billboard::Create()
	{
		vertices().push_back({ 0.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)});
		vertices().push_back({ 1.0f, 0.0f, 1.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)});
		vertices().push_back({ 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)});
		vertices().push_back({ 1.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)});


		indices().push_back(2);
		indices().push_back(0);
		indices().push_back(1);
		indices().push_back(3);

		vertex_buffer_ = environment::render_device().CreateVertexBuffer(vertices());
		index_buffer_ = environment::render_device().CreateIndexBuffer(indices());
	}

	//-------------------------------------------------------------------------------------------
	void Billboard::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertex_buffer_);
		environment::render_device().SetIndexBuffer(index_buffer_);
	}

	//-------------------------------------------------------------------------------------------
  XMMATRIX& Billboard::world_matrix(Camera* camera)
	{
		XMMATRIX baseMatrix(XMMatrixTranspose(camera->view_matrix()));

		baseMatrix._14 = baseMatrix._24 = baseMatrix._34 = baseMatrix._41 = baseMatrix._42 = baseMatrix._43 = 0;
		baseMatrix._44 = 1;

		XMMATRIX trans = XMMatrixTranslationFromVector(translation());

		if (environment::render_settings().y_down() == true)
		{
			trans._42 = -trans._42;
		}

		world_ = scaling() * offset() * XMMatrixRotationX(-XM_PI / 2) * baseMatrix * trans;

		return world_;
	}
}