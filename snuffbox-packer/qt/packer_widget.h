#pragma once

#include <qobject.h>

#include "main_window_ui.h"

class QFileSystemModel;

namespace snuffboxpacker
{
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

  private:
    QString DeduceType(QString path);
    ContentTypes GetContentType(QString type);

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