#include "../../snuffbox/freetype/freetype_font_atlas.h"
#include "../../snuffbox/d3d11/d3d11_texture.h"
#include "../../snuffbox/d3d11/d3d11_display_device.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------
	FontAtlas::FontAtlas() : width_(0), height_(0)
	{

	}

	//---------------------------------------------------------------------------------------
	FontAtlas::FontAtlas(int w, int h) : width_(w), height_(h)
	{

	}

	//---------------------------------------------------------------------------------------
	bool FontAtlas::ContainsGlyph(char character)
	{
		if (glyphs_.find(character) != glyphs_.end())
		{
			return true;
		}

		return false;
	}

	//---------------------------------------------------------------------------------------
	void FontAtlas::FillRegion(const FontAtlasRegion& region, const unsigned char* buffer, unsigned short stride)
	{

	}

	//---------------------------------------------------------------------------------------
	FontAtlasRegion& FontAtlas::CreateRegion(int w, int h, char character)
	{
		if (regions_.size() == 0)
		{
			regions_.push_back(FontAtlasRegion(0, 0, w, h));
			return regions_.at(0);
		}

		int maxX = 0;
		int maxY = 0;

		for (int i = 0; i < regions_.size(); ++i)
		{
			const FontAtlasRegion& region = regions_.at(i);

			if (region.x + region.w > maxX)
			{
				maxX = region.x + region.w;
			}

			if (region.y + region.h > maxY)
			{
				maxY = region.y + region.h;
			}
		}

		if (maxX + w > width_)
		{
			maxY += h;
			maxX = -1;
		}

		regions_.push_back(FontAtlasRegion(0, 0, w, h));

		FontAtlasRegion& region = regions_.at(regions_.size() - 1);

		region.x = maxX + 1;
		region.y = maxY + 1;

		glyphs_.emplace(character, true);

		return region;
	}

	//---------------------------------------------------------------------------------------
	Texture* FontAtlas::texture()
	{
		return atlas_.get();
	}

	//---------------------------------------------------------------------------------------
	FontAtlas::~FontAtlas()
	{

	}
}