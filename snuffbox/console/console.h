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
		/**
		* @brief Construct with a window parent and a corresponding widget class
		* @param[in] window (QWidget&) The Qt main window widget
		* @param[in] console_widget (snuffbox::ConsoleWidget&) The underlying console widget class
		*/
		Console(QWidget& window, ConsoleWidget& console_widget);

		/// Default destructor
		~Console();

		/**
		* @brief Initialises the console
		* @param[in] app (QApplication&) The Qt application to run from
		*/
		void Initialise(QApplication& app);

		/**
		* @brief Receive a message with a severity
		* @param[in] severity (snuffbox::LogSeverity) The severity this message has
		* @param[in] msg (const char*) The actual message
		*/
		void Receive(LogSeverity severity, const char* msg);

		/**
		* @brief Watches a variable under a name
		* @param[in] name (std::string) The name to watch under
		* @param[in] val (v8::Local<Value>&) The JavaScript value
		*/
		void Watch(std::string name, Local<Value>& val);

		/// Shows the console when hidden
		void Show();

		/// Hides the console
		void Hide();

		/*
		* @return (bool) Returns if the console was ever loaded
		*/
		bool is_loaded(){ return is_loaded_; }

		/*
		* @return (bool) Returns if the console is currently visible
		*/
		bool is_visible() { return console_widget_.is_visible(); }

	private:
		QWidget& window_; //!< The Qt window
		ConsoleWidget& console_widget_; //!< The console widget
		bool is_loaded_; //!< Is the console loaded?
	};
}