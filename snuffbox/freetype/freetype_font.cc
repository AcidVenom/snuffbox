#include "../../snuffbox/freetype/freetype_font.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/d3d11/d3d11_settings.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------
	Font::Font(std::string path) : face_(NULL)
	{
		std::string p = environment::game().path() + "/" + path;
		const FT_Library& lib = environment::free_type_wrapper().library();
		
		std::string fullPath = "";
		for (int i = 0; i < p.length(); ++i)
		{
			auto it = p.at(i);
			if (it == '/')
			{
				fullPath += "\\";
			}
			else
			{
				fullPath += it;
			}
		}
		
		FT_Error err = FT_New_Face(lib, fullPath.c_str(), 0, &face_);

		if (err == 1)
		{
			SNUFF_LOG_ERROR("Cannot open font file, probably the file doesn't exist or the file is corrupt");
		}
		else if (err == 2)
		{
			SNUFF_LOG_ERROR("Unknown font format");
		}
		else if (err != 0)
		{
			SNUFF_LOG_ERROR("Unknown error loading a font");
		}

		SNUFF_XASSERT(err == 0, std::string("[FreeType] Unable to load font " + path).c_str());
	
		err = FT_Set_Char_Size(
			face_,
			0,
			16 * 64,
			environment::render_settings().settings().resolution.w,
			environment::render_settings().settings().resolution.h
			);

		if (err != 0)
		{
			SNUFF_LOG_WARNING(std::string("Font " + path + " does not support sizing").c_str());
		}

		
		std::string test = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
		FT_GlyphSlot slot = face_->glyph;
		FT_UInt glyph_index = 0;

		for (int i = 0; i < test.length(); ++i)
		{
			char ch = test.at(i);

			if (atlas_.ContainsGlyph(ch) == true)
			{
				continue;
			}

			glyph_index = FT_Get_Char_Index(face_, ch);

			err = FT_Load_Glyph(face_, glyph_index, FT_LOAD_RENDER);

			if (err)
			{
				continue;
			}

			
		}
	}

	//---------------------------------------------------------------------------------------------
	const FT_Face& Font::face()
	{
		return face_;
	}

	//---------------------------------------------------------------------------------------------
	const FontAtlas& Font::atlas()
	{
		return atlas_;
	}

	//---------------------------------------------------------------------------------------------
	Font::~Font()
	{

	}
}