#include "../../../snuffbox/d3d11/elements/terrain_element.h"

namespace snuffbox
{
	//------------------------------------------------------------------------------
	Terrain::Terrain() : RenderElement(RenderElement::ElementTypes::kTerrain)
	{

	}

	//------------------------------------------------------------------------------
	Terrain::Terrain(JS_ARGS) : RenderElement(RenderElement::ElementTypes::kTerrain)
	{
		JS_CHECK_PARAMS("NN");

		w_ = wrapper.GetNumber<unsigned int>(0);
		h_ = wrapper.GetNumber<unsigned int>(1);

		Create();
	}

	//------------------------------------------------------------------------------
	void Terrain::Create()
	{
		const double yScale = std::sqrt(0.75f);

		HRESULT result = S_OK;
		for (unsigned int y = 0; y < h_; y++)
		{
			for (unsigned int x = 0; x < w_; x++)
			{
				unsigned int i = y * w_ + x;
				Vertex vertex;

				vertex.x = static_cast<float>(x);
				vertex.y = 0.0f;
				vertex.z = static_cast<float>(y*yScale);
				vertex.w = 1.0f;
				vertex.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				vertex.tex_coord = XMFLOAT2(static_cast<float>(x) / w_, 1.0f-(static_cast<float>(y) / h_));
				vertex.colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				vertices().push_back(vertex);
			}
		}
		for (unsigned int y = 0; y < h_-1; ++y)
		{
			if (y % 2 == 0)
			{
				for (unsigned int x = 0; x < w_; ++x)
				{

					indices().push_back(y*w_ + x);
					indices().push_back((y + 1)* w_ + x);

					if (x == w_ - 1)
					{
						indices().push_back((y+1)*w_ + x);
					}
				}
			}
			else
			{
				for (int x = w_ - 1; x >= 0; --x)
				{
					indices().push_back(y*w_ + x);
					indices().push_back((y + 1)* w_ + x);

					if (x == 0)
					{
						indices().push_back((y + 1)*w_ + x);
					}
				}
			}
			
		}

		vertex_buffer_ = environment::render_device().CreateVertexBuffer(vertices());
		index_buffer_ = environment::render_device().CreateIndexBuffer(indices());
	}

	//------------------------------------------------------------------------------
	void Terrain::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertex_buffer_);
		environment::render_device().SetIndexBuffer(index_buffer_);
	}

	//------------------------------------------------------------------------------
	Terrain::~Terrain()
	{
		vertex_buffer_->Release();
		index_buffer_->Release();
	}
}