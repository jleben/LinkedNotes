#ifndef ITEM_EDITOR_H
#define ITEM_EDITOR_H

#include <QTextEdit>
#include <QComboBox>
#include <QSplitter>
#include <QScrollArea>
#include <QToolBar>
#include <QPushButton>
#include <QToolButton>
#include <QSignalMapper>
#include <QMenu>

class Item;
class Tag;
class TagWorld;
class TagCloud2;
class TagCloud;
class Attachments;

class ItemEditor : public QWidget
{
  Q_OBJECT;

  public:
    ItemEditor( TagWorld * );
    void addTag( Tag * );
    Tag *currentTag();
    Item *currentItem() {return curItem;}
  public slots:
    void setItem( Item * );
    void save();
  private slots:
    void setBold( bool );
    void setItalic( bool );
    void insertLink();
    void refreshCharFormat ( const QTextCharFormat & );
    void saveItemName();
    void refreshTagAdded( Item *, Tag * );
    void refreshTagRemoved( Item *, Tag * );
    void clearDeletedItem(Item*);
  signals:
    void currentItemChanged( Item* );
    void showTag( Tag* );
  private:
    TagWorld *tagWorld;
    QLineEdit *title;
    QTextEdit *textEdit;
    Attachments *attachments;
    TagCloud *tagCloud;
    Item *curItem;
    QAction *actBold;
    QAction *actItalic;
};

class TagCloud : public QWidget
{
  Q_OBJECT
  public:
    TagCloud( TagWorld*, Item* = 0, Tag* = 0 );
    void setItem( Item*, Tag* = 0 );
    Tag *currentTag() {return tag;}
  signals:
    void itemActivated( Item* );
  private slots:
    void addTag();
    void removeTag();
    //void tagTriggered(int i) {emit tagActivated(tags[i]);}
    void parseAction( QAction* );
  private:
    TagWorld *tagWorld;
    Item* item;
    QList<Tag*> tags;
    QSignalMapper *mapper;
    QPushButton *tagDisplay;
    QMenu *tagMenu;
    Tag *tag;
};

class TagCloud2 : public QWidget
{
  Q_OBJECT;
  public:
    TagCloud2( TagWorld*, Item* = 0 );
    void setItem( Item* );
  signals:
    void tagActivated( Tag* );
  private slots:
    void addTag();
  private:
    //QSize sizeHint () const;
    //QSize minimumSizeHint () const;
    //void resizeEvent ( QResizeEvent * event );
    QToolBar* tagsBar;
    QToolButton *btnLeft;
    QToolButton *btnRight;
    TagWorld *tagWorld;
    Item *item;
    QWidget *tagWrap;
};
#endif
