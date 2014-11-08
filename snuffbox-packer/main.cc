#include <qapplication.h>
#include <qmainwindow.h>
#include <qstylefactory.h>

#include "qt/packer_widget.h"

using namespace snuffboxpacker;

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  app.setApplicationName("Snuffbox Packer");
  app.setStyle(QStyleFactory::create("Fusion"));

  QPalette steamPalette;
  steamPalette.setColor(QPalette::Window, QColor(88, 106, 80));
  steamPalette.setColor(QPalette::WindowText, Qt::white);
  steamPalette.setColor(QPalette::Base, QColor(64, 70, 60));
  steamPalette.setColor(QPalette::AlternateBase, QColor(88, 106, 80));
  steamPalette.setColor(QPalette::ToolTipBase, Qt::white);
  steamPalette.setColor(QPalette::ToolTipText, Qt::white);
  steamPalette.setColor(QPalette::Text, Qt::white);
  steamPalette.setColor(QPalette::Button, QColor(88, 106, 80));
  steamPalette.setColor(QPalette::ButtonText, Qt::white);
  steamPalette.setColor(QPalette::BrightText, Qt::red);
  steamPalette.setColor(QPalette::Link, QColor(159, 164, 98));

  steamPalette.setColor(QPalette::Highlight, QColor(159, 164, 98));
  steamPalette.setColor(QPalette::HighlightedText, Qt::black);

  app.setPalette(steamPalette);

  QMainWindow* window = new QMainWindow();

  PackerWidget* packer = new PackerWidget(window);

  window->show();

  int exitCode = app.exec();

  delete packer;
  packer = nullptr;

  delete window;
  window = nullptr;

  return exitCode;
}