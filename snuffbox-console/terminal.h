#pragma once

#define _WINSOCKAPI_
#include <QtWidgets>
#include <qtextdocument.h>
#include <qplaintextedit.h>
#include <qlayout.h>
#include <qlineedit.h>
#include "networking/connection.h"

enum LogSeverity
{
	kInfo,
	kDebug,
	kSuccess,
	kError,
	kFatal,
	kWarning
};

struct SeverityColours
{
	SeverityColours(QColor& bg, QColor& fg) : bg(bg), fg(fg){}
	QColor fg;
	QColor bg;
};

class Terminal : public QAbstractScrollArea
{
public:
	Q_OBJECT
public:
	Terminal(QObject* parent);
	~Terminal();

	void AddLine(LogSeverity severity, const char* msg);
	SeverityColours SeverityToColour(LogSeverity severity);
	void WelcomeMessage();

	Connection* connection(){ return connection_; }
	QLineEdit* line(){ return lineEdit_; }
	QTextBrowser* text(){ return terminal_; }
private slots:
	void HandleEvent();
	void ReceiveMsg(int sev, QString msg);
private:
	QLineEdit* lineEdit_;
	QTextBrowser* terminal_;
	QString* ip_;
	QString* port_;
	QObject* parent_;

	QString* infoPath_;
	QString* warningPath_;
	QString* successPath_;
	QString* errorPath_;

	Connection* connection_;
};