#pragma once

#include <map>
#include <vector>
#include "../../snuffbox/memory/shared_ptr.h"

namespace snuffbox
{
	class Texture;

	/**
	* @struct snuffbox::FontAtlasRegion
	* @brief An atlas region, used to put glyphs into the atlas
	* @author Daniël Konings
	*/
	struct FontAtlasRegion
	{
		int w = 0, h = 0, x = 0, y = 0;

		FontAtlasRegion(int _x, int _y, int _w, int _h) : 
			x(_x),
			y(_y),
			w(_w),
			h(_h){}
	};

	/**
	* @class snuffbox::FontAtlas
	* @brief A font texture atlas to store glyphs in and to use as a texture
	* @author Daniël Konings
	*/
	class FontAtlas
	{
	public:

		///Default constructor
		FontAtlas();

		/**
		* @brief Construct a font atlas
		* @param[in] w (int) The width of the atlas
		* @param[in] h (int) The height of the atlas
		*/
		FontAtlas(int w, int h);

		/// Default destructor
		~FontAtlas();

		/**
		* @param[in] character (char) The character to check
		* @return (bool) Does this atlas contain a specific character already?
		*/
		bool ContainsGlyph(char character);

		/**
		* @brief Fills an atlas region with pixel data
		* @param[in] region (const snuffbox::FontAtlasRegion&) The region to fill
		* @param[in] buffer (const unsigned char*) The buffer to use for filling
		* @param[in] stride (unsigned short) The stride to use for filling
		*/
		void FillRegion(const FontAtlasRegion& region, const unsigned char* buffer, unsigned short stride);

		/**
		* @brief Creates an atlas region that is automatically fitted
		* @param[in] w (int) The width of the pixels to put in the atlas
		* @param[in] h (int) The height of the pixels to put in the atlas
		* @param[in] character (char) The character to put in the region
		* @return (snuffbox::FontAtlasRegion&) The region
		*/
		FontAtlasRegion& CreateRegion(int w, int h, char character);

		/**
		* @return (snuffbox::Texture*) The atlas texture
		*/
		Texture* texture();

	private:
		std::map<char, bool> glyphs_; //!< List of loaded glyphs
		std::vector<FontAtlasRegion> regions_; //!< List of font atlas regions
		std::vector<const unsigned char*> data_; //!< The data in the atlas
		int	width_; //!< The width of the texture atlas
		int height_; //!< The height of the texture atlas
		SharedPtr<Texture> atlas_;	//!< The actual texture of the atlas
	};
}