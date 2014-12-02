#include "../../../snuffbox/d3d11/elements/polygon_element.h"

namespace snuffbox
{
	//-----------------------------------------------------------------------------------------
	Polygon::Polygon() : 
		topology_(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP),
		RenderElement(ElementTypes::kPolygon)
	{
		Create();
		vertices().clear();
		indices().clear();
	}

	//-----------------------------------------------------------------------------------------
	Polygon::Polygon(JS_ARGS) :
		topology_(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP),
		RenderElement(ElementTypes::kPolygon)
	{
		Create();
		vertices().clear();
		indices().clear();
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::Create()
	{
		if (vertices().size() == 0)
		{
			vertices().push_back({ 0.0f, 0.0f, 0.0f, 0.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) });
			indices().push_back(0);
		}
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
		if (environment::render_settings().y_down() == true)
		{
			y = -y;
		}
		vertices().push_back({ x, y, z, 1.0f, XMFLOAT3(nx, ny, nz), XMFLOAT2(tx, ty), XMFLOAT4(r, g, b, a) });
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::RemoveVertex(int idx)
	{
		if (idx >= vertices().size() || idx < 0)
		{
			SNUFF_LOG_ERROR(("Index of vertex is out of bounds! Vertices size is " + std::to_string(vertices().size()) + ", index is " + std::to_string(idx)).c_str());
			return;
		}
		vertices().erase(vertices().begin() + idx);
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::AddIndex(int idx)
	{
		if (idx >= vertices().size())
		{
			SNUFF_LOG_WARNING("An index was added to the polygon with a greater size than the current vertex length");
		}

		if (idx < 0)
		{
			SNUFF_LOG_ERROR("Indices cannot be smaller than 0");
			return;
		}

		indices().push_back(idx);
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::RemoveIndex(int idx)
	{
		if (idx >= indices().size() || idx < 0)
		{
			SNUFF_LOG_ERROR(("Index of index is out of bounds! Indices size is " + std::to_string(indices().size()) + ", index is " + std::to_string(idx)).c_str());
			return;
		}
		indices().erase(indices().begin() + idx);
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::SetColour(int idx, float r, float g, float b, float a)
	{
		if (idx >= vertices().size() || idx < 0)
		{
			SNUFF_LOG_ERROR(("Index of vertex is out of bounds! Vertices size is " + std::to_string(vertices().size()) + ", index is " + std::to_string(idx)).c_str());
			return;
		}

		Vertex& vertex = vertices().at(idx);
		XMFLOAT4& colour = vertex.colour;
		
		colour.x = r;
		colour.y = g;
		colour.z = b;
		colour.w = a;
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::SetPosition(int idx, float x, float y, float z)
	{
		if (idx >= vertices().size() || idx < 0)
		{
			SNUFF_LOG_ERROR(("Index of vertex is out of bounds! Vertices size is " + std::to_string(vertices().size()) + ", index is " + std::to_string(idx)).c_str());
			return;
		}

		Vertex& vertex = vertices().at(idx);

		vertex.x = x;
		vertex.y = y;
		vertex.z = z;
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::SetNormal(int idx, float x, float y, float z)
	{
		if (idx >= vertices().size() || idx < 0)
		{
			SNUFF_LOG_ERROR(("Index of vertex is out of bounds! Vertices size is " + std::to_string(vertices().size()) + ", index is " + std::to_string(idx)).c_str());
			return;
		}

		Vertex& vertex = vertices().at(idx);
		XMFLOAT3& normal = vertex.normal;

		normal.x = x;
		normal.y = y;
		normal.z = z;
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::SetTexCoords(int idx, float x, float y)
	{
		if (idx >= vertices().size() || idx < 0)
		{
			SNUFF_LOG_ERROR(("Index of vertex is out of bounds! Vertices size is " + std::to_string(vertices().size()) + ", index is " + std::to_string(idx)).c_str());
			return;
		}

		Vertex& vertex = vertices().at(idx);
		XMFLOAT2& texCoords = vertex.tex_coord;

		texCoords.x = x;
		texCoords.y = y;
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::SetTopology(int type)
	{
		SNUFF_XASSERT(type == D3D_PRIMITIVE_TOPOLOGY_LINESTRIP ||
			type == D3D_PRIMITIVE_TOPOLOGY_LINELIST ||
			type == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST ||
			type == D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, "Unknown primitive topology for polygon was set");

		topology_ = static_cast<D3D11_PRIMITIVE_TOPOLOGY>(type);
	}

	//-----------------------------------------------------------------------------------------
	D3D11_PRIMITIVE_TOPOLOGY Polygon::topology()
	{
		return topology_;
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::Flush()
	{
		SNUFF_SAFE_RELEASE(vertex_buffer_);
		SNUFF_SAFE_RELEASE(index_buffer_);

		Create();
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::RegisterExtraFunctions(JS_EXTRA)
	{
		JSFunctionRegister funcs[] = {
			JSFunctionRegister("addVertex", JSAddVertex),
			JSFunctionRegister("removeVertex", JSRemoveVertex),
			JSFunctionRegister("addIndex", JSAddIndex),
			JSFunctionRegister("removeIndex", JSRemoveIndex),
			JSFunctionRegister("clear", JSClear),
			JSFunctionRegister("clearIndices", JSClearIndices),
			JSFunctionRegister("clearVertices", JSClearVertices),
			JSFunctionRegister("flush", JSFlush),
			JSFunctionRegister("setPosition", JSSetPosition),
			JSFunctionRegister("setColour", JSSetColour),
			JSFunctionRegister("setNormal", JSSetNormal),
			JSFunctionRegister("setTexCoords", JSSetTexCoords),
			JSFunctionRegister("setTopology", JSSetTopology)
		};

		JS_REGISTER_OBJECT_FUNCTIONS_EXTRA(obj, funcs);
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
	void Polygon::JSRemoveVertex(JS_ARGS)
	{
		JS_SETUP(Polygon, "N");

		self->RemoveVertex(wrapper.GetNumber<int>(0));
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSClearVertices(JS_ARGS)
	{
		JS_SETUP(Polygon, "V");
		self->vertices().clear();
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSClearIndices(JS_ARGS)
	{
		JS_SETUP(Polygon, "V");
		self->indices().clear();
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSClear(JS_ARGS)
	{
		JS_SETUP(Polygon, "V");
		self->vertices().clear();
		self->indices().clear();
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSAddIndex(JS_ARGS)
	{
		JS_SETUP(Polygon, "N");
		
		self->AddIndex(wrapper.GetNumber<int>(0));
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSRemoveIndex(JS_ARGS)
	{
		JS_SETUP(Polygon, "N");

		self->RemoveIndex(wrapper.GetNumber<int>(0));
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSFlush(JS_ARGS)
	{
		JS_SETUP(Polygon, "V");

		self->Flush();
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSSetPosition(JS_ARGS)
	{
		JS_SETUP(Polygon, "NNNN");

		int i = wrapper.GetNumber<int>(0);

		self->SetPosition(i, wrapper.GetNumber<float>(1), wrapper.GetNumber<float>(2), wrapper.GetNumber<float>(3));
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSSetColour(JS_ARGS)
	{
		JS_SETUP(Polygon, "NNNNN");

		int i = wrapper.GetNumber<int>(0);

		self->SetColour(i, wrapper.GetNumber<float>(1), wrapper.GetNumber<float>(2), wrapper.GetNumber<float>(3), wrapper.GetNumber<float>(4));
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSSetNormal(JS_ARGS)
	{
		JS_SETUP(Polygon, "NNNN");

		int i = wrapper.GetNumber<int>(0);

		self->SetNormal(i, wrapper.GetNumber<float>(1), wrapper.GetNumber<float>(2), wrapper.GetNumber<float>(3));
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSSetTexCoords(JS_ARGS)
	{
		JS_SETUP(Polygon, "NNN");

		int i = wrapper.GetNumber<int>(0);

		self->SetTexCoords(i, wrapper.GetNumber<float>(1), wrapper.GetNumber<float>(2));
	}

	//-----------------------------------------------------------------------------------------
	void Polygon::JSSetTopology(JS_ARGS)
	{
		JS_SETUP(Polygon, "N");

		self->SetTopology(wrapper.GetNumber<int>(0));
	}

	//-----------------------------------------------------------------------------------------
	Polygon::~Polygon()
	{
		SNUFF_SAFE_RELEASE(vertex_buffer_);
		SNUFF_SAFE_RELEASE(index_buffer_);
	}
}