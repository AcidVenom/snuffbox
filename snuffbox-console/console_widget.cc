#include "console_widget.h"
#include <QStyleOption>
#include <time.h>
#include <string>

//------------------------------------------------------------
ConsoleWidget::ConsoleWidget()
{
	vLayout_ = new QVBoxLayout();
	lineEdit_ = new QLineEdit();
	textBox_ = new QTextBrowser();
	scrollBar_ = new QScrollBar();
	menuBar_ = new QMenuBar();

	vLayout_->setContentsMargins(2, 2, 2, 2);
	
	menuBar_->setMinimumWidth(360);
	menuBar_->setStyleSheet("background-color: rgb(46,58,38); border-radius: 2px; border: 1px solid rgb(115,109,63); color: white;");
	QAction* setPort = new QAction("Set port", menuBar_);
	QAction* setIP = new QAction("Set IP", menuBar_);

	QMenu* settingsMenu = menuBar_->addMenu("Settings");
	settingsMenu->addAction(setPort);
	settingsMenu->addSeparator();
	settingsMenu->addAction(setIP);
	settingsMenu->setStyleSheet("selection-color: rgb(190,179,87);");

	label_ = new QLabel();
	label_->setStyleSheet("color: rgb(170,170,170);");

	label_->setText("Snuffbox Console");

	textBox_->resize(360, 480);
	textBox_->addScrollBarWidget(scrollBar_, Qt::AlignRight);
	textBox_->setContentsMargins(QMargins(0, 0, 0, 0));

	textBox_->setStyleSheet("background-color: rgb(46,58,38); font-size:8pt; font-weight:900;");

	lineEdit_->setStyleSheet("background-color: rgb(46,58,38);");
	vLayout_->addWidget(menuBar_);
	vLayout_->addWidget(textBox_);
	vLayout_->addWidget(lineEdit_);
	vLayout_->addWidget(label_);
	lineEdit_->setAlignment(Qt::AlignBottom);

	port_ = 1337; 
	ip_ = "127.0.0.1";

	QString result = "Awaiting connection on IP: " + ip_ + ":" + std::to_string(port_).c_str() + "..";

	AddLine(LogSeverity::kInfo, "Welcome!");
	AddLine(LogSeverity::kInfo, result.toStdString().c_str());
}

//------------------------------------------------------------
SeverityColours ConsoleWidget::SeverityToColour(LogSeverity severity)
{
	QColor bg;
	QColor fg;

	switch (severity)
	{
		case LogSeverity::kDebug :
			bg = QColor(46, 58, 38);
			fg = QColor(138, 138, 138);
			break;

		case LogSeverity::kInfo:
			bg = QColor(46, 58, 38);
			fg = QColor(255, 255, 255);
			break;

		case LogSeverity::kWarning:
			bg = QColor(71, 58, 9);
			fg = QColor(190, 179, 87);
			break;

		case LogSeverity::kSuccess:
			bg = QColor(21, 96, 51);
			fg = QColor(45, 225, 70);
			break;

		case LogSeverity::kError:
			bg = QColor(63, 36, 36);
			fg = QColor(255,10,10);
			break;

		case LogSeverity::kFatal:
			bg = QColor(255, 0, 0);
			fg = QColor(255, 255, 255);
			break;
	}

	return SeverityColours(bg, fg);
}

//------------------------------------------------------------
void ConsoleWidget::AddLine(LogSeverity severity, const char* msg)
{
	time_t t = time(0);
	struct tm * now = localtime(&t);
	std::string hour = std::to_string(now->tm_hour);
	if (strlen(hour.c_str()) < 2)
	{
		hour = "0" + hour;
	}

	std::string min = std::to_string(now->tm_min);
	if (strlen(min.c_str()) < 2)
	{
		min = "0" + min;
	}

	std::string sec = std::to_string(now->tm_sec);
	if (strlen(sec.c_str()) < 2)
	{
		sec = "0" + sec;
	}
	std::string timeStamp = hour + ":" + min + ":" + sec;
	QString time = timeStamp.c_str();
	QString line(msg);
	QString result = time + " " + line;
	SeverityColours colour = SeverityToColour(severity);

	QTextCursor cursor = textBox_->textCursor();
	QTextBlockFormat format;
	format.setBackground(colour.bg);
	
	QTextCharFormat textFormat;
	textFormat.setForeground(colour.fg);

	cursor.movePosition(QTextCursor::End);
	cursor.insertBlock(format, textFormat);
	cursor.insertText(result);
	
}

//------------------------------------------------------------
ConsoleWidget::~ConsoleWidget()
{

}