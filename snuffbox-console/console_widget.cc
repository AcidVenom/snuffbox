#define _WINSOCKAPI_
#include "console_widget.h"
#include <QStyleOption>
#include <string>
#include "moc_console_widget.cpp"

#include "networking/connection.h"

//------------------------------------------------------------
ConsoleWidget::ConsoleWidget() : terminal_(new Terminal(this)), layout_(new QVBoxLayout())
{
	terminal_->text()->setMinimumSize(QSize(300,380));
	terminal_->resize(QSize(300,380));

	layout_->setContentsMargins(2, 2, 2, 2);

	label_ = new QLabel();
	label_->setStyleSheet("color: rgb(170,170,170);");

	label_->setText("Snuffbox Console");

	layout_->addWidget(terminal_->text());
	layout_->addWidget(terminal_->line());
	layout_->addWidget(label_);
}

//------------------------------------------------------------
ConsoleWidget::~ConsoleWidget()
{

}