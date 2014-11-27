#include "../../snuffbox/freetype/freetype_font_manager.h"
#include "../../snuffbox/freetype/freetype_font_atlas.h"

#define BASE_WCHAR L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"

namespace snuffbox
{
  namespace environment
  {
    namespace
    {
      FontManager* globalInstance = nullptr;
    }

    bool has_font_manager()
    {
      return globalInstance != nullptr;
    }

    FontManager& font_manager()
    {
      SNUFF_ASSERT_NOTNULL(globalInstance);
      return *globalInstance;
    }
  }
  //------------------------------------------------------------------------------------------------
  FontManager::FontManager()
  {
    environment::globalInstance = this;
		atlas_ = environment::memory().ConstructShared<FontAtlas>(4096, 4);
    default_font_ = GetFont("fonts/arial.ttf", 16);
  }

  //------------------------------------------------------------------------------------------------
  Font* FontManager::GetFont(std::string path, float fontSize)
  {
    auto fontExists = map_.find(path);
    
    if (fontExists != map_.end())
    {
      auto& sizeMap = fontExists->second;
      auto sizeExists = sizeMap.find(fontSize);

      if (sizeExists != sizeMap.end())
      {
        return sizeExists->second.get();
      }
      else
      {
        SharedPtr<Font> font = environment::memory().ConstructShared<Font>();

        font->Load(path, fontSize);
        font->LoadGlyphs(BASE_WCHAR);

        Font* f = font.get();

        sizeMap.emplace(fontSize, SharedPtr<Font>(std::move(font)));

        SNUFF_LOG_INFO(std::string("Loading font " + path + " with size " + std::to_string(fontSize)).c_str());

        return f;
      }
    }
    else
    {
      SharedPtr<Font> font = environment::memory().ConstructShared<Font>();

      font->Load(path, fontSize);
      font->LoadGlyphs(BASE_WCHAR);

      std::map<float, SharedPtr<Font>> map;

      Font* f = font.get();
      map.emplace(fontSize, SharedPtr<Font>(std::move(font)));

      map_.emplace(path, map);
      SNUFF_LOG_INFO(std::string("Loading font " + path + " with size " + std::to_string(fontSize)).c_str());

      return f;
    }
  }

	//------------------------------------------------------------------------------------------------
	FontAtlas* FontManager::atlas()
	{
		return atlas_.get();
	}

  //------------------------------------------------------------------------------------------------
  Font* FontManager::default_font()
  {
    return default_font_;
  }

  //------------------------------------------------------------------------------------------------
  FontManager::~FontManager()
  {

  }
}