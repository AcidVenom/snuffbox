#pragma once

#include <QtGui>
#include <qlayout.h>
#include <qlineedit.h>
#include <qscrollbar.h>
#include <qlabel.h>
#include <qmenubar.h>
#include <qtextdocument.h>
#include <qplaintextedit.h>
#include "networking/connection.h"
#include "terminal.h"


class ConsoleWidget : public QObject
{
public:
	Q_OBJECT
public:
	ConsoleWidget();
	~ConsoleWidget();

	QVBoxLayout* layout(){ return layout_; }
	Terminal* terminal(){ return terminal_; }

private:
	ConsoleWidget(const ConsoleWidget& other);
	QVBoxLayout* layout_;
	
	QLabel* label_;
	Terminal* terminal_;
};