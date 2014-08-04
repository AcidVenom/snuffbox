#include "../../snuffbox/console/console.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/logging.h"

namespace snuffbox
{
	namespace environment
	{
		namespace
		{
			Console* globalInstance = nullptr;
		}

		bool has_console(){ return globalInstance != nullptr; }
		Console& console()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}
}
namespace snuffbox
{
	//----------------------------------------------------------------------
	Console::Console(QWidget& window, ConsoleWidget& console_widget) :
		window_(window),
		console_widget_(console_widget),
		is_loaded_(false)
	{
		environment::globalInstance = this;
	}

	//----------------------------------------------------------------------
	Console::~Console()
	{
		environment::globalInstance = nullptr;
	}

	//----------------------------------------------------------------------
	void Console::Initialise(QApplication& app)
	{
		app.setApplicationName("Snuffbox Console");
		app.setStyle(QStyleFactory::create("Fusion"));

		QPalette steamPalette;
		steamPalette.setColor(QPalette::Window, QColor(88, 106, 80));
		steamPalette.setColor(QPalette::WindowText, Qt::white);
		steamPalette.setColor(QPalette::Base, QColor(64, 70, 60));
		steamPalette.setColor(QPalette::AlternateBase, QColor(88, 106, 80));
		steamPalette.setColor(QPalette::ToolTipBase, Qt::white);
		steamPalette.setColor(QPalette::ToolTipText, Qt::white);
		steamPalette.setColor(QPalette::Text, Qt::white);
		steamPalette.setColor(QPalette::Button, QColor(88, 106, 80));
		steamPalette.setColor(QPalette::ButtonText, Qt::white);
		steamPalette.setColor(QPalette::BrightText, Qt::red);
		steamPalette.setColor(QPalette::Link, QColor(159, 164, 98));

		steamPalette.setColor(QPalette::Highlight, QColor(159, 164, 98));
		steamPalette.setColor(QPalette::HighlightedText, Qt::black);

		app.setPalette(steamPalette);
		is_loaded_ = true;
	}

	//----------------------------------------------------------------------
	void Console::Receive(LogSeverity sev, const char* msg)
	{
		console_widget_.AddLine(sev, msg);
	}

	//----------------------------------------------------------------------
	void Console::Watch(std::string name, Local<Value>& val)
	{
		console_widget_.AddToWatch(name, val);
	}

	//----------------------------------------------------------------------
	void Console::Show()
	{
		console_widget_.Show();
	}

	//----------------------------------------------------------------------
	void Console::Hide()
	{
		console_widget_.Hide();
	}
}