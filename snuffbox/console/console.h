#pragma once
#include <qapplication.h>
#include <QtWidgets>
#include <qstylefactory.h>
#include "../../snuffbox/console/console_ui.h"
#include "../../snuffbox/console/console_widget.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Console
	* @brief The actual JavaScript console
	* @author Daniël Konings
	*/
	class Console
	{
	public:
		/// Default constructor
		Console(QWidget& window, ConsoleWidget& console_widget);
		/// Default destructor
		~Console();

		/// Initialises the console
		void Initialise(QApplication& app);

		/// Receive messages from log functions
		void Receive(LogSeverity severity, const char* msg);

		/// Watches a variable
		void Watch(std::string name, Local<Value>& val);

		/// Shows the console when hidden
		void Show();

	private:
		QWidget& window_; ///< The Qt window
		ConsoleWidget& console_widget_; ///< The console widget
	};
}