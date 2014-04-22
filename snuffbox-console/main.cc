#define _WINSOCKAPI_
#include <QApplication>
#include <QtGui>

#include "console_widget.h"
#include "networking/connection.h"
#include <qstylefactory.h>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("Snuffbox Console");
	app.setOrganizationName("Daniël Konings");
	app.setOrganizationDomain("http://www.danielkonings.com/");
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

	QWidget window;
	ConsoleWidget* console = new ConsoleWidget();

	window.setLayout(console->layout());
	window.show();

	return app.exec();
}


