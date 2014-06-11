/********************************************************************************
** Form generated from reading UI file 'console_ui.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConsoleUI
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *consoleTabs;
    QWidget *javascript;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *toggleWatch;
    QVBoxLayout *terminalLayout;
    QTextBrowser *terminal;
    QTextEdit *commandLine;
    QWidget *variableWatch;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *variableLayout;
    QTreeWidget *variableTree;
    QLabel *snuffboxLabel;

    void setupUi(QWidget *ConsoleUI)
    {
        if (ConsoleUI->objectName().isEmpty())
            ConsoleUI->setObjectName(QStringLiteral("ConsoleUI"));
        ConsoleUI->resize(587, 537);
        ConsoleUI->setMinimumSize(QSize(587, 537));
        verticalLayout = new QVBoxLayout(ConsoleUI);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        consoleTabs = new QTabWidget(ConsoleUI);
        consoleTabs->setObjectName(QStringLiteral("consoleTabs"));
        consoleTabs->setMinimumSize(QSize(568, 501));
        javascript = new QWidget();
        javascript->setObjectName(QStringLiteral("javascript"));
        verticalLayout_2 = new QVBoxLayout(javascript);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        toggleWatch = new QCheckBox(javascript);
        toggleWatch->setObjectName(QStringLiteral("toggleWatch"));

        verticalLayout_2->addWidget(toggleWatch);

        terminalLayout = new QVBoxLayout();
        terminalLayout->setObjectName(QStringLiteral("terminalLayout"));
        terminal = new QTextBrowser(javascript);
        terminal->setObjectName(QStringLiteral("terminal"));
        terminal->setMinimumSize(QSize(0, 400));
        terminal->setMaximumSize(QSize(16777215, 16777215));
        terminal->setLineWrapMode(QTextEdit::NoWrap);

        terminalLayout->addWidget(terminal);

        commandLine = new QTextEdit(javascript);
        commandLine->setObjectName(QStringLiteral("commandLine"));
        commandLine->setMinimumSize(QSize(0, 24));
        commandLine->setMaximumSize(QSize(16777215, 24));

        terminalLayout->addWidget(commandLine);


        verticalLayout_2->addLayout(terminalLayout);

        consoleTabs->addTab(javascript, QString());
        variableWatch = new QWidget();
        variableWatch->setObjectName(QStringLiteral("variableWatch"));
        variableWatch->setMinimumSize(QSize(563, 475));
        verticalLayout_3 = new QVBoxLayout(variableWatch);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        variableLayout = new QVBoxLayout();
        variableLayout->setObjectName(QStringLiteral("variableLayout"));
        variableTree = new QTreeWidget(variableWatch);
        variableTree->setObjectName(QStringLiteral("variableTree"));
        variableTree->setMinimumSize(QSize(541, 441));
        variableTree->setIndentation(10);
        variableTree->setUniformRowHeights(false);
        variableTree->setAnimated(false);
        variableTree->setAllColumnsShowFocus(true);
        variableTree->setWordWrap(false);
        variableTree->setHeaderHidden(true);
        variableTree->setColumnCount(2);
        variableTree->header()->setVisible(false);
        variableTree->header()->setCascadingSectionResizes(false);
        variableTree->header()->setDefaultSectionSize(280);
        variableTree->header()->setHighlightSections(true);
        variableTree->header()->setMinimumSectionSize(280);
        variableTree->header()->setStretchLastSection(false);

        variableLayout->addWidget(variableTree);


        verticalLayout_3->addLayout(variableLayout);

        consoleTabs->addTab(variableWatch, QString());

        verticalLayout->addWidget(consoleTabs);

        snuffboxLabel = new QLabel(ConsoleUI);
        snuffboxLabel->setObjectName(QStringLiteral("snuffboxLabel"));
        snuffboxLabel->setStyleSheet(QStringLiteral("color: gray;"));

        verticalLayout->addWidget(snuffboxLabel);


        retranslateUi(ConsoleUI);

        consoleTabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ConsoleUI);
    } // setupUi

    void retranslateUi(QWidget *ConsoleUI)
    {
        ConsoleUI->setWindowTitle(QApplication::translate("ConsoleUI", "Snuffbox Console", 0));
        toggleWatch->setText(QApplication::translate("ConsoleUI", "Add return values to Variable Watch", 0));
        consoleTabs->setTabText(consoleTabs->indexOf(javascript), QApplication::translate("ConsoleUI", "JavaScript", 0));
        QTreeWidgetItem *___qtreewidgetitem = variableTree->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("ConsoleUI", "Variable Types", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("ConsoleUI", "Variable Names", 0));
        consoleTabs->setTabText(consoleTabs->indexOf(variableWatch), QApplication::translate("ConsoleUI", "Variable Watch", 0));
        snuffboxLabel->setText(QApplication::translate("ConsoleUI", "Snuffbox Console", 0));
    } // retranslateUi

};

namespace Ui {
    class ConsoleUI: public Ui_ConsoleUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // CONSOLE_UI_H
