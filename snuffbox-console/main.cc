#define _WINSOCKAPI_
#include <QApplication>
#include <QtGui>

#include "console_widget.h"
#include "networking/connection.h"
#include <qstylefactory.h>

int main(int argc, char** argv)
{
	Connection connection;
	
	QApplication app(argc, argv);
	app.setApplicationName("Snuffbox Console");
	app.setOrganizationName("Daniël Konings");
	app.setOrganizationDomain("http://www.danielkonings.com/");
	app.setStyle(QStyleFactory::create("fusion"));

	qApp->setStyleSheet("QScrollBar{color: white;} QTextBrowser, QLineEdit {border-radius: 2px; border: 1px solid rgb(115,109,63); color: white; selection-color: rgb(20,20,70); }");
	QWidget window;
	window.resize(360, 480);
	window.show();
	window.setWindowTitle(QApplication::translate("mainWindow","Snuffbox Console"));
	window.setStyleSheet("background-color: rgb(76,88,68);");
	
	ConsoleWidget console(app,connection);

	window.setLayout(console.layout());
	const char* result = connection.Initialise();

	if (strcmp(result,"Success") != 0)
		console.AddLine(LogSeverity::kFatal,result);
	else
	{
		console.AddLine(LogSeverity::kSuccess, "Successfully opened socket");
		console.WelcomeMessage();
	}

	result = connection.Connect(console,app);

	if (strcmp(result, "Success") != 0)
		console.AddLine(LogSeverity::kFatal, result);
	else
		console.AddLine(LogSeverity::kSuccess, "Connection established");

	return app.exec();
}


