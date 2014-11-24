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
    connect(ui_->actionOpen, SIGNAL(triggered()), this, SLOT(OnOpen()));
    connect(ui_->actionNew, SIGNAL(triggered()), this, SLOT(OnNew()));
    connect(ui_->actionExit, SIGNAL(triggered()), this, SLOT(OnQuit()));

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
  void PackerWidget::OnQuit()
  {
    exit(0);
  }
  
  //-----------------------------------------------------------------------------------
  void PackerWidget::OnNew()
  {
    for (int i = ui_->packageTable->topLevelItemCount()-1; i >= 0; --i)
    {
      QTreeWidgetItem* item = ui_->packageTable->topLevelItem(i);

      for (int j = item->childCount() - 1; j >= 0; --j)
      {
        QTreeWidgetItem* toDelete = item->child(j);

        delete toDelete;
        toDelete = nullptr;
      }
    }

    currentSavePath_ = nullptr;
  }

  //-----------------------------------------------------------------------------------
  void PackerWidget::OnOpen()
  {
    currentSavePath_ = QFileDialog::getOpenFileName(ui_->centralwidget, "Open file", path_, tr("Snuffbox Package Files (*.box)"), &tr("Snuffbox Package Files (*.box)"));

    if (currentSavePath_ == nullptr)
    {
      return;
    }

    bool failed = false;
    std::string contentPath = currentSavePath_.toStdString();
    Package result = ParsePackage(contentPath, &failed);

    if (failed == true)
    {
      currentSavePath_ = nullptr;
      return;
    }

    for (auto it : result)
    {
      for (auto path : it.second)
      {
        QTreeWidgetItem* item = new QTreeWidgetItem(ContentItem);
        ContentTypes contentType = GetContentType(it.first.c_str());

        QTreeWidgetItem* parent = nullptr;
        for (int i = 0; i < ui_->packageTable->topLevelItemCount(); ++i)
        {
          parent = ui_->packageTable->topLevelItem(i);

          if (contentType == parent->type())
          {
            break;
          }
        }

        item->setText(0, it.first.c_str());
        item->setText(1, path.c_str());

        item->setTextColor(0, QColor(159, 164, 98));
        item->setTextColor(1, QColor(200, 200, 200));

        parent->addChild(item);
      }
    }
  }

  //-----------------------------------------------------------------------------------
  Package PackerWidget::ParsePackage(std::string& contentPath, bool* failed)
  {
    *failed = false;
    std::fstream content(contentPath);

    if (content)
    {
      std::string package;
      char ch;

      while (content >> std::noskipws >> ch)
      {
        package += ch;
      }

      std::string current = "";
      std::string expected = "load";

      Package parsedValues;
      bool checkType = true;

      for (int i = 0; i < package.length(); ++i)
      {
        ch = package.at(i);

        SkipWhiteSpaces(i, package, ch);

        if (i >= package.length())
        {
          content.close();
          return parsedValues;
        }

        bool result = Consume(i, package, expected);

        if (result == false)
        {
          return Package();
        }
        else if (expected == "load")
        {
          expected = "(";
        }
        else if (expected == "(")
        {
          expected = "\"";
        }
        else if (expected == "\"")
        {
          ++i;
          std::string val = GetValue(i, package, "\"");

          if (checkType == false)
          {
            return Package();
          }
          else
          {
            SkipWhiteSpaces(i, package, ch);
            ++i;
            result = Consume(i, package, ",");

            if (result == false)
            {
              return Package();
            }
            else
            {
              SkipWhiteSpaces(i, package, ch);
              ++i;
              result = Consume(i, package, "\"");

              if (result == false)
              {
                return Package();
              }
              else
              {
                ++i;
                std::string val2 = GetValue(i, package, "\"");
                auto it = parsedValues.find(val);

                if (it == parsedValues.end())
                {
                  std::vector<std::string> vec;
                  vec.push_back(val2);
                  parsedValues.emplace(val, vec);
                }
                else
                {
                  it->second.push_back(val2);
                }
                ++i;

                bool result = Consume(i, package, ")");

                if (result == false)
                {
                  return Package();
                }
                else
                {
                  expected = "load";
                }
              }
            }
          }
        }
      }
    }
    else
    {
      QMessageBox* mb = new QMessageBox(QMessageBox::Icon::Critical, "Failed opening box file", "Could not open the box file for reading, is the file in use?");
      mb->exec();

      delete mb;
      mb = nullptr;

      *failed = true;
    }

    content.close();

    return Package();
  }

  //-----------------------------------------------------------------------------------
  void PackerWidget::SkipWhiteSpaces(int& i, std::string& str, char& curChar)
  {
    char ch = str.at(i);

    while (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t')
    {
      ++i;
      curChar = ch;

      if (i >= str.length())
      {
        break;
      }

      ch = str.at(i);
    }
  }

  //-----------------------------------------------------------------------------------
  bool PackerWidget::Consume(int& i, std::string& str, std::string toConsume)
  {
    int idx = 0;
    std::string result = "";
    char ch = str.at(i);

    while (i < str.length() && idx < toConsume.length() && str.at(i) == toConsume.at(idx))
    {
      result += str.at(i);
      ++idx;
      ++i;
    }

    if (result != toConsume)
    {
      result += str.at(i);
      QMessageBox* mb = new QMessageBox(QMessageBox::Icon::Critical, "Failed parsing box file", QString("Expected ' " + QString(toConsume.c_str()) + " ' but found ' " + QString(result.c_str()) + " '"));

      mb->exec();

      delete mb;
      mb = nullptr;

      return false;
    }

    --i;
    return true;
  }

  //-----------------------------------------------------------------------------------
  std::string PackerWidget::GetValue(int& i, std::string& str, std::string endAt)
  {
    char ch = str.at(i);
    std::string val = "";

    while (ch != endAt.at(0))
    {
      val += ch;
      ++i;
      ch = str.at(i);
    }

    return val;
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