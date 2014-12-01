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
		/**
		* @brief Parses a given text as markup operations
		* @param[in] buffer (std::wstring*) The text buffer to read from
		* @return (snuffbox::MarkupOperations) A list of markup operations, if no tags were found it's always a default operation
		*/
		static MarkupOperations Parse(std::wstring* buffer);
		
	private:
		/**
		* @brief Gets the tag after a '[' character
		* @param[in] i (int&) The current index, will be incremented
		* @param[in] buffer (std::wstring*) The buffer to read the tag from
		*/
		static std::wstring GetTag(int& i, std::wstring* buffer);

		/**
		* @brief Skips whitespaces
		* @param[in] i (int&) The current index, will be incremented
		* @param[in] buffer (std::wstring*) The buffer to skip whitespaces in
		*/
		static void SkipWhitespaces(int& i, std::wstring* buffer);

		/**
		* @brief Consumes a value to check whether a string segment is equal to the value
		* @param[in] toConsume (std::wstring) The value to consume
		* @param[in] i (int&) The current index, will be incremented
		* @param[in] buffer (std::wstring*) The buffer to consume in
		* @return (bool) Was the value found?
		*/
		static bool Consume(std::wstring toConsume, int& i, std::wstring* buffer);

		/**
		* @brief Consumes a value to check whether a string segment is equal to the value, without incrementing the current index
		* @param[in] toConsume (std::wstring) The value to consume
		* @param[in] buffer (std::wstring*) The buffer to consume in
		* @return (bool) Was the value found?
		*/
		static bool ConsumeRaw(std::wstring toConsume, std::wstring* buffer);

		/**
		* @brief Consumes a value to check whether a string segment is equal to the value, from a given index, current index will not increment
		* @param[in] toConsume (std::wstring) The value to consume
		* @param[in] i (int) The index to start from
		* @param[in] buffer (std::wstring*) The buffer to consume in
		* @return (bool) Was the value found?
		*/
		static bool ConsumeRawIndex(std::wstring toConsume, int i, std::wstring* buffer);

		/**
		* @brief Removes the whitespaces out of a given buffer
		* @param[in] buffer (std::wstring*) The buffer to remove whitespaces from
		*/
		static void RemoveWhitespaces(std::wstring* buffer);

		/**
		* @brief Retrieve the colour value from a string of the format 'FFAACC'
		* @param[in] buffer (std::wstring*) The buffer to read the colour value from
		* @return (std::vector<float>) A 3 sized vector of floats. [0] = R, [1] = G, [2] = B
		*/
		static std::vector<float> GetColour(std::wstring* buffer);

		/**
		* @brief Gets the size from a given string of the format '99.88'
		* @param[in] buffer (std::wstring*) The string buffer to retrieve the number from
		* @return (float) The parsed number
		*/
		static float GetSize(std::wstring* buffer);

		/**
		* @brief Splits a value off of a string buffer
		* @param[in] buffer (std::wstring) The string buffer to split
		*/
		static void SplitValue(std::wstring* buffer);

		/**
		* @brief Checks if a given character is in the string buffer
		* @param[in] value (wchar_t) The value to check for
		* @param[in] buffer (std::wstring*) The string buffer to check in
		* @return (bool) Was the value found?
		*/
		static bool Contains(wchar_t value, std::wstring* buffer);
	};
}