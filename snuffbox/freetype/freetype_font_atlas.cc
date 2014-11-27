#include "../../snuffbox/freetype/freetype_font_atlas.h"
#include "../../snuffbox/d3d11/d3d11_texture.h"

namespace snuffbox
{
  //------------------------------------------------------------------------------------------------
  FontAtlas::FontAtlas(int size, unsigned char depth) :
    size_(size),
    depth_(depth),
    used_(0)
  {
    data_.resize(size*size*depth, 0);
    nodes_.emplace_back(1u, 1u, size - 2u);
  }

  //------------------------------------------------------------------------------------------------
  int FontAtlas::TestFit(int idx, int width, int height)
  {
    const FontAtlasNode& node = nodes_.at(idx);
    int x = node.x;
    int y = node.y;
    int widthLeft = width;

    if (x + width > size_ - 1)
      return std::numeric_limits<int>::max();

    while (widthLeft > 0)
    {
      const FontAtlasNode& n = nodes_.at(idx);

      if (n.y > y) y = n.y;
      if ((y + height) > (size_ - 1)) return std::numeric_limits<int>::max();

      widthLeft -= n.z;
      ++idx;
    }

    return y;
  }

  //------------------------------------------------------------------------------------------------
  FontAtlasRegion FontAtlas::CreateRegion(int width, int height)
  {
    FontAtlasRegion region(0, 0, width, height);

    int bestHeight = std::numeric_limits<int>::max();
    int bestWidth = std::numeric_limits<int>::max();
    int bestIndex = std::numeric_limits<int>::max();

    for (int i = 0; i < nodes_.size(); ++i)
    {
      int y = TestFit(i, width, height);
      if (y < std::numeric_limits<int>::max())
      {
        const FontAtlasNode& node = nodes_[i];
        if (((y + height) < bestHeight) ||
          (y + height == bestHeight && node.z < bestWidth))
        {
          bestHeight = y + height;
          bestIndex = i;
          bestWidth = node.z;
          region.x = node.x;
          region.y = y;
        }
      }
    }

    if (bestIndex == std::numeric_limits<int>::max())
    {
      region.x = region.y = std::numeric_limits<int>::max();
      region.width = region.height = 0;
      return region;
    }

    FontAtlasNode node;
    node.x = region.x;
    node.y = region.y + height;
    node.z = width;
    nodes_.insert(nodes_.begin() + bestIndex, node);

    for (int i = bestIndex + 1; i < nodes_.size(); ++i)
    {
      FontAtlasNode &node = nodes_[i];
      FontAtlasNode &prev = nodes_[i - 1];

      if (node.x < (prev.x + prev.z))
      {
        int shrink = prev.x + prev.z - node.x;
        node.x += shrink;
        if (node.z <= shrink)
        {
          nodes_.erase(nodes_.begin() + i);
          --i;
        }
        else
        {
          node.z -= shrink;
          break;
        }
      }
      else
        break;
    }

    Merge();

    used_ += width*height;
    return region;
  }

  //-------------------------------------------------------------------------------------------------
  void FontAtlas::Merge()
  {
    for (int i = 0; i < nodes_.size() - 1; ++i)
    {
      FontAtlasNode &node = nodes_[i];
      FontAtlasNode &next = nodes_[i + 1];

      if (node.y == next.y)
      {
        node.z += next.z;
        nodes_.erase(nodes_.begin() + i + 1);
        --i;
      }
    }
  }

  //------------------------------------------------------------------------------------------------
  void FontAtlas::FillRegion(const FontAtlasRegion& region, const unsigned char* data, int stride)
  {
    SNUFF_XASSERT(region.x > 0 && region.y > 0 && region.x + region.width < size_ - 1 && region.y + region.height < size_ - 1, "The region does not fit in the atlas");

    for (int y = 0; y < region.height; ++y)
    {
      for (int x = 0; x < region.width; ++x)
      {
        unsigned char luminance = data[y*stride + x];
        int color = (luminance << 0) | (luminance << 8) | (luminance << 16) | (luminance << 24);
        data_[(region.y + y) * size_ + region.x + x] = color;
      }
    }
  }


  //-------------------------------------------------------------------------------------------------
  void FontAtlas::ClearRegion(const FontAtlasRegion& region)
  {
    for (int y = 0; y < region.height; ++y)
    {
      for (int x = 0; x < region.width; ++x)
      {
        data_[(region.y + y) * size_ + region.x + x] = 0;
      }
    }
  }

  //------------------------------------------------------------------------------------------------
  void FontAtlas::Clear()
  {
    nodes_.clear();
    nodes_.emplace_back(1u, 1u, size_ - 2u);
  }

  //------------------------------------------------------------------------------------------------
  int FontAtlas::size()
  {
    return size_;
  }

  //------------------------------------------------------------------------------------------------
	unsigned char FontAtlas::depth()
  {
    return depth_;
  }

  //------------------------------------------------------------------------------------------------
  void FontAtlas::CreateTexture()
  {
    atlas_ = environment::memory().ConstructShared<Texture>(environment::render_device().CreateTexture2D(size_, size_, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, &data_[0], size_ * depth_));
  }

  //------------------------------------------------------------------------------------------------
  FontAtlas::~FontAtlas()
  {

  }
}