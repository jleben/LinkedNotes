#include <QPushButton>
#include <QGridLayout>

#include "../Lib/TagWorld.hpp"

#include "TagBrowser.hpp"
#include "TagTreeWidget.hpp"

Q_DECLARE_METATYPE( Item* );
Q_DECLARE_METATYPE( Tag* );

TagBrowser::TagBrowser( TagWorld *w ) :
  tagWorld( w ),
  tagTree( new TagTreeWidget( w ) ),
  itemList( new QListWidget ),
  currentTag(0),
  currentItem(0)
{
  itemList->setAlternatingRowColors( true );

  QPushButton *btnAddItem = new QPushButton( "+curI" );
  QPushButton *btnNewItem = new QPushButton( "+newI" );
  QPushButton *btnRmItem = new QPushButton( "-I" );
  QPushButton *btnAddTag = new QPushButton( "+tag");
  QPushButton *btnRmTag = new QPushButton( "-tag");

  QGridLayout *grid = new QGridLayout();
  grid->addWidget( tagTree, 0, 0, 1, 5 );
  grid->addWidget( itemList, 1, 0, 1, 5 );
  /*grid->addWidget( btnAddItem, 2, 0 );
  grid->addWidget( btnNewItem, 2, 1 );
  grid->addWidget( btnRmItem, 2, 2 );
  grid->addWidget( btnAddTag, 2, 3 );
  grid->addWidget( btnRmTag, 2, 4 );*/
  
  setLayout( grid );

  QObject::connect( btnAddItem, SIGNAL( clicked() ), this, SLOT( addCurrentItem() ) );
  QObject::connect( btnNewItem, SIGNAL( clicked() ), this, SLOT( addNewItem() ) );
  QObject::connect( btnRmItem, SIGNAL( clicked() ), this, SLOT( removeSelectedItems() ) );
  QObject::connect( tagTree, SIGNAL( tagActivated( Tag* ) ),
                    this, SLOT( showItemsFor( Tag* ) ) );
  QObject::connect( itemList, SIGNAL( itemActivated( QListWidgetItem* ) ),
                    this, SLOT( activateItem( QListWidgetItem* ) ) );
  connect( tagWorld, SIGNAL( itemTagAdded( Item*, Tag* ) ),
           this, SLOT( showAddedItem( Item*, Tag* ) ) );
  connect( tagWorld, SIGNAL (itemTagRemoved( Item*, Tag* ) ),
           this, SLOT( hideRemovedItem( Item*, Tag* ) ) );
  connect( tagWorld, SIGNAL (itemCreated( Item* ) ),
           this, SLOT( showAddedItem( Item* ) ) );
  connect( tagWorld, SIGNAL (itemDeleted( Item* ) ),
           this, SLOT( hideRemovedItem( Item* ) ) );
  connect( tagWorld, SIGNAL (itemNameChanged( Item* ) ),
           this, SLOT( refreshItem( Item* ) ) );
}

void TagBrowser::setCurrentItem( Item* item )
{
  currentItem = item;
}

void TagBrowser::showTag( Tag* tag )
{
  if( tagTree->find( tag ) )
    showItemsFor( tag );
}

void TagBrowser::addCurrentItem()
{
  if( !currentItem || !currentTag ) return;
  currentTag->appendItem( currentItem );
}

void TagBrowser::removeSelectedItems()
{
  /*foreach( QListWidgetItem* listItem, selected ) {
    Item *item = listItem->data( Qt::UserRole ).value<Item*>();
    if( item && currentTag ) currentTag->removeItem( item );
  } TODO: should probably use QEvent for callbacks to remove all selected!*/

  QListWidgetItem* selected = itemList->currentItem();
  if( selected ) {
    Item *item = selected->data( Qt::UserRole ).value<Item*>();
    if( item && currentTag ) currentTag->removeItem( item );
  }
}

void TagBrowser::addNewItem()
{
  if( !currentTag ) return;
  Item *item = tagWorld->addItem();
  currentTag->appendItem( item );
  emit itemActivated( item );
}

#include <iostream>
void TagBrowser::showItemsFor( Tag *tag )
{
  itemList->clear();
  QList<Item*> items = tag == tagWorld->rootTag() ? tagWorld->items() : tag->items();
  foreach( Item* item, items ) {
    QListWidgetItem *listItem = new QListWidgetItem( item->name() );
    listItem->setData( Qt::UserRole, QVariant::fromValue( item ) );
    itemList->addItem( listItem );
  }
  currentTag = tag;
}

void TagBrowser::activateItem( QListWidgetItem* listItem )
{
  Item *item = listItem->data( Qt::UserRole ).value<Item*>();
  if( item ) emit itemActivated( item );
}

void TagBrowser::showAddedItem( Item* item, Tag* tag )
{
  if( currentTag == tagWorld->rootTag() || tag != currentTag ) return;
  QListWidgetItem *listItem = new QListWidgetItem( item->name() );
  listItem->setData( Qt::UserRole, QVariant::fromValue( item ) );
  itemList->addItem( listItem );
}

void TagBrowser::showAddedItem( Item* item )
{
  if( currentTag != tagWorld->rootTag() && !item->tags().contains(currentTag) ) return;
  QListWidgetItem *listItem = new QListWidgetItem( item->name(), itemList );
  listItem->setData( Qt::UserRole, QVariant::fromValue( item ) );
}

void TagBrowser::hideRemovedItem( Item* item, Tag* tag )
{
  if( currentTag == tagWorld->rootTag() || tag != currentTag ) return;
  hideRemovedItem( item );
}

void TagBrowser::hideRemovedItem( Item* item )
{
  int c = itemList->count();
  for( int i = 0; i<c; ++i ) {
    if( itemList->item(i)->data( Qt::UserRole ).value<Item*>() == item ) {
      delete itemList->item(i);
      return;
    }
  }
}
void TagBrowser::refreshItem( Item* item )
{
  int c = itemList->count();
  for( int i = 0; i<c; ++i ) {
    if( itemList->item(i)->data( Qt::UserRole ).value<Item*>() == item ) {
      itemList->item(i)->setText( item->name() );
      return;
    }
  }
}
