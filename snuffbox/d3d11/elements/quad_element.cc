#include "../../../snuffbox/d3d11/elements/quad_element.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Quad::Quad() : RenderElement(RenderElement::ElementTypes::kQuad)
	{

	}

	//-------------------------------------------------------------------------------------------
	Quad::Quad(JS_ARGS) : RenderElement(RenderElement::ElementTypes::kQuad)
	{
		Create();
	}

	//-------------------------------------------------------------------------------------------
	Quad::~Quad()
	{
		vertex_buffer_->Release();
		index_buffer_->Release();
	}
	
	//-------------------------------------------------------------------------------------------
	void Quad::Create()
	{
		vertices().push_back({ 0.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
		vertices().push_back({ 1.0f, 0.0f, 1.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
		vertices().push_back({ 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
		vertices().push_back({ 1.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });

		indices().push_back(2);
		indices().push_back(0);
		indices().push_back(1);
    indices().push_back(3);

		vertex_buffer_ = environment::render_device().CreateVertexBuffer(vertices());
		index_buffer_ = environment::render_device().CreateIndexBuffer(indices());
	}

	//-------------------------------------------------------------------------------------------
	void Quad::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertex_buffer_);
		environment::render_device().SetIndexBuffer(index_buffer_);
	}

	//-------------------------------------------------------------------------------------------
	Quad2D::Quad2D(JS_ARGS) : RenderElement(RenderElement::ElementTypes::kQuad2D)
	{
		Create();
	}

	//-------------------------------------------------------------------------------------------
	void Quad2D::Create()
	{
		vertices().push_back({ 0.0f, -1.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
		vertices().push_back({ 0.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
		vertices().push_back({ 1.0f, -1.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
		vertices().push_back({ 1.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });

		indices().push_back(1);
		indices().push_back(0);
		indices().push_back(3);
		indices().push_back(2);

		vertex_buffer_ = environment::render_device().CreateVertexBuffer(vertices());
		index_buffer_ = environment::render_device().CreateIndexBuffer(indices());
	}

	//-------------------------------------------------------------------------------------------
	void Quad2D::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertex_buffer_);
		environment::render_device().SetIndexBuffer(index_buffer_);
	}

	//-------------------------------------------------------------------------------------------
	XMMATRIX& Quad2D::world_matrix(Camera* camera)
	{
    world_matrix_ = scaling_2d() *
			offset_2d() *
			rotation() *
			XMMatrixTranslationFromVector(translation());

    return world_matrix_;
	}

	//-------------------------------------------------------------------------------------------
	Quad2D::~Quad2D()
	{
		SNUFF_SAFE_RELEASE(vertex_buffer_);
		SNUFF_SAFE_RELEASE(index_buffer_);
	}
}