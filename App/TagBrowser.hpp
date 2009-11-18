#ifndef TAG_BROWSER_H
#define TAG_BROWSER_H

#include <QListWidget>
#include <QTreeWidgetItem>

class TagWorld;
class Item;
class Tag;
class TagTreeWidget;

class TagBrowser : public QWidget
{
  Q_OBJECT
  public:
    TagBrowser( TagWorld* );
  signals:
    void itemActivated( Item* );
  public slots:
    void setCurrentItem( Item* );
    void showTag( Tag* );
  private slots:
    void addCurrentItem();
    void addNewItem();
    void removeSelectedItems();
    void activateItem( QListWidgetItem* );
    void showAddedItem( Item*, Tag* );
    void showAddedItem( Item* );
    void hideRemovedItem( Item*, Tag* );
    void hideRemovedItem( Item* );
    void refreshItem( Item* );
    void showItemsFor( Tag* );
  private:
    TagWorld *tagWorld;
    TagTreeWidget *tagTree;
    QListWidget *itemList;
    Tag *currentTag;
    Item *currentItem;
};

#endif