#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../../snuffbox/environment.h"
#include "../../snuffbox/freetype/freetype_font_atlas.h"

#include <string>

namespace snuffbox
{
	class Font;

	/**
	* @class snuffbox::FreeTypeWrapper
	* @author Daniël Konings
	* @brief A wrapper class for all FreeType related initialisations and operations
	*/
	class FreeTypeWrapper
	{
	public:
		/// Default constructor
		FreeTypeWrapper();

		/// Default destructor
		~FreeTypeWrapper();

		/// Initialises FreeType
		void Initialise();

		/// Loads the default font
		void LoadDefaultFont();

		/**
		* @return (FT_Library&) The FreeType library used by the wrapper
		*/
		FT_Library& library();

		/**
		* @return (snuffbox::Font*) The default font
		*/
		Font* default_font();

	private:
		FT_Library		library_; //!< The FreeType library
		Font*					default_font_; //!< The default font
	};
}
