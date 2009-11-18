#ifndef TAG_TREE_WIDGET_H
#define TAG_TREE_WIDGET_H

#include <QTreeWidget>

class TagWorld;
class Tag;

enum TagTreeItemRole {
  TAG_ROLE = Qt::UserRole,
  STATUS_ROLE
};

enum TagTreeItemStatus {
  ALL_ITEMS_STATUS,
  TAGS_ROOT_STATUS
};

class TagTreeWidget : public QTreeWidget
{
  Q_OBJECT;
  public:
    TagTreeWidget( TagWorld * = 0 );
    void setTagWorld( TagWorld * );
    bool find( Tag* );
    Tag* currentTag();
  signals:
    void tagActivated( Tag* ); //Tag* == 0 for all items!
  private slots:
    void fillNew( Tag* );
    void removeDeleted( Tag* );
    void activate( QTreeWidgetItem*, int );
  private:
    void fillChildren( QTreeWidgetItem*, Tag* );
    QTreeWidgetItem* findChild( QTreeWidgetItem*, Tag* );
    Tag* tagFromItem( QTreeWidgetItem* );
    TagWorld *_world;
};

#endif