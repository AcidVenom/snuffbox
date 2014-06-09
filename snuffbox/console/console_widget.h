#pragma once
#include <QtGui>
#include "../../snuffbox/console/console_ui.h"
#include "v8.h"
#include <map>

using namespace v8;

namespace snuffbox
{
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

		/// Add a log message
		void AddLine(LogSeverity sev, const char* msg);

		/// Adds a value to a tree
		void AddToTree(Local<Value>& obj, QTreeWidgetItem* parent);

		/// Adds a variable to the watch tree
		void AddToWatch(std::string name, Local<Value>& obj);

	private slots:
		/// Handles a command entered in the command line
		void HandleCommand();

	private:
		QWidget* window_;	///< The parent window
		Ui::ConsoleUI* ui_;	///< The actual console UI
		std::map<std::string,QTreeWidgetItem*> watchedVariables_; ///< A list of watched variables
	};
}