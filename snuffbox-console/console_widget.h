#pragma once

#include <QtGui>
#include <qlayout.h>
#include <qlineedit.h>
#include <qtextbrowser.h>
#include <qscrollbar.h>
#include <qlabel.h>
#include <qmenubar.h>

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

class ConsoleWidget
{

public:
	ConsoleWidget();
	~ConsoleWidget();

	QVBoxLayout* layout(){ return vLayout_; }

	void AddLine(LogSeverity severity, const char* msg);
	void paintEvent(QPaintEvent* );

	SeverityColours SeverityToColour(LogSeverity severity);

private:
	QVBoxLayout* vLayout_;
	
	QMenuBar* menuBar_;
	QTextBrowser* textBox_;
	QLineEdit* lineEdit_;
	QScrollBar* scrollBar_;
	QLabel* label_;

	unsigned int port_;
	QString ip_;
};