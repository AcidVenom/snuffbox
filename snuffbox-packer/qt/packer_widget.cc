#include <qfilesystemmodel.h>
#include <qfiledialog.h>
#include <qdebug.h>
#include <fstream>
#include <string>
#include <qmessagebox.h>

#include "packer_widget.h"

namespace snuffboxpacker
{
  //-----------------------------------------------------------------------------------
  PackerWidget::PackerWidget(QMainWindow* parent) :
    parent_(parent),
    model_(nullptr),
    ui_(nullptr),
    path_(""),
    currentSavePath_("")
  {
    ui_ = new Ui_MainWindow();
    ui_->setupUi(parent_);

    model_ = new QFileSystemModel();
    path_ = QFileDialog::getExistingDirectory(ui_->centralwidget, "Select working directory", QDir::currentPath());

    if (path_ == nullptr)
    {
      QMessageBox* mb = new QMessageBox(QMessageBox::Icon::Critical, "Failed to open working directory", "You did not specify a working directory, aborting");
      mb->exec();

      delete mb;
      delete ui_;
      delete model_;

      exit(0);
    }
    QModelIndex idx = model_->setRootPath(path_);

    ui_->fileBrowser->setModel(model_);
    ui_->fileBrowser->setRootIndex(idx);
    ui_->fileBrowser->hideColumn(1);
    ui_->fileBrowser->hideColumn(2);
    ui_->fileBrowser->hideColumn(3);
    ui_->fileBrowser->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui_->fileBrowser->setContextMenuPolicy(Qt::CustomContextMenu);

    ui_->packageTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui_->packageTable->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui_->fileBrowser, SIGNAL(clicked(const QModelIndex&)), this, SLOT(OnFileClicked(const QModelIndex&)));
    connect(ui_->fileBrowser, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnFileMenuRequested(QPoint)));
   
    connect(ui_->packageTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnContentMenuRequested(QPoint)));

    connect(ui_->actionSave, SIGNAL(triggered()), this, SLOT(OnSave()));
    connect(ui_->actionSaveAs, SIGNAL(triggered()), this, SLOT(OnSaveAs()));

    QTreeWidgetItem* item = new QTreeWidgetItem(ContentTypes::kTexture);
    item->setText(0, "textures");
    ui_->packageTable->addTopLevelItem(item);

    item = new QTreeWidgetItem(ContentTypes::kFont);
    item->setText(0, "fonts");
    ui_->packageTable->addTopLevelItem(item);

    item = new QTreeWidgetItem(ContentTypes::kModel);
    item->setText(0, "models");
    ui_->packageTable->addTopLevelItem(item);

    item = new QTreeWidgetItem(ContentTypes::kBox);
    item->setText(0, "boxes");
    ui_->packageTable->addTopLevelItem(item);

    item = new QTreeWidgetItem(ContentTypes::kShader);
    item->setText(0, "shaders");
    ui_->packageTable->addTopLevelItem(item);

    item = new QTreeWidgetItem(ContentTypes::kUnknown);
    item->setText(0, "unknown");
    ui_->packageTable->addTopLevelItem(item);
  }

  //-----------------------------------------------------------------------------------
  void PackerWidget::OnFileClicked(const QModelIndex& index)
  {
    QList<QModelIndex> indices = ui_->fileBrowser->selectionModel()->selectedRows();
    selectedPaths_.clear();

    for (auto it : indices)
    {
      QString filePath = model_->filePath(it);
      QStringList splitted = filePath.split(path_ + "/");

      selectedPaths_.push_back(splitted.at(1));
    }
  }

  //-----------------------------------------------------------------------------------
  void PackerWidget::OnFileMenuRequested(QPoint point)
  {
    QMenu* menu = new QMenu(ui_->fileBrowser);
    QAction* add = new QAction("Add", ui_->fileBrowser);
    menu->addAction(add);
    menu->popup(ui_->fileBrowser->viewport()->mapToGlobal(point));
    QAction* selected = menu->exec();

    if (selected != nullptr)
    {
      for (auto it : selectedPaths_)
      {
        QTreeWidgetItem* item = new QTreeWidgetItem(ContentItem);
        QString type = DeduceType(it);
        ContentTypes contentType = GetContentType(type);

        QTreeWidgetItem* parent = nullptr;
        for (int i = 0; i < ui_->packageTable->topLevelItemCount(); ++i)
        {
          parent = ui_->packageTable->topLevelItem(i);

          if (contentType == parent->type())
          {
            break;
          }
        }

        item->setText(0, type);
        item->setText(1, it);

        item->setTextColor(0, QColor(159, 164, 98));
        item->setTextColor(1, QColor(200, 200, 200));

        parent->addChild(item);
      }

      ui_->statusLabel->setText("Unsaved changes*");
    }

    delete add;
    add = nullptr;

    delete menu;
    menu = nullptr;
  }

  //-----------------------------------------------------------------------------------
  void PackerWidget::OnContentMenuRequested(QPoint point)
  {
    QMenu* menu = new QMenu(ui_->packageTable);
    QAction* remove = nullptr;

    QTreeWidgetItem* item = ui_->packageTable->itemAt(point);
    bool all = false;
    if (item->type() == ContentItem)
    {
      remove = new QAction("Remove", ui_->packageTable);
      all = false;
    }
    else
    {
      remove = new QAction("Remove all in this group", ui_->packageTable);
      all = true;
    }

    menu->addAction(remove);
    menu->popup(ui_->packageTable->viewport()->mapToGlobal(point));

    QAction* selected = menu->exec();

    if (selected != nullptr)
    {
      if (all == true)
      {
        for (int i = item->childCount()-1; i >= 0; --i)
        {
          QTreeWidgetItem* child = item->child(i);

          delete child;
          child = nullptr;
        }
      }
      else
      {
        QList<QTreeWidgetItem*> list = ui_->packageTable->selectedItems();

        for (int i = list.size()-1; i >= 0; --i)
        {
          delete list.at(i);
        }
      }
      ui_->statusLabel->setText("Unsaved changes*");
    }

    delete remove;
    remove = nullptr;

    delete menu;
    menu = nullptr;
  }

  //-----------------------------------------------------------------------------------
  QString PackerWidget::DeduceType(QString path)
  {
    QStringList splitted = path.split(".");
    QString extension = splitted.at(splitted.size() - 1);

    if (extension == "png" || extension == "jpeg" || extension == "jpg" || extension == "dds" || extension == "tga" || extension == "tiff" || extension == "bmp")
    {
      return "texture";
    }
    else if (extension == "fx" || extension == "effect" || extension == "shader")
    {
      return "shader";
    }
    else if (extension == "box" || extension == "pkg" || extension == "package")
    {
      return "box";
    }
    else if (extension == "fbx")
    {
      return "model";
    }
    else if (extension == "ttf")
    {
      return "font";
    }
    else
    {
      return "unknown";
    }
  }

  //-----------------------------------------------------------------------------------
  ContentTypes PackerWidget::GetContentType(QString type)
  {
    if (type == "texture")
    {
      return ContentTypes::kTexture;
    }
    else if (type == "shader")
    {
      return ContentTypes::kShader;
    }
    else if (type == "box")
    {
      return ContentTypes::kBox;
    }
    else if (type == "model")
    {
      return ContentTypes::kModel;
    }
    else if (type == "font")
    {
      return ContentTypes::kFont;
    }
    else
    {
      return ContentTypes::kUnknown;
    }
  }

  //-----------------------------------------------------------------------------------
  void PackerWidget::OnSaveAs()
  {
    QString old = currentSavePath_;

    currentSavePath_ = QFileDialog::getSaveFileName(ui_->centralwidget, "Save file", path_, tr("Snuffbox Package Files (*.box)"), &tr("Snuffbox Package Files (*.box)"));

    if (currentSavePath_ == nullptr)
    {
      currentSavePath_ = old;
      return;
    }

    OnSave();
  }

  //-----------------------------------------------------------------------------------
  void PackerWidget::OnSave()
  {
    toSave_ = "";
    for (int i = 0; i < ui_->packageTable->topLevelItemCount(); ++i)
    {
      QTreeWidgetItem* item = ui_->packageTable->topLevelItem(i);
      for (int j = 0; j < item->childCount(); ++j)
      {
        QTreeWidgetItem* child = item->child(j);
        toSave_ += "load(\"" + child->text(0) + "\",\"" + child->text(1) + "\")\n";
      }
    }

    if (currentSavePath_ == nullptr)
    {
      currentSavePath_ = QFileDialog::getSaveFileName(ui_->centralwidget, "Save file", path_, tr("Snuffbox Package Files (*.box)"), &tr("Snuffbox Package Files (*.box)"));
    }

    if (currentSavePath_ == nullptr)
    {
      return;
    }

    QFile out(currentSavePath_);

    if (out.open(QIODevice::WriteOnly))
    {
      out.write(toSave_.toLatin1());
      out.close();
      ui_->statusLabel->setText("Saved changes!");
    }
    else
    {
      ui_->statusLabel->setText("Error saving changes, is the file open somewhere else?");
    }
  }

  //-----------------------------------------------------------------------------------
  PackerWidget::~PackerWidget()
  {
    delete model_;
    model_ = nullptr;

    delete ui_;
    ui_ = nullptr;
  }
}