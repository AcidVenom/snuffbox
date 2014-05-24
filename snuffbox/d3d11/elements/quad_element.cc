#include "../../../snuffbox/d3d11/elements/quad_element.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Quad::Quad()
	{

	}

	//-------------------------------------------------------------------------------------------
	Quad::Quad(JS_ARGS)
	{
		Create();
	}

	//-------------------------------------------------------------------------------------------
	Quad::~Quad()
	{
		vertexBuffer_->Release();
		indexBuffer_->Release();
	}
	
	//-------------------------------------------------------------------------------------------
	void Quad::Create()
	{
		vertices().push_back({ 0.0f, 0.0f, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f,1.0f) });
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
	void Quad::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertexBuffer_);
		environment::render_device().SetIndexBuffer(indexBuffer_);
	}
}