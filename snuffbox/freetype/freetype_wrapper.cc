#include "../../snuffbox/freetype/freetype_wrapper.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/freetype/freetype_font.h"
#include "../../snuffbox/content/content_manager.h"


namespace snuffbox
{
	namespace environment
	{
		namespace
		{
			FreeTypeWrapper* globalInstance = nullptr;
		}

		bool has_free_type_wrapper()
		{
			return globalInstance != nullptr;
		}

		FreeTypeWrapper& free_type_wrapper()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}
}

namespace snuffbox
{

	//-------------------------------------------------------------------------------------------------
	FreeTypeWrapper::FreeTypeWrapper()
	{
		environment::globalInstance = this;
	}

	//-------------------------------------------------------------------------------------------------
	void FreeTypeWrapper::Initialise()
	{
		FT_Error err = FT_Init_FreeType(&library_);
		
		SNUFF_XASSERT(err == 0, "Error initialising FreeType!");
		SNUFF_LOG_SUCCESS("Succesfully initialised FreeType 2.3.5");

		LoadDefaultFont();
	}

	//-------------------------------------------------------------------------------------------------
	void FreeTypeWrapper::LoadDefaultFont()
	{
		environment::content_manager().Load<Font>("fonts/arial.ttf");
		default_font_ = environment::content_manager().Get<Font>("fonts/arial.ttf").get();

		SNUFF_XASSERT(default_font_ != nullptr, "Error loading default font fonts/arial.ttf!");
	}

	//-------------------------------------------------------------------------------------------------
	FT_Library& FreeTypeWrapper::library()
	{
		return library_;
	}

	//-------------------------------------------------------------------------------------------------
	Font* FreeTypeWrapper::default_font()
	{
		return default_font_;
	}

	//-------------------------------------------------------------------------------------------------
	FreeTypeWrapper::~FreeTypeWrapper()
	{
		environment::globalInstance = nullptr;
	}
}