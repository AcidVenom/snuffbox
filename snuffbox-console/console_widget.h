#pragma once

#include <QtGui>
#include <qlayout.h>
#include <qlineedit.h>
#include <qtextbrowser.h>
#include <qscrollbar.h>
#include <qlabel.h>
#include <qmenubar.h>
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

class ConsoleWidget : public QObject
{
public:
	Q_OBJECT
public:
	ConsoleWidget(QApplication& parent, Connection& connection);
	~ConsoleWidget();

	QVBoxLayout* layout(){ return vLayout_; }

	void AddLine(LogSeverity severity, const char* msg);
	void WelcomeMessage();

	QLineEdit* line(){ return lineEdit_; }

	SeverityColours SeverityToColour(LogSeverity severity);

private slots:
	void HandleEvent();

private:
	QVBoxLayout* vLayout_;
	QApplication& parent_;
	Connection& connection_;
	
	QTextBrowser* textBox_;
	QLineEdit* lineEdit_;
	QScrollBar* scrollBar_;
	QLabel* label_;

	QString port_;
	QString ip_;
  QString infoPath_;
  QString warningPath_;
  QString successPath_;
  QString errorPath_;
};