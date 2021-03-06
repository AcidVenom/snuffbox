#include "../../snuffbox/console/console_widget.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/game.h"
#include <time.h>

#define SNUFF_SAFE_DELETE(ptr) SNUFF_ASSERT_NOTNULL(ptr); delete ptr; ptr = NULL;

namespace snuffbox
{
	//---------------------------------------------------------------
	JavaScriptSyntaxHighlighter::JavaScriptSyntaxHighlighter(QObject* parent) : QSyntaxHighlighter(parent)
	{
		Initialise();
	}

	//---------------------------------------------------------------
	JavaScriptSyntaxHighlighter::JavaScriptSyntaxHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent)
	{
		Initialise();
	}

	//---------------------------------------------------------------
	void JavaScriptSyntaxHighlighter::Initialise()
	{
		HighlightingRule rule;

		keywordFormat.setForeground(QColor(159, 164, 98));
		keywordFormat.setFontWeight(QFont::Bold);
		QStringList keywordPatterns;
		keywordPatterns << "\\bvoid\\b" << "\\bbreak\\b" << "\\bconst\\b"
			<< "\\b?\\b" << "\\bcontinue\\b" << "\\bdelete\\b"
			<< "\\bdo\\b" << "\\bwhile\\b" << "\\bexport\\b"
			<< "\\bfor\\b" << "\\bin\\b" << "\\bimport\\b"
			<< "\\binstanceOf\\b" << "\\blabel\\b" << "\\blet\\b"
			<< "\\bnew\\b" << "\\breturn\\b" << "\\bswitch\\b"
			<< "\\bthis\\b" << "\\bthrow\\b" << "\\btry\\b"
			<< "\\bcatch\\b" << "\\btypeof\\b" << "\\bvar\\b"
			<< "\\bwith\\b" << "\\byield\\b" << "\\bassert\\b"
			<< "\\bLog\\b" << "\\bGame\\b" << "\\bfunction\\b" << "\\bnull\\b"
			<< "\\bundefined\\b" << "\\btrue\\b" << "\\bfalse\\b" << "\\bdefault\\b"
			<< "\\bcase\\b" << "\\bif\\b" << "\\belse\\b";
		foreach(const QString &pattern, keywordPatterns) {
			rule.pattern = QRegExp(pattern);
			rule.format = keywordFormat;
			highlightingRules.append(rule);
		}

		quotationFormat.setForeground(QColor(240, 165, 110));
		rule.pattern = QRegExp("\".*\"");
		rule.format = quotationFormat;
		highlightingRules.append(rule);

		functionFormat.setFontWeight(QFont::Bold);
		functionFormat.setFontItalic(true);
		functionFormat.setForeground(QColor(225, 210, 140));
		rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
		rule.format = functionFormat;
		highlightingRules.append(rule);

		singleLineCommentFormat.setForeground(Qt::gray);
		rule.pattern = QRegExp("//[^\n]*");
		rule.format = singleLineCommentFormat;
		highlightingRules.append(rule);

		multiLineCommentFormat.setForeground(Qt::gray);

		commentStartExpression = QRegExp("/\\*");
		commentEndExpression = QRegExp("\\*/");
	}

	//---------------------------------------------------------------
	void JavaScriptSyntaxHighlighter::highlightBlock(const QString& text)
	{
		foreach(const HighlightingRule &rule, highlightingRules) {
			QRegExp expression(rule.pattern);
			int index = expression.indexIn(text);
			while (index >= 0) {
				int length = expression.matchedLength();
				setFormat(index, length, rule.format);
				if (length == 0)
					break;
				index = expression.indexIn(text, index + length);
			}
		}

		setCurrentBlockState(0);

		int startIndex = 0;
		if (previousBlockState() != 1)
			startIndex = commentStartExpression.indexIn(text);

		while (startIndex >= 0) {
			int endIndex = commentEndExpression.indexIn(text, startIndex);
			int commentLength;
			if (endIndex == -1) {
				setCurrentBlockState(1);
				commentLength = text.length() - startIndex;
			}
			else {
				commentLength = endIndex - startIndex
					+ commentEndExpression.matchedLength();
			}
			setFormat(startIndex, commentLength, multiLineCommentFormat);
			startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
		}
	}

	//---------------------------------------------------------------
	ConsoleWidget::ConsoleWidget(QWidget& parent) :
		window_(&parent),
		ui_(new Ui::ConsoleUI()),
		shift_pressed_(false),
		history_index_(0),
		last_line_(""),
		repeat_count_(0),
		last_sev_(LogSeverity::kDebug)
	{
		window_->setMinimumSize(QSize(400, 480));

		ui_->setupUi(window_);
		highlighter_ = new JavaScriptSyntaxHighlighter(ui_->commandLine->document());

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

		ui_->commandLine->installEventFilter(this);
	}

	bool ConsoleWidget::eventFilter(QObject *obj, QEvent *event)
	{
		if (obj == ui_->commandLine)
		{
			if (event->type() == QEvent::KeyPress)
			{
				QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
				if (keyEvent->key() == Qt::Key_Return && !shift_pressed_)
				{
					HandleCommand();
					return true;
				}

				if (keyEvent->key() == Qt::Key_Shift)
				{
					shift_pressed_ = true;
				}
			}

			if (event->type() == QEvent::KeyRelease)
			{
				QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
				if (keyEvent->key() == Qt::Key_Shift)
				{
					shift_pressed_ = false;
				}

				if (shift_pressed_)
				{
					if (keyEvent->key() == Qt::Key_Up)
					{
						if (history_index_ > 0)
						{
							--history_index_;
							ui_->commandLine->setText(history_[history_index_]);
							return true;
						}
					}

					if (keyEvent->key() == Qt::Key_Down)
					{
						if (history_index_ < static_cast<unsigned int>(history_.size()))
						{
							++history_index_;
							ui_->commandLine->setText(history_[history_index_ - 1]);
							return true;
						}
						else
						{
							ui_->commandLine->setText("");
						}
					}
				}
			}
		}

		return false;
	}

	//---------------------------------------------------------------
	void ConsoleWidget::Hide()
	{
		window_->setVisible(false);
	}

	//---------------------------------------------------------------
	void ConsoleWidget::Show()
	{
		window_->showNormal();
	}

	//---------------------------------------------------------------
	void ConsoleWidget::HandleCommand()
	{
		if ((environment::has_game() && !environment::game().started()) || !environment::has_game())
		{
			AddLine(LogSeverity::kFatal, "Cannot execute JavaScript commands when the game is shut down!");
			ui_->commandLine->clear();
			return;
		}

		JS_CREATE_SCOPE;
		std::string txt = ui_->commandLine->toPlainText().toStdString();

		TryCatch try_catch;

		Local<Script> script = Script::Compile(String::NewFromUtf8(JS_ISOLATE, txt.c_str()), String::NewFromUtf8(JS_ISOLATE, "Console"));
		Local<Value> res = script->Run();

		if (res.IsEmpty())
		{
			bool failed = false;
			std::string exception(environment::js_state_wrapper().GetException(&try_catch, &failed));
			AddLine(LogSeverity::kError, exception.c_str());
		}
		else
		{
			AddLine(LogSeverity::kDebug, *String::Utf8Value(res->ToString()));
			if (ui_->toggleWatch->isChecked() && !res->IsUndefined())
			{
				AddToWatch(txt.c_str(), res);
			}
		}

		history_.push_back(ui_->commandLine->toPlainText());
		history_index_ = static_cast<unsigned int>(history_.size());
		ui_->commandLine->clear();
	}

	//---------------------------------------------------------------
	void ConsoleWidget::AddToWatch(std::string name, Local<Value>& variable)
	{
		QTreeWidgetItem* item = nullptr;
		bool found = false;
		auto it = watched_variables_.find(name);
		if (it != watched_variables_.end())
		{
			QList<QTreeWidgetItem*> list = it->second->takeChildren();
			for (auto child : list)
			{
				it->second->removeChild(child);
			}

			item = it->second;
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
			watched_variables_.emplace(name, item);
		}
	}

	//---------------------------------------------------------------
	void ConsoleWidget::AddToTree(Local<Value>& value, QTreeWidgetItem* parent, bool next)
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

					if (next)
					{
						AddToTree(childValue, child, false);
					}
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
	void ConsoleWidget::AddLine(LogSeverity sev, const char* msg, float r1, float g1, float b1, float r2, float g2, float b2)
	{
		if (msg == nullptr) return;

		if (strcmp(last_line_.c_str(), msg) == 0 && last_sev_ == sev)
		{
			++repeat_count_;
			QTextCursor cursor = ui_->terminal->textCursor();
			cursor.movePosition(QTextCursor::End);
			if (repeat_count_ > 1)
			{
				unsigned int width = static_cast<unsigned int>(std::strlen(std::to_string(repeat_count_ - 1).c_str()));
				for (unsigned int i = 0; i < width + 5; ++i)
				{
					cursor.deletePreviousChar();
					cursor.movePosition(QTextCursor::End);
				}
			}

			cursor.insertText(" ");

			QTextCharFormat format;
			format.setBackground(QColor(128, 158, 182, 255));
			format.setForeground(QColor(255, 255, 255, 255));

			cursor.setCharFormat(format);

			cursor.insertText((std::string(" [") + std::to_string(repeat_count_) + std::string("] ")).c_str());
			return;
		}

		repeat_count_ = 0;

		last_sev_ = sev;
		last_line_ = std::string(msg);
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
			bg = QColor(70, 65, 15);
			break;
		case LogSeverity::kError:
			fg = QColor(255, 0, 0);
			bg = QColor(60, 0, 0);
			break;
		case LogSeverity::kFatal:
			fg = QColor(255, 255, 255);
			bg = QColor(255, 0, 0);
			break;
    case LogSeverity::kRgb:
      fg = QColor(r1, g1, b1);
      bg = QColor(r2, g2, b2);
      break;
		}

		QTextCursor cursor = ui_->terminal->textCursor();
		QTextBlockFormat format;
		format.setBackground(bg);

		QTextCharFormat textFormat;
		textFormat.setForeground(fg);

		cursor.movePosition(QTextCursor::End);
		cursor.insertBlock(format, textFormat);

		std::string prefix = "";

		if (sev != LogSeverity::kDebug && sev != LogSeverity::kRgb)
		{
			cursor.insertText((timeStamp + std::string(" [") + SeverityToString(sev) + "] " + std::string(msg)).c_str());
		}
		else
		{
			cursor.insertText((timeStamp + std::string(" ") + std::string(msg)).c_str());
		}
		ui_->terminal->verticalScrollBar()->setSliderPosition(ui_->terminal->verticalScrollBar()->maximum());
	}

	//---------------------------------------------------------------
	ConsoleWidget::~ConsoleWidget()
	{
		SNUFF_SAFE_DELETE(highlighter_);

		for (auto it = watched_variables_.begin(); it != watched_variables_.end(); ++it)
		{
			QList<QTreeWidgetItem*> list = it->second->takeChildren();
			for (auto child : list)
			{
				it->second->removeChild(child);
				SNUFF_SAFE_DELETE(child);
			}

			SNUFF_SAFE_DELETE(it->second);
		}

		watched_variables_.clear();

		SNUFF_SAFE_DELETE(ui_);
	}
}