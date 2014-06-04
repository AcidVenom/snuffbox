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
		JSWrapper wrapper(args);

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
				vertex.colour = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				vertex.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				vertex.texCoord = XMFLOAT2(static_cast<float>(x) / w_, 1.0f-(static_cast<float>(y) / h_));

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

		vertexBuffer_ = environment::render_device().CreateVertexBuffer(vertices());
		indexBuffer_ = environment::render_device().CreateIndexBuffer(indices());
	}

	//------------------------------------------------------------------------------
	void Terrain::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertexBuffer_);
		environment::render_device().SetIndexBuffer(indexBuffer_);
	}

	//------------------------------------------------------------------------------
	Terrain::~Terrain()
	{
		vertexBuffer_->Release();
		indexBuffer_->Release();
	}
}