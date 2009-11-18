#ifndef ATTACHMENTS_H
#define ATTACHMENTS_H

#include <QListWidget>
#include <QLineEdit>
#include <QDialog>
#include <QAction>
#include "../Lib/TagWorld.hpp"

class TagWorld;
class Item;

enum AttachmentRole {
  URI_ROLE = Qt::UserRole
};

class Attachments : public QWidget
{
  Q_OBJECT;
  public:
    Attachments( TagWorld*, Item* = 0);
    void setItem( Item* );
    QList<Attachment> attachments();
  private slots:
    void add();
    void remove();
    void edit();
    void open();
    void contextMenu ( const QPoint & pos );
  private:
    void refresh();
    TagWorld* world;
    Item* item;
    QListWidget *attList;
    QActionGroup *actions;
    QAction *addAction;
    QAction *rmAction;
    QAction *openAction;
    QAction *editAction;
};

class AttDialog : public QDialog
{
  Q_OBJECT;
  public:
    AttDialog( const Attachment& = Attachment() );
    QString name() { return _name->text(); }
    QString uri() { return _uri->text(); }
  private slots:
    void open();
  private:
    QLineEdit *_name;
    QLineEdit *_uri;
};

#endif