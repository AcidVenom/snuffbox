#include "terminal.h"
#include "moc_terminal.cpp"
#include <time.h>

//----------------------------------------------------------------
Terminal::Terminal(QObject* parent)
:
terminal_(new QTextBrowser()),
lineEdit_(new QLineEdit()),
connection_(new Connection(this)),
parent_(parent),
maxLines_(800),
lines_(0)
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	setAttribute(Qt::WA_MacSmallSize);

	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_InputMethodEnabled);
	setAttribute(Qt::WA_KeyCompression);
	setAttribute(Qt::WA_MacShowFocusRect);
	setMouseTracking(true);
	setAcceptDrops(false);

	infoPath_ = new QString(QDir::currentPath() + "\\img\\info.png");
	warningPath_ = new QString(QDir::currentPath() + "\\img\\warning.png");
	successPath_ = new QString(QDir::currentPath() + "\\img\\success.png");
	errorPath_ = new QString(QDir::currentPath() + "\\img\\error.png");

	QFont f;
	f.setStyleHint(QFont::Monospace);
	f.setWeight(QFont::Bold);
	setFont(f);

	terminal_->setFont(font());

	QObject::connect(lineEdit_, SIGNAL(returnPressed()), this, SLOT(HandleEvent()));
	WelcomeMessage();

	ip_ = new QString("127.0.0.1");
  port_ = new QString(SNUFF_DEFAULT_PORT);

	viewport()->setCursor(Qt::IBeamCursor);
}

//----------------------------------------------------------------
Terminal::~Terminal()
{

}

//------------------------------------------------------------
SeverityColours Terminal::SeverityToColour(LogSeverity severity)
{
	QColor bg;
	QColor fg;

	switch (severity)
	{
	case LogSeverity::kDebug:
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
		fg = QColor(255, 10, 10);
		break;

	case LogSeverity::kFatal:
		bg = QColor(255, 0, 0);
		fg = QColor(255, 255, 255);
		break;
	}

	return SeverityColours(bg, fg);
}

//------------------------------------------------------------
void Terminal::AddLine(LogSeverity severity, const char* msg)
{
	time_t t = time(0);
	struct tm now = *localtime(&t);
	std::string hour = std::to_string(now.tm_hour);
	if (strlen(hour.c_str()) < 2)
	{
		hour = "0" + hour;
	}

	std::string min = std::to_string(now.tm_min);
	if (strlen(min.c_str()) < 2)
	{
		min = "0" + min;
	}

	std::string sec = std::to_string(now.tm_sec);
	if (strlen(sec.c_str()) < 2)
	{
		sec = "0" + sec;
	}
	std::string timeStamp = hour + ":" + min + ":" + sec;
	QString time = timeStamp.c_str();
	QString result;

	result = time + " " + msg;
	SeverityColours colour = SeverityToColour(severity);

	QTextCursor cursor = terminal_->textCursor();
	QTextBlockFormat format;
	format.setBackground(colour.bg);

	QTextCharFormat textFormat;
	textFormat.setForeground(colour.fg);

	cursor.movePosition(QTextCursor::End);
	cursor.insertBlock(format, textFormat);

	switch (severity)
	{
		case LogSeverity::kInfo:
			cursor.insertHtml("<img src=" + *infoPath_ + "></img> [Info] ");
			break;
		case LogSeverity::kSuccess:
			cursor.insertHtml("<img src=" + *successPath_ + "></img> <span style='color: rgb(45, 225, 70);'> [Success] </span>");
			break;
		case LogSeverity::kWarning:
			cursor.insertHtml("<img src=" + *warningPath_ + "></img> <span style='color: rgb(190, 179, 87);'> [Warning] </span>");
			break;
		case LogSeverity::kError:
			cursor.insertHtml("<img src=" + *errorPath_ + "></img> <span style='color: red;'> [Error] </span>");
			break;
		case LogSeverity::kFatal:
			cursor.insertHtml("<img src=" + *errorPath_ + "></img> [Error] ");
			break;
	}
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(result);

	if (++lines_ > maxLines_)
	{
		lines_ = 0;
		terminal_->clear();
	}

	viewport()->update();
	terminal_->verticalScrollBar()->setValue(terminal_->verticalScrollBar()->maximum());
}

//------------------------------------------------------------
void Terminal::WelcomeMessage()
{
	AddLine(LogSeverity::kInfo, "Welcome!");
	AddLine(LogSeverity::kInfo, "Please start the engine if you're experiencing problems!");
}

//------------------------------------------------------------
void Terminal::HandleEvent()
{
	QString txt = lineEdit_->text();
	if (txt == "")
		return;
	if (strcmp(txt.toStdString().c_str(), "reconnect") == 0)
	{
		if (!connection_->connected())
		{
			delete connection_;
			connection_ = new Connection(this);
      if(int result = connection_->Initialise(ip_->toStdString().c_str()) == 0)
			  connection_->Connect();
		}
		else
		{
			AddLine(LogSeverity::kError, "Already connected to the engine!");
		}
	}
  else if (strcmp(txt.toStdString().c_str(), "ip") == 0)
  {
    QString result = "Current IP address: " + *ip_;
    AddLine(LogSeverity::kInfo, result.toStdString().c_str());
  }
	else
	{
    if (strlen(txt.toStdString().c_str()) >= 6)
    {
      std::string str = txt.toStdString();
      std::string result = "";
      for (unsigned int i = 0; i < 6; ++i)
      {
        result += str.at(i);
      }
      if (strcmp(result.c_str(), "set ip") == 0)
      {
        ip_ = new QString(&str[6]);
      }
      
      result = "Set new IP to: " + ip_->toStdString();
      AddLine(LogSeverity::kInfo, result.c_str());
      lineEdit_->setText("");
      return;
    }

		QString result = "Unknown command: " + txt;
		AddLine(LogSeverity::kError, result.toStdString().c_str());
	}

	lineEdit_->setText("");
}

//------------------------------------------------------------
void Terminal::ReceiveMsg(int sev, QString msg)
{
	AddLine(static_cast<LogSeverity>(sev), msg.toStdString().c_str());
}