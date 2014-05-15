#include "../../../snuffbox/d3d11/elements/terrain_element.h"

namespace snuffbox
{
	//------------------------------------------------------------------------------
	Terrain::Terrain()
	{

	}

	//------------------------------------------------------------------------------
	Terrain::Terrain(JS_ARGS)
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

		vertices().resize(w_*h_);

		HRESULT result = S_OK;
		for (unsigned int y = 0; y < h_; y++)
		{
			for (unsigned int x = 0; x < w_; x++)
			{
				unsigned int i = y * w_ + x;
				Vertex vertex;

				if (y % 2 == 0)
				{
					vertex.x = static_cast<float>(x);
				}
				else
				{
					vertex.x = static_cast<float>(x + 0.5f);
				}

				vertex.y = 0.0f;
				vertex.z = static_cast<float>(y*yScale);
				vertex.colour = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				vertex.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

				vertices()[i] = vertex;
			}
		}
		for (unsigned int y = 0; y < h_ - 1; ++y)
		{
			for (unsigned int x = 0; x < w_; ++x)
			{
				if ((y % 2) == 0)
				{
					indices().push_back(y*w_ + x);
					indices().push_back((y + 1)*w_ + x);
				}
				else
				{
					indices().push_back((y + 1)*w_ + x);
					indices().push_back(y*w_ + x);
				}
			}

			if (y < h_ - 2)
			{
				indices().push_back(indices().back());
				indices().push_back(indices().back() + 1);
			}
		}

		vertexBuffer_ = environment::render_device().CreateVertexBuffer(vertices());
		indexBuffer_ = environment::render_device().CreateIndexBuffer(indices());

		environment::render_device().renderElements().push_back(this);
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