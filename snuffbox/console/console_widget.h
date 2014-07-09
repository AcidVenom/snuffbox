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
		/// Constructor by Qt object
		JavaScriptSyntaxHighlighter(QObject* parent);
		/// Constructor by Qt text document
		JavaScriptSyntaxHighlighter(QTextDocument* parent);
		/// Default destructor
		~JavaScriptSyntaxHighlighter(){}

		/// Initialises the highlighter
		void Initialise();

	protected:
		/// The overridden highlightBlock function
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
		/// Default constructor
		ConsoleWidget(QWidget& parent);

		/// Default destructor
		~ConsoleWidget();

		/// Shows the console when hidden
		void Show();

		/// Hides the console
		void Hide();

		/// Add a log message
		void AddLine(LogSeverity sev, const char* msg);

		/// Adds a value to a tree
		void AddToTree(Local<Value>& obj, QTreeWidgetItem* parent);

		/// Adds a variable to the watch tree
		void AddToWatch(std::string name, Local<Value>& obj);

		/// Returns if the window is visible
		bool isVisible(){ return window_->isVisible(); }

	private:
		/// Handles a command entered in the command line
		void HandleCommand();

	protected:
		bool eventFilter(QObject *obj, QEvent *evt);

	private:
		QWidget* window_;	///< The parent window
		Ui::ConsoleUI* ui_;	///< The actual console UI
		std::map<std::string,QTreeWidgetItem*> watchedVariables_; ///< A list of watched variables
		JavaScriptSyntaxHighlighter* highlighter_; ///< Highlight JavaScript syntax
		bool shiftPressed_; ///< If shift is pressed..
    std::vector<QString> history_; ///< A list for the history of the console
    unsigned int historyIndex_; ///< The current history index
		std::string lastLine_; ///< The last line added to the console
		unsigned int repeatCount_; ///< The repeat count for a console line
		LogSeverity lastSev_;
	};
}