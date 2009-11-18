#include "../Lib/TagWorld.hpp"

#include "TagTreeModel.hpp"

TagTreeModel::TagTreeModel( TagWorld *w )
  : _world( w )
{}

QModelIndex TagTreeModel::index ( int row, int column, const QModelIndex & parent ) const
{
  return createIndex( row, column, tagFromIndex( parent )->subTagAt( row ) );
}

QModelIndex TagTreeModel::parent ( const QModelIndex & index ) const
{
  Tag *tag = tagFromIndex( index );
  return ( tag->parent() ? TagTreeModel::index( tag->parent() ) : QModelIndex() );
}

QVariant TagTreeModel::data ( const QModelIndex & index, int role ) const
{
  if( role != Qt::DisplayRole ) return QVariant();

  return QVariant( tagFromIndex( index )->name() );
}

int TagTreeModel::columnCount ( const QModelIndex & ) const
{
  return 1;
}

int TagTreeModel::rowCount ( const QModelIndex & parent ) const
{
  return tagFromIndex( parent )->subTagCount();
}

Tag* TagTreeModel::tagFromIndex( const QModelIndex &index ) const
{
  Tag *tag = index.isValid() ?
    static_cast<Tag*>( index.internalPointer() ) :
    _world->rootTag();

  return tag;
}

QModelIndex TagTreeModel::index ( Tag *tag ) const
{
  if( !tag->parent() ) return QModelIndex();

  int row = tag->parent()->indexOfSubTag( tag );
  return createIndex( row, 0, tag );
}

void TagTreeModel::createTag( QString name, Tag* parent )
{
  if( !parent ) parent = _world->rootTag();
  
  int i = parent->subTagCount();
  beginInsertRows( index( parent ), i, i  );
  parent->appendSubTag( name );
  endInsertRows();
}

void TagTreeModel::destroyTag( Tag* tag)
{
  int i = tag->parent()->indexOfSubTag( tag );
  beginRemoveRows( index( tag->parent() ), i, i );
  delete tag;
  endRemoveRows();
}

QList<Tag*> TagTreeModel::selectedTags()
{return QList<Tag*>();}
