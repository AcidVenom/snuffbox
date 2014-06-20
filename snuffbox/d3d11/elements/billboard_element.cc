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
		vertexBuffer_->Release();
		indexBuffer_->Release();
	}

	//-------------------------------------------------------------------------------------------
	void Billboard::Create()
	{
		vertices().push_back({ 0.0f, 0.0f, 0.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) });
		vertices().push_back({ 1.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) });
		vertices().push_back({ 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) });
		vertices().push_back({ 1.0f, 0.0f, 0.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) });

		indices().push_back(2);
		indices().push_back(0);
		indices().push_back(1);
		indices().push_back(3);

		vertexBuffer_ = environment::render_device().CreateVertexBuffer(vertices());
		indexBuffer_ = environment::render_device().CreateIndexBuffer(indices());
	}

	//-------------------------------------------------------------------------------------------
	void Billboard::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertexBuffer_);
		environment::render_device().SetIndexBuffer(indexBuffer_);
	}

	//-------------------------------------------------------------------------------------------
	XMMATRIX& Billboard::World(Camera* camera)
	{
		XMMATRIX baseMatrix(XMMatrixTranspose(camera->view()));

		baseMatrix._14 = baseMatrix._24 = baseMatrix._34 = baseMatrix._41 = baseMatrix._42 = baseMatrix._43 = 0;
		baseMatrix._44 = 1;

		XMVECTOR trans = translation();
		float x = XMVectorGetX(trans);
		float y = XMVectorGetY(trans);
		float z = XMVectorGetZ(trans);

		XMVECTOR s = scale();
		float sx = XMVectorGetX(s);
		float sy = XMVectorGetY(s);
		float sz = XMVectorGetZ(s);

		world_ = scaling() * offset() * XMMatrixRotationX(-XM_PI / 2) * baseMatrix * XMMatrixTranslation(x*sx,y*sy,z*sz);

		return world_;
	}
}