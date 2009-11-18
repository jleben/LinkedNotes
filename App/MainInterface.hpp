#ifndef MAIN_INTERFACE_H
#define MAIN_INTERFACE_H

#include <QAbstractItemModel>
#include <QTreeView>
#include <QTreeWidgetItem>
#include <QDialog>
#include <QMainWindow>
#include <QDockWidget>
#include <QLineEdit>

class TagWorld;
class Tag;
class Item;
class TagTreeWidget;
class ItemEditor;
class TagBrowser;

enum ListItemRole {
  TAGGED_ITEM = Qt::UserRole
};

class MyApp : public QMainWindow
{
  Q_OBJECT;

  public:
    MyApp();
    void showItemsFor( Tag* );
  public slots:
    void save();
    void newItem();
    void deleteItem();
    void showBrowser();
  private:
    TagWorld *tags;
    TagBrowser *tagBrowser;
    ItemEditor *itemEditor;
    QDockWidget *browserDock;
    Tag* currentTag;
};

class AddTagDialog : public QDialog
{
  Q_OBJECT
  public:
    AddTagDialog( TagWorld * );
    Tag *tag();
  private slots:
    void add();
    void remove();
  private:
    TagTreeWidget *tree;
    QLineEdit *newName;
    Tag *root;
};

#endif