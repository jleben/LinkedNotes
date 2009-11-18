#include <QHeaderView>

#include "../Lib/TagWorld.hpp"

#include "TagTreeWidget.hpp"

Q_DECLARE_METATYPE( Tag* );

#include <iostream>

TagTreeWidget::TagTreeWidget( TagWorld *w ) :
  _world(0)
{
  header()->hide();
  setTagWorld( w );
}

void TagTreeWidget::setTagWorld( TagWorld *w )
{
  if( !w || _world == w ) return;
  if( _world ) QObject::disconnect( _world, 0, this, 0 );
  _world = w;
  connect( _world, SIGNAL(tagCreated( Tag* )), this, SLOT(fillNew(Tag*)) );
  connect( _world, SIGNAL(tagDeleted( Tag* )), this, SLOT(removeDeleted(Tag*)) );

  clear();

  QTreeWidgetItem *tagRoot = new QTreeWidgetItem( this );
  tagRoot->setText( 0, "All items" );
  tagRoot->setData( 0, STATUS_ROLE, TAGS_ROOT_STATUS );
  tagRoot->setData( 0, TAG_ROLE, QVariant::fromValue( _world->rootTag() ) );

  fillChildren( tagRoot, _world->rootTag() );

  tagRoot->setExpanded( true );

  connect( this, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ),
           this, SLOT( activate( QTreeWidgetItem*, int ) ) );
  connect( this, SIGNAL( itemActivated( QTreeWidgetItem*, int ) ),
           this, SLOT( activate( QTreeWidgetItem*, int ) ) );
}

bool TagTreeWidget::find( Tag* tag )
{
  QTreeWidgetItem *item = findChild( invisibleRootItem(), tag );
  //if( item ) setCurrentItem( item, 0, QItemSelectionModel::SelectCurrent );
  if( item ) {
    setCurrentItem( item );
    scrollToItem( item );
    return true;
  }
  return false;
}

void TagTreeWidget::fillNew( Tag* tag )
{
  QTreeWidgetItem *parent = tag->parent() ?
    findChild( invisibleRootItem(), tag->parent() ) :
    invisibleRootItem();
  if( !parent ) return;

  QTreeWidgetItem *item = new QTreeWidgetItem( parent );
  item->setText( 0, tag->name() );
  item->setData( 0, TAG_ROLE, QVariant::fromValue( tag ) );
}

void TagTreeWidget::removeDeleted( Tag* tag )
{
  delete findChild( invisibleRootItem(), tag );
}

Tag *TagTreeWidget::currentTag()
{
  if( currentItem() )
    return currentItem()->data( 0, TAG_ROLE ).value<Tag*>();
  else
    return 0;
}

Tag* TagTreeWidget::tagFromItem( QTreeWidgetItem* item )
{ return item->data( 0, TAG_ROLE ).value<Tag*>(); }

void TagTreeWidget::fillChildren( QTreeWidgetItem* pItem, Tag* pTag )
{
  int i = 0; int c = pTag->subTagCount();
  for(; i<c; ++i) {
    Tag *tag = pTag->subTagAt(i);
    QTreeWidgetItem *item = new QTreeWidgetItem( pItem );
    item->setText( 0, tag->name() );
    item->setData( 0, TAG_ROLE, QVariant::fromValue( tag ) );
    fillChildren( item, tag );
  }
}

QTreeWidgetItem* TagTreeWidget::findChild( QTreeWidgetItem* parent, Tag* tag )
{
  int c = parent->childCount();
  for( int i = 0; i < c; ++i ) {
    QTreeWidgetItem* item = parent->child(i);
    if( item->data( 0, TAG_ROLE ).value<Tag*>() == tag ) return item;

    item = findChild( item, tag );
    if( item ) return item;
  }
  return 0;
}

void TagTreeWidget::activate( QTreeWidgetItem* item, int )
{
  Tag *tag = item->data(0, TAG_ROLE).value<Tag*>();
  emit tagActivated( tag );
}
