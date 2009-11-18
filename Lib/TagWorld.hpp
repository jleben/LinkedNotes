#ifndef TAG_WORLD_H
#define TAG_WORLD_H

#include <QTreeWidget>
#include <QList>

class Tag;
class Item;
class TagWorld;
class Attachment;


class Attachment
{
  public:
    Attachment( QString n = QString(), QString u = QString() )
      : name(n), uri(u) {}
    bool isEmpty() { return (name.isEmpty() && uri.isEmpty()); }
    QString name;
    QString uri;
};

class Tag
{
  friend class TagWorld;
  friend class Item;

  public:
    ~Tag();

    int subTagCount() { return _children.count(); }

    Tag* subTagAt( int pos ) {
      if( pos < _children.count() ) return _children[pos];
      else return 0;
    }

    int indexOfSubTag( Tag* tag ) { return _children.indexOf( tag ); }

    Tag *appendSubTag( QString name );

    Tag *insertSubTag( QString name, int pos );

    void appendItem( Item* item );

    void insertItem( Item* item, int pos );

    void removeItem( Item* item );

    QList<Tag*> children() { return _children; }

    QList<Item*> items() { return _items; }

    QString name() { return _name; }

    Tag *parent() { return _parent; }

    TagWorld *world() { return _world; }

    void setName( QString n ) { _name = n; }

  private:
    Tag( TagWorld* w, QString n, Tag* p );
    TagWorld *_world;
    QList<Tag*> _children;
    QList<Item*> _items;
    QString _name;
    Tag *_parent;

};

class Item
{
  friend class Tag;
  friend class TagWorld;
  public:
    ~Item();
    QString name() { return _name; }
    QString data() { return _data; }
    void setName( const QString& n );
    void setData( const QString& d ) { _data = d; }
    void setAttachments( const QList<Attachment>& att ) { _attached = att; }
    QList<Tag*> tags() { return _tags; }
    QList<Attachment> attachments() { return _attached; }
  private:
    Item( TagWorld* world, const QString& name = QString("Note") );
    TagWorld *_world;
    QString _name;
    QString _data;
    QList<Tag*> _tags;
    QList<Attachment> _attached;
};

class TagWorld : public QObject
{
  Q_OBJECT;
  friend class Item;
  friend class Tag;

  public:
    TagWorld();
    Item *addItem( const QString& name = "New item" );
    Tag *rootTag() { return _rootTag; }
    QList<Item*> items() { return _items; }
  signals:
    void itemTagRemoved( Item*, Tag* );
    void itemTagAdded( Item*, Tag* );
    void tagCreated( Tag* );
    void tagDeleted( Tag* );
    void itemCreated( Item* );
    void itemDeleted( Item* );
    void itemNameChanged( Item* );
  private:
    void sigItemTagRemoved( Item*, Tag* ){}
    void sigTagDeleted( Tag* ){}
    void sigItemDeleted( Item* ){}

    Tag *_rootTag;
    QList<Item*> _items;
};

#endif