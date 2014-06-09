#include "../../snuffbox/console/console_widget.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/js/js_state_wrapper.h"
#include <time.h>

namespace snuffbox
{
	//---------------------------------------------------------------
	ConsoleWidget::ConsoleWidget(QWidget& parent) :
		window_(&parent),
		ui_(new Ui::ConsoleUI())
	{
		window_->setMinimumSize(QSize(400, 480));
		ui_->setupUi(window_);
		
		QFont monospace;
		monospace.setFamily("Arial");
		monospace.setStyleHint(QFont::Monospace);
		monospace.setFixedPitch(true);
		monospace.setPointSize(10);
		monospace.setBold(true);

		ui_->commandLine->setFont(monospace);
		ui_->terminal->setFont(monospace);

		QFontMetrics metrics(monospace);
		ui_->terminal->setTabStopWidth(2 * metrics.width(" "));
		ui_->variableTree->header()->setVisible(true);

		QObject::connect(ui_->commandLine, SIGNAL(returnPressed()), this, SLOT(HandleCommand()));
	}

	//---------------------------------------------------------------
	void ConsoleWidget::Show()
	{
		window_->show();
	}

	//---------------------------------------------------------------
	void ConsoleWidget::HandleCommand()
	{
		JS_CREATE_SCOPE;
		std::string txt = ui_->commandLine->text().toStdString();

		TryCatch try_catch;

		Local<Script> script = Script::Compile(String::NewFromUtf8(JS_ISOLATE, txt.c_str()), String::NewFromUtf8(JS_ISOLATE, "Console"));
		Local<Value> res = script->Run();

		if (res.IsEmpty())
		{
			bool failed = false;
			std::string exception(environment::js_state_wrapper().GetException(&try_catch, &failed));
			AddLine(LogSeverity::kError,exception.c_str());
		}
		else if (!res->IsUndefined())
		{
			AddLine(LogSeverity::kDebug,*String::Utf8Value(res->ToString()));
			if (ui_->toggleWatch->isChecked())
			{
				AddToWatch(txt.c_str(),res);
			}
		}

		ui_->commandLine->clear();
	}

	//---------------------------------------------------------------
	void ConsoleWidget::AddToWatch(std::string name, Local<Value>& variable)
	{
		QTreeWidgetItem* item = nullptr;
		bool found = false;
		auto it = watchedVariables_.find(name);
		if (it != watchedVariables_.end())
		{
			item = it->second;
			found = true;
			std::vector<QTreeWidgetItem*> temp;
			for (unsigned int i = 0; i != item->childCount(); ++i)
			{
				temp.push_back(item->child(i));
			}

			for (auto it : temp)
			{
				item->removeChild(it);
				delete it;
			}
		}
		else
		{
			item = new QTreeWidgetItem();
		}
		SNUFF_ASSERT_NOTNULL(item);
		item->setText(0, name.c_str());
		if (variable->IsFunction())
		{
			item->setText(1, "[Function]");
		}
		else if (variable->IsObject() || variable->IsArray())
		{
			if (variable->IsArray())
				item->setText(1, "[Array]");
			else
				item->setText(1, "[Object]");

			AddToTree(variable, item);
		}
		else
		{
			item->setText(0, std::string(item->text(0).toStdString() + ": " + *String::Utf8Value(variable->ToString())).c_str());
			item->setTextColor(0, QColor(128, 128, 128));
			item->setBackgroundColor(0, QColor(30, 30, 30, 50));
		}
		item->setTextColor(1, QColor(159, 164, 98));

		if (!found)
		{
			ui_->variableTree->addTopLevelItem(item);
			watchedVariables_.emplace(name,item);
		}
	}

	//---------------------------------------------------------------
	void ConsoleWidget::AddToTree(Local<Value>& value, QTreeWidgetItem* parent)
	{
		if (value->IsObject() || value->IsArray())
		{
			Local<Object> obj = value->ToObject();

			Local<Array> names = obj->GetPropertyNames();

			for (unsigned int i = 0; i < names->Length(); ++i)
			{
				QTreeWidgetItem* child = new QTreeWidgetItem();
				child->setText(0, *String::Utf8Value(names->Get(i)->ToString()));
				child->setBackgroundColor(0, QColor(30, 30, 30, 50));

				Local<Value> childValue = obj->Get(names->Get(i));
				if (childValue->IsFunction())
				{
					QTreeWidgetItem* smallChild = new QTreeWidgetItem();
					smallChild->setText(0, *String::Utf8Value(childValue->ToString()));
					smallChild->setTextColor(0, QColor(128, 128, 128));
					smallChild->setBackgroundColor(0, QColor(30, 30, 30, 50));
					child->addChild(smallChild);
					child->setText(1, "[Function]");
				}
				else if (childValue->IsObject() || childValue->IsArray())
				{
					if (childValue->IsArray())
						child->setText(1, "[Array]");
					else
						child->setText(1, "[Object]");

					AddToTree(childValue, child);
				}
				else
				{
					child->setText(0, std::string(child->text(0).toStdString() + ": " + *String::Utf8Value(childValue->ToString())).c_str());
					child->setTextColor(0, QColor(128, 128, 128));
					child->setBackgroundColor(0, QColor(30, 30, 30, 50));
				}
				child->setTextColor(1, QColor(159, 164, 98));
				parent->addChild(child);
			}
		}
	}

	//---------------------------------------------------------------
	void ConsoleWidget::AddLine(LogSeverity sev, const char* msg)
	{
		time_t t = time(0);
		struct tm now;
		localtime_s(&now, &t);
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

		QColor fg;
		QColor bg;

		switch (sev)
		{
		case LogSeverity::kDebug:
			fg = QColor(128, 128, 128);
			bg = QColor(0, 0, 0, 0);
			break;
		case LogSeverity::kInfo:
			fg = QColor(255, 255, 255);
			bg = QColor(30, 30, 30, 30);
			break;
		case LogSeverity::kSuccess:
			fg = QColor(35, 255, 0);
			bg = QColor(70, 120, 55);
			break;
		case LogSeverity::kWarning:
			fg = QColor(255, 185, 0);
			bg = QColor(140, 110, 30);
			break;
		case LogSeverity::kError:
			fg = QColor(255, 0, 0);
			bg = QColor(60, 0, 0);
			break;
		case LogSeverity::kFatal:
			fg = QColor(255, 255, 255);
			bg = QColor(255, 0, 0);
			break;
		}

		QTextCursor cursor = ui_->terminal->textCursor();
		QTextBlockFormat format;
		format.setBackground(bg);

		QTextCharFormat textFormat;
		textFormat.setForeground(fg);

		cursor.movePosition(QTextCursor::End);
		cursor.insertBlock(format, textFormat);

		cursor.insertText((timeStamp + std::string(" ") + std::string(msg)).c_str());
		ui_->terminal->verticalScrollBar()->setSliderPosition(ui_->terminal->verticalScrollBar()->maximum());
	}

	//---------------------------------------------------------------
	ConsoleWidget::~ConsoleWidget()
	{
		
	}
}