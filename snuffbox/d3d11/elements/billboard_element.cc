#include "../../../snuffbox/d3d11/elements/billboard_element.h"

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
		vertexBuffer_->Release();
		indexBuffer_->Release();
	}

	//-------------------------------------------------------------------------------------------
	void Billboard::Create()
	{
		vertices().push_back({ 0.0f, 0.0f, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) });
		vertices().push_back({ 1.0f, 0.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) });
		vertices().push_back({ 0.0f, 0.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) });
		vertices().push_back({ 1.0f, 0.0f, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) });

		indices().push_back(2);
		indices().push_back(0);
		indices().push_back(1);
		indices().push_back(3);

		vertexBuffer_ = environment::render_device().CreateVertexBuffer(vertices());
		indexBuffer_ = environment::render_device().CreateIndexBuffer(indices());

		environment::render_device().renderElements().push_back(this);
	}

	//-------------------------------------------------------------------------------------------
	void Billboard::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertexBuffer_);
		environment::render_device().SetIndexBuffer(indexBuffer_);
	}

	//-------------------------------------------------------------------------------------------
	XMMATRIX Billboard::WorldFromCamera(Camera* camera)
	{
		XMMATRIX baseMatrix(XMMatrixTranspose(camera->view()));

		baseMatrix._14 = baseMatrix._24 = baseMatrix._34 = baseMatrix._41 = baseMatrix._42 = baseMatrix._43 = 0;
		baseMatrix._44 = 1;

		XMMATRIX transformation = XMMatrixMultiply(offset() * XMMatrixRotationX(-XM_PI / 2) * baseMatrix, scaling() * XMMatrixTranslationFromVector(translation()));

		return transformation;
	}
}