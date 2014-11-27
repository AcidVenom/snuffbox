#include "../../snuffbox/freetype/freetype_font.h"
#include "../../snuffbox/freetype/freetype_font_atlas.h"
#include "../../snuffbox/freetype/freetype_font_manager.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/d3d11/d3d11_texture.h"

#include "../../snuffbox/logging.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include FT_LCD_FILTER_H

#define HRES  64
#define HRESf 64.f
#define DPI   72

namespace snuffbox
{
  //------------------------------------------------------------------------------------------------
  Font::Font() :
    library_(nullptr),
    face_(nullptr)
  {
    
  }

  //------------------------------------------------------------------------------------------------
  Font::~Font()
  {
    if (face_ != nullptr)
    {
      FT_Done_Face(face_);
      face_ = nullptr;
    }

    if (library_ != nullptr)
    {
      FT_Done_FreeType(library_);
      library_ = nullptr;
    }
  }

  //------------------------------------------------------------------------------------------------
  void Font::Load(std::string relativePath, float size)
  {
    FT_Error error;

    FT_Matrix matrix = {
      (int)((1.0 / HRES) * 0x10000L),
      (int)((0.0) * 0x10000L),
      (int)((0.0) * 0x10000L),
      (int)((1.0) * 0x10000L),
    };

    error = FT_Init_FreeType(&library_);
    SNUFF_XASSERT(!error, std::string("Error initializing freetype for font " + relativePath).c_str());

    error = FT_New_Face(library_, (environment::game().path() + "/" + relativePath).c_str(), 0, &face_);
    SNUFF_XASSERT(!error, std::string("Failed loading face for font " + relativePath).c_str());

    error = FT_Select_Charmap(face_, FT_ENCODING_UNICODE);
    SNUFF_XASSERT(!error, std::string("Error selecting charmap for font " + relativePath).c_str());

    error = FT_Set_Char_Size(face_, (int)(size * HRES), 0, DPI*HRES, DPI);
    SNUFF_XASSERT(!error, std::string("Error setting size for font " + relativePath).c_str());

    FT_Set_Transform(face_, &matrix, nullptr);

    size_ = size;

    height_ = 0.0f;
    ascender_ = 0.0f;
    descender_ = 0.0f;

    lcd_weights_[0] = 0x10; 
    lcd_weights_[1] = 0x40;
    lcd_weights_[2] = 0x70;
    lcd_weights_[3] = 0x40;
    lcd_weights_[4] = 0x10;
    
    FT_Size_Metrics metrics;
    metrics = face_->size->metrics;
    ascender_ = (metrics.ascender >> 6) / 100.0f;
    descender_ = (metrics.descender >> 6) / 100.0f;
    height_ = (metrics.height >> 6) / 100.0f;
    linegap_ = height_ - ascender_ + descender_;

    LoadGlyph(-1);
  }

  //------------------------------------------------------------------------------------------------
  FontGlyph* Font::LoadGlyph(wchar_t charcode)
  {
		FontAtlas* atlas = environment::font_manager().atlas();
    for (auto it = glyphs_.find(charcode); it != glyphs_.end() && it->second->charcode == charcode; ++it)
    {
      FontGlyph* glyph = it->second.get();
      if (glyph->charcode == (wchar_t)(-1))
      {
        return it->second.get();
      }
    }

    if (charcode == (wchar_t)(-1))
    {
			int width = atlas->size();
			int height = atlas->size();
			FontAtlasRegion region = atlas->CreateRegion(5, 5);
      SharedPtr<FontGlyph> glyph = environment::memory().ConstructShared<FontGlyph>();
      static signed char data[4 * 4 * 3] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

      SNUFF_XASSERT(region.x > 0, "Font Texture Atlas is full!");

			atlas->FillRegion(region, reinterpret_cast<const unsigned char*>(data), 0);
      glyph->charcode = (wchar_t)(-1);
      glyph->tex_coords.left = region.x / (float)width;
      glyph->tex_coords.top = region.y / (float)height;
      glyph->tex_coords.right = region.width / (float)width;
      glyph->tex_coords.bottom = region.height / (float)height;

      FontGlyph* ptr = glyph.get();
      glyphs_.emplace(-1, glyph);
      return ptr;
    }

    wchar_t buffer[2] = { charcode, 0 };
    if (LoadGlyphs(buffer) == 0)
    {
      return glyphs_.find(charcode)->second.get();
    }

    return nullptr;
  }

  //------------------------------------------------------------------------------------------------
  int Font::LoadGlyphs(wchar_t* str)
  {
		FontAtlas* atlas = environment::font_manager().atlas();
		int atlasSize = atlas->size();

    const int count = static_cast<int>(std::char_traits<wchar_t>::length(str));
    for (int i = 0; i < count; ++i)
    {
      wchar_t c = str[i];

      if (glyphs_.find(c) != glyphs_.end())
      {
        continue;
      }

      FT_UInt glyphIndex = FT_Get_Char_Index(face_, c);

			FT_Library_SetLcdFilterWeights(library_, lcd_weights_);
			FT_Int32 flags = FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT;

      FT_Error error = FT_Load_Glyph(face_, glyphIndex, flags);
      SNUFF_XASSERT(!error, "Unable to load glyph");

      FT_GlyphSlot slot;
      FT_Bitmap ftBitmap;
      int ftGlyphTop = 0, ftGlyphLeft = 0;

      slot = face_->glyph;
      ftBitmap = slot->bitmap;
      ftGlyphTop = slot->bitmap_top;
      ftGlyphLeft = slot->bitmap_left;

      int width = ftBitmap.width + 1;
      int height = ftBitmap.rows + 1;
			FontAtlasRegion region = atlas->CreateRegion(width, height);
      SNUFF_XASSERT(region.x > 0, "Texture font atlas is full!");

      region.width -= 1;
      region.height -= 1;
			atlas->FillRegion(region, ftBitmap.buffer, ftBitmap.pitch);

      SharedPtr<FontGlyph> glyph = environment::memory().ConstructShared<FontGlyph>();
      glyph->charcode = c;
      glyph->width = width - 1;
      glyph->height = height - 1;
      glyph->x_offset = ftGlyphLeft;
      glyph->y_offset = ftGlyphTop;
      glyph->tex_coords.left = region.x / (float)atlasSize;
      glyph->tex_coords.top = region.y / (float)atlasSize;
      glyph->tex_coords.right = glyph->tex_coords.left + glyph->width / (float)atlasSize;
      glyph->tex_coords.bottom = glyph->tex_coords.top + glyph->height / (float)atlasSize;

			FT_Load_Glyph(face_, glyphIndex, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
      slot = face_->glyph;
      glyph->x_advance = slot->advance.x / HRESf;
      glyph->y_advance = slot->advance.y / HRESf;

      glyphs_.emplace(c, glyph);
    }

		atlas->CreateTexture();
    return 0;
  }

	//------------------------------------------------------------------------------------------------
	float Font::ascender()
	{
		return ascender_;
	}

	//------------------------------------------------------------------------------------------------
	float Font::line_gap()
	{
		return linegap_;
	}

	//------------------------------------------------------------------------------------------------
	float Font::line_height()
	{
		return height_;
	}

  //------------------------------------------------------------------------------------------------
  FontGlyph* Font::glyph(wchar_t charcode)
  {
    auto glyph = glyphs_.find(charcode);

    if (glyph == glyphs_.end())
    {
      wchar_t buffer[1] = { charcode };
      LoadGlyphs(buffer);

      glyph = glyphs_.find(charcode);
    }

    return glyph->second.get();
  }
}