#pragma once

#include "../../snuffbox/freetype/freetype_wrapper.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Font
	* @author Daniël Konings
	* @brief Stores font information loaded by FreeType
	*/
	class Font
	{
	public:
		/// Default constructor
		Font(std::string path);

		/// Default destructor
		~Font();

		/**
		* @return (const FT_Face&) The face object
		*/
		const FT_Face& face();

		/**
		* @return (const snuffbox::FontAtlas&) The font atlas of this font
		*/
		const FontAtlas& atlas();

		/// Create the atlas of this font
		void CreateAtlas();

	private:
		FT_Face		face_; //!< Handle to the font face
		FontAtlas atlas_; //!< The font atlas for this font
	};
}