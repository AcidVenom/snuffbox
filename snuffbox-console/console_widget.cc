#define _WINSOCKAPI_
#include "console_widget.h"
#include <QStyleOption>
#include <time.h>
#include <string>
#include "moc_console_widget.cpp"

#include "networking/connection.h"

//------------------------------------------------------------
ConsoleWidget::ConsoleWidget(QApplication& parent, Connection& connection)
: 
parent_(parent),
connection_(connection),
connected_(false)
{
	vLayout_ = new QVBoxLayout();
	lineEdit_ = new QLineEdit();
	textBox_ = new QTextBrowser();
	scrollBar_ = new QScrollBar();
	vLayout_->setContentsMargins(2, 2, 2, 2);

	label_ = new QLabel();
	label_->setStyleSheet("color: rgb(170,170,170);");

	label_->setText("Snuffbox Console");

	textBox_->resize(400, 480);
	textBox_->addScrollBarWidget(scrollBar_, Qt::AlignRight);
	textBox_->setContentsMargins(QMargins(0, 0, 0, 0));

	textBox_->setStyleSheet("background-color: rgb(46,58,38); font-size:8pt; font-weight:900;");

	lineEdit_->setStyleSheet("background-color: rgb(46,58,38);");
	vLayout_->addWidget(textBox_);
	vLayout_->addWidget(lineEdit_);
	vLayout_->addWidget(label_);
	lineEdit_->setAlignment(Qt::AlignBottom);

  infoPath_ = QDir::currentPath() + "\\img\\info.png";
  warningPath_ = QDir::currentPath() + "\\img\\warning.png";
  successPath_ = QDir::currentPath() + "\\img\\success.png";
  errorPath_ = QDir::currentPath() + "\\img\\error.png";

	port_ = SNUFF_DEFAULT_PORT;
	ip_ = "127.0.0.1";

	QObject::connect(lineEdit_, SIGNAL(returnPressed()), this, SLOT(HandleEvent()));
}

//------------------------------------------------------------
void ConsoleWidget::HandleEvent()
{
	QString txt = lineEdit_->text();

	if (strcmp(txt.toStdString().c_str(), "reconnect") == 0)
	{
		if (connected_)
		{
			AddLine(LogSeverity::kError, "Already connected to the engine!");
			lineEdit_->setText("");
			return;
		}
		AddLine(LogSeverity::kInfo, "Attempting to reconnect..");
		const char* result = connection_.Initialise();
		if (strcmp(result, "Success") != 0)
		{
			AddLine(LogSeverity::kFatal, "Failed to re-initialise the connection socket!");
		}
		result = connection_.Connect(*this, parent_);

		if (strcmp(result, "Success") != 0)
			AddLine(LogSeverity::kFatal, result);
		else
		{
			AddLine(LogSeverity::kSuccess, "Connection established");
			socketThread_.join();
			socketThread_ = std::thread(&Connection::Receive,connection_,this);
		}
	}
	else
	{
		QString result = "Unknown command: " + txt;
		AddLine(LogSeverity::kError, result.toStdString().c_str());
	}

	lineEdit_->setText("");
}

//------------------------------------------------------------
void ConsoleWidget::WelcomeMessage()
{
	QString result = "Awaiting connection on IP: " + ip_ + ":" + port_ + "..";

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
  QString result;
  
  result = time + " " + line;
  SeverityColours colour = SeverityToColour(severity);

	QTextCursor cursor = textBox_->textCursor();
	QTextBlockFormat format;
	format.setBackground(colour.bg);
	
	QTextCharFormat textFormat;
	textFormat.setForeground(colour.fg);

  cursor.movePosition(QTextCursor::End);
  cursor.insertBlock(format, textFormat);
  cursor.movePosition(QTextCursor::StartOfBlock);

  

  switch (severity)
  {
  case LogSeverity::kInfo:
    cursor.insertHtml(QString("<img src=" + infoPath_ + "></img> [Info] "));
    break;
  case LogSeverity::kSuccess:
    cursor.insertHtml("<img src=" + successPath_ + "></img> <span style='color: rgb(45, 225, 70);'> [Success] </span>");
    break;
  case LogSeverity::kWarning:
    cursor.insertHtml("<img src=" + warningPath_ + "></img> <span style='color: rgb(190, 179, 87);'> [Warning] </span>");
    break;
  case LogSeverity::kError:
    cursor.insertHtml("<img src=" + errorPath_ + "></img> <span style='color: red;'> [Error] </span>");
    break;
  case LogSeverity::kFatal:
    cursor.insertHtml("<img src=" + errorPath_ + "></img> [Error] ");
    break;
  }
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(result);

	vLayout_->update();
	textBox_->update();
}

//------------------------------------------------------------
ConsoleWidget::~ConsoleWidget()
{

}