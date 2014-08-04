#pragma once
#include <QtGui>
#include <map>

#include "qcompleter.h"

#include "v8.h"

#include "../../snuffbox/console/console_ui.h"



using namespace v8;

namespace snuffbox
{
	/**
	* @class snuffbox::JavaScriptSyntaxHighlighter
	* @brief Highlights JavaScript syntax in the console
	* @author Daniël Konings
	*/
	class JavaScriptSyntaxHighlighter : public QSyntaxHighlighter
	{
	public:
		JavaScriptSyntaxHighlighter(QObject* parent);
		JavaScriptSyntaxHighlighter(QTextDocument* parent);
		~JavaScriptSyntaxHighlighter(){}

		/// Initialises the highlighter
		void Initialise();

	protected:
		void highlightBlock(const QString& text);

	private:
		struct HighlightingRule
		{
			QRegExp pattern;
			QTextCharFormat format;
		};
		QVector<HighlightingRule> highlightingRules;

		QRegExp commentStartExpression;
		QRegExp commentEndExpression;

		QTextCharFormat keywordFormat;
		QTextCharFormat singleLineCommentFormat;
		QTextCharFormat multiLineCommentFormat;
		QTextCharFormat quotationFormat;
		QTextCharFormat functionFormat;
	};

	/**
	* @enum snuffbox::LogSeverity
	* @brief Different log severities
	* @author Daniël Konings
	*/
	enum LogSeverity
	{
		kInfo,
		kDebug,
		kSuccess,
		kError,
		kFatal,
		kWarning
	};

	/**
	* @class snuffbox::ConsoleWidget
	* @brief The Qt console widget for the actual console
	* @author Daniël Konings
	*/
	class ConsoleWidget : public QObject
	{
	public:
		Q_OBJECT
	public:
		/**
		* @brief Constructs a Console Widget class
		* @param[in] parent (QWidget&) The Qt widget parent
		*/
		ConsoleWidget(QWidget& parent);

		/// Default destructor
		~ConsoleWidget();

		/// Shows the console when hidden
		void Show();

		/// Hides the console
		void Hide();

		/**
		* @brief Adds a line to the console
		* @param[in] sev (snuffbox::LogSeverity) The severity of the message
		* @param[in] msg (const char*) The actual message
		*/
		void AddLine(LogSeverity sev, const char* msg);

		/**
		* @brief Adds a value to the watch tree
		* @param[in] obj (v8::Local<Value>&) The actual value to pass on
		* @param[in] parent (QTreeWidgetItem*) The parent item this value should come from
		* @param[in] next (bool) Recurse from here or stop
		*/
		void AddToTree(Local<Value>& obj, QTreeWidgetItem* parent, bool next = true);

		/**
		* @brief Adds a variable to the watch tree
		* @param[in] name (std::string) The name the variable should lie under
		* @param[in] obj (v8::Local<Value>&) The JavaScript variable
		*/
		void AddToWatch(std::string name, Local<Value>& obj);

		/// Returns if the window is visible
		bool is_visible(){ return window_->isVisible(); }

	private:
		/// Handles a command entered in the command line
		void HandleCommand();

	protected:
		bool eventFilter(QObject *obj, QEvent *evt);

	private:
		QWidget* window_;	//!< The parent window
		Ui::ConsoleUI* ui_;	//!< The actual console UI
		std::map<std::string, QTreeWidgetItem*> watched_variables_; //!< A list of watched variables
		JavaScriptSyntaxHighlighter* highlighter_; //!< Highlight JavaScript syntax
		bool shift_pressed_; //!< If shift is pressed..
		std::vector<QString> history_; //!< A list for the history of the console
		unsigned int history_index_; //!< The current history index
		std::string last_line_; //!< The last line added to the console
		unsigned int repeat_count_; //!< The repeat count for a console line
		LogSeverity last_sev_;	//!< The last type of severity
	};
}