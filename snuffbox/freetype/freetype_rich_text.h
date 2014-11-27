#pragma once

#include <vector>

namespace snuffbox
{
	/**
	* @struct snuffbox::Markup
	* @brief A structure containing markup data
	* @author Daniël Konings
	*/
	struct Markup
	{
		Markup() : doSize(false), bold(false), italic(false), doColour(false), icon(false), isDefault(false), newFont(false), size(0.0f), iconPath(L""), text(L""){}
		bool doSize, bold, italic, doColour, icon, isDefault, newFont;
		float size;
		float colour[3];

		std::wstring iconPath;
		std::wstring text;
		std::string font;
	};

	typedef std::vector<Markup> MarkupOperations;

	/**
	* @class snuffbox::RichTextParser
	* @brief A rich text parser for use with a text element
	* @author Daniël Konings
	*/
	class RichTextParser
	{
	public:
		static MarkupOperations Parse(std::wstring* buffer);
		
	private:
		static std::wstring GetTag(int& i, std::wstring* buffer);
		static void SkipWhitespaces(int& i, std::wstring* buffer);
		static bool Consume(std::wstring toConsume, int& i, std::wstring* buffer);
		static bool ConsumeRaw(std::wstring toConsume, std::wstring* buffer);
		static bool ConsumeRawIndex(std::wstring toConsume, int i, std::wstring* buffer);
		static void RemoveWhitespaces(std::wstring* buffer);
		static std::vector<float> GetColour(std::wstring* buffer);
		static float GetSize(std::wstring* buffer);
		static void SplitValue(std::wstring* buffer);
		static bool Contains(wchar_t value, std::wstring* buffer);
	};
}