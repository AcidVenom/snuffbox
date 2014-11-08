#pragma once

#include <qobject.h>

#include "main_window_ui.h"
#include <string>
#include <map>

class QFileSystemModel;

namespace snuffboxpacker
{
  typedef std::map<std::string, std::vector<std::string>> Package;
  static int ContentItem = 100005;

  enum ContentTypes
  {
    kTexture = 9999,
    kFont = 10000,
    kModel = 10001,
    kBox = 10002,
    kShader = 10003,
    kUnknown = 10004
  };

  class PackerWidget : public QObject
  {
  Q_OBJECT

  public:
    PackerWidget(QMainWindow* parent);
    virtual ~PackerWidget();

  public slots:
    void OnFileClicked(const QModelIndex& index);
    void OnFileMenuRequested(QPoint point);
    void OnContentMenuRequested(QPoint point);
    void OnSave();
    void OnSaveAs();
    void OnOpen();
    void OnNew();
    void OnQuit();

  private:
    QString DeduceType(QString path);
    ContentTypes GetContentType(QString type);
    void SkipWhiteSpaces(int& i, std::string& str, char& curChar);
    bool Consume(int& i, std::string& str, std::string toConsume);
    std::string GetValue(int& i, std::string& str, std::string endAt);
    Package ParsePackage(std::string& contentPath, bool* failed);

  private:
    QString path_;
    QString currentSavePath_;
    QString toSave_;
    QMainWindow* parent_;
    Ui_MainWindow* ui_;
    QFileSystemModel* model_;
    QStringList selectedPaths_;
  };
}