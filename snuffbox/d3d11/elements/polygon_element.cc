#include "../../../snuffbox/d3d11/elements/polygon_element.h"

namespace snuffbox
{
	//-----------------------------------------------------------------------------------------
	Polygon::Polygon() : 
		topology_(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP),
		RenderElement(ElementTypes::kPolygon)
	{
		Create();
	}

	//-----------------------------------------------------------------------------------------
	Polygon::Polygon(JS_ARGS) :
		topology_(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP),
		RenderElement(ElementTypes::kPolygon)
	{
		Create();
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::Create()
	{
		vertices().clear();
		indices().clear();

		vertex_buffer_ = environment::render_device().CreateVertexBuffer(vertices());
		index_buffer_ = environment::render_device().CreateIndexBuffer(indices());
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertex_buffer_);
		environment::render_device().SetIndexBuffer(index_buffer_);
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::AddVertex(float x, float y, float z, float r, float g, float b, float a, float nx, float ny, float nz, float tx, float ty)
	{
		vertices().push_back({ x, y, z, 1.0f, XMFLOAT3(nx, ny, nz), XMFLOAT2(tx, ty), XMFLOAT4(r, g, b, a) });
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::RegisterExtraFunctions(JS_EXTRA)
	{

	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSAddVertex(JS_ARGS)
	{
		JS_SETUP(Polygon, "NNNNNNNNNNNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		float r = wrapper.GetNumber<float>(3);
		float g = wrapper.GetNumber<float>(4);
		float b = wrapper.GetNumber<float>(5);
		float a = wrapper.GetNumber<float>(6);

		float nx = wrapper.GetNumber<float>(7);
		float ny = wrapper.GetNumber<float>(8);
		float nz = wrapper.GetNumber<float>(9);

		float tx = wrapper.GetNumber<float>(10);
		float ty = wrapper.GetNumber<float>(11);

		self->AddVertex(x, y, z, r, g, b, a, nx, ny, nz, tx, ty);
	}

	//-----------------------------------------------------------------------------------------
	Polygon::~Polygon()
	{
		SNUFF_SAFE_RELEASE(vertex_buffer_);
		SNUFF_SAFE_RELEASE(index_buffer_);
	}
}