#include "../../snuffbox/freetype/freetype_rich_text.h"
#include "../../snuffbox/logging.h"

#define IS_WHITESPACE_CHAR(ch) (ch == L'\n' || ch == L'\t' || ch == L'\r' || ch == L' ')

namespace snuffbox
{
	MarkupOperations RichTextParser::Parse(std::wstring* buffer)
	{
		MarkupOperations operations;
		wchar_t ch = L'';
		std::wstring current = L"";
		bool inTag = false;

		for (int i = 0; i < buffer->size(); ++i)
		{
			ch = buffer->at(i);

			if (ch == L'[')
			{
				inTag = true;
				if (current.size() > 0)
				{
					Markup markup;
					markup.isDefault = true;
					markup.text = current;
					operations.push_back(markup);

					current.clear();
				}
				++i;

				if (i >= buffer->size())
				{
					return operations;
				}
				else
				{
					if (buffer->at(i) == L'/')
					{
						continue;
					}
				}
				std::wstring tag = GetTag(i, buffer);
				std::wstring toClose = L"[/" + tag + L"]";
				bool nested = false;
				Markup markup;

				if (tag == L"b")
				{
					markup.bold = true;
				}
				else if (tag == L"i")
				{
					markup.italic = true;
				}
				else if (ConsumeRaw(L"size", &tag))
				{
					RemoveWhitespaces(&tag);
					SplitValue(&tag);

					markup.doSize = true;
					markup.size = GetSize(&tag);

					toClose = L"[/size]";
				}
				else if (ConsumeRaw(L"colour", &tag))
				{
					RemoveWhitespaces(&tag);
					SplitValue(&tag);

					std::vector<float> colour = GetColour(&tag);
					markup.doColour = true;
					
					for (int j = 0; j < colour.size(); ++j)
					{
						markup.colour[j] = colour[j];
					}

					toClose = L"[/colour]";
				}
				else if (ConsumeRaw(L"icon", &tag))
				{
					RemoveWhitespaces(&tag);
					SplitValue(&tag);

					markup.icon = true;
					markup.iconPath = tag;

					nested = true;
				}
				else if (ConsumeRaw(L"font", &tag))
				{
					RemoveWhitespaces(&tag);
					SplitValue(&tag);

					markup.newFont = true;

					int size = WideCharToMultiByte(CP_UTF8, 0, tag.c_str(), -1, NULL, 0, 0, 0);

					char* multistr = new char[size];
					WideCharToMultiByte(CP_UTF8, 0, tag.c_str(), -1, multistr, size, 0, 0);

					markup.font = multistr;

					delete multistr;

					toClose = L"[/font]";
				}

				if (nested == false)
				{
					std::wstring encapsulated = L"";
					while (i < buffer->size())
					{
						++i;
						if (ConsumeRawIndex(toClose, i, buffer))
						{
							operations.push_back(markup);

							if (Contains(L'[', &encapsulated))
							{
								MarkupOperations ops = Parse(&encapsulated);
								for (int i = 0; i < ops.size(); ++i)
								{
									operations.push_back(ops.at(i));
								}
							}
							else
							{
								Markup markup;
								markup.isDefault = true;
								markup.text = encapsulated;
								operations.push_back(markup);
							}
							break;
						}
						if (i >= buffer->size())
						{

							int size = WideCharToMultiByte(CP_UTF8, 0, buffer->c_str(), -1, NULL, 0, 0, 0);

							char* multistr = new char[size];
							WideCharToMultiByte(CP_UTF8, 0, buffer->c_str(), -1, multistr, size, 0, 0);

							SNUFF_ASSERT("RichText formatting error! Aborting\n\n\t" + std::string(multistr));
						}
						encapsulated += buffer->at(i);
					}
				}
			}
			else if (ch == L']')
			{
				inTag = false;
			}
			else if (inTag == false)
			{
				current += ch;
			}
		}

		if (current.size() > 0)
		{
			Markup markup;
			markup.isDefault = true;
			markup.text = current;
			operations.push_back(markup);
		}

		return operations;
	}

	bool RichTextParser::Consume(std::wstring toConsume, int& i, std::wstring* buffer)
	{
		int start = i;
		while (i - start < toConsume.size() && i - start < buffer->size())
		{
			if (toConsume.at(i - start) != buffer->at(i))
			{
				return false;
			}

			++i;
		}

		return true;
	}

	bool RichTextParser::ConsumeRaw(std::wstring toConsume, std::wstring* buffer)
	{
		int i = 0;
		while (i < toConsume.size() && i < buffer->size())
		{
			if (toConsume.at(i) != buffer->at(i))
			{
				return false;
			}

			++i;
		}

		return true;
	}

	bool RichTextParser::ConsumeRawIndex(std::wstring toConsume, int i, std::wstring* buffer)
	{
		int start = i;
		while (i - start < toConsume.size() && i - start < buffer->size())
		{
			if (i >= buffer->size() || toConsume.at(i - start) != buffer->at(i))
			{
				return false;
			}

			++i;
		}

		return true;
	}

	bool RichTextParser::Contains(wchar_t value, std::wstring* buffer)
	{
		for (int i = 0; i < buffer->size(); ++i)
		{
			if (buffer->at(i) == value)
			{
				return true;
			}
		}

		return false;
	}

	std::wstring RichTextParser::GetTag(int& i, std::wstring* buffer)
	{
		std::wstring result;
		wchar_t ch = L'';

		while (i < buffer->size())
		{
			SkipWhitespaces(i, buffer);
			ch = buffer->at(i);

			if (ch == L']')
			{
				break;
			}

			result += ch;
			++i;
		}

		return result;
	}

	void RichTextParser::SkipWhitespaces(int& i, std::wstring* buffer)
	{
		while (IS_WHITESPACE_CHAR(buffer->at(i)))
		{
			++i;
		}
	}

	void RichTextParser::RemoveWhitespaces(std::wstring* buffer)
	{
		std::wstring result;
		wchar_t ch = L'';

		for (int i = 0; i < buffer->size(); ++i)
		{
			ch = buffer->at(i);
			if (!IS_WHITESPACE_CHAR(ch))
			{
				result += ch;
			}
		}

		*buffer = result;
	}

	void RichTextParser::SplitValue(std::wstring* buffer)
	{
		std::wstring result = L"";
		bool next = false;
		wchar_t ch = L'';
		for (int i = 0; i < buffer->size(); ++i)
		{
			ch = buffer->at(i);

			if (next == true)
			{
				result += ch;
			}
			if (ch == L'=')
			{
				next = true;
			}
		}

		*buffer = result;
	}

	std::vector<float> RichTextParser::GetColour(std::wstring* buffer)
	{
		std::wstring hex = L"0x" + *buffer;
		int colour = wcstoul(hex.c_str(), NULL, 0);
		std::vector<float> col;

		col.push_back(((colour >> 16) & 0xFF) / 255.0f);
		col.push_back(((colour >> 8) & 0xFF) / 255.0f);
		col.push_back(((colour) & 0xFF) / 255.0f);

		return col;
	}

	float RichTextParser::GetSize(std::wstring* buffer)
	{
		return wcstof(buffer->c_str(), NULL);
	}
}