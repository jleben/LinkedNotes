#include "TagWorld.hpp"

Tag::Tag( TagWorld* w, QString n, Tag* p ) : _world(w), _name(n), _parent(p)  {}

Tag::~Tag() {
  if( _parent ) _parent->_children.removeAll( this );
  emit _world->tagDeleted( this );
}

Tag *Tag::appendSubTag( QString name ) {
  Tag *tag = new Tag( _world, name, this  );
  _children.append( tag );
  emit _world->tagCreated( tag );
  return tag;
}

Tag *Tag::insertSubTag( QString name, int pos ) {
  Tag *tag = new Tag( _world, name, this );
  _children.insert( pos, tag );
  emit _world->tagCreated( tag );
  return tag;
}

void Tag::appendItem( Item* item ) {
  if( !_parent ) return; //don't tagging with root;
  _items.append( item );
  item->_tags.append( this );
  emit _world->itemTagAdded( item, this );
}

void Tag::insertItem( Item* item, int pos ) {
  if( !_parent ) return; //don't tagging with root;
  _items.insert( pos, item );
  item->_tags.append( this );
  emit _world->itemTagAdded( item, this );
}

void Tag::removeItem( Item* item ) {
  _items.removeAll( item );
  item->_tags.removeAll( this );
  emit _world->itemTagRemoved( item, this );
}

Item::Item( TagWorld* w, const QString& n )
      : _world( w), _name(n)
{ }

Item::~Item() {
  foreach( Tag* tag, _tags ) {
    tag->_items.removeAll( this );
  }

  _world->_items.removeAll( this );

  emit _world->itemDeleted( this );
}

void Item::setName( const QString& n )
{
  _name = n;
  emit _world->itemNameChanged( this );
}

TagWorld::TagWorld()
  : _rootTag( new Tag( this, "root", 0 ) )
{ }

Item *TagWorld::addItem( const QString& name ) {
  Item *item = new Item( 0, name ); //don't pass world to not TagWorld::addItem(item)
  item->_world = this;
  _items.append( item );
  emit itemCreated( item );
  return item;
}
