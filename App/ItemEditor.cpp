#include <QPushButton>
#include <QLineEdit>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QResizeEvent>
#include <QApplication>
#include <QMenu>

#include "../Lib/TagWorld.hpp"

#include "ItemEditor.hpp"
#include "MainInterface.hpp"
#include "Attachments.hpp"

#include <iostream>
using namespace std;

Q_DECLARE_METATYPE( Tag* );
Q_DECLARE_METATYPE( Item* );

ItemEditor::ItemEditor( TagWorld *w ) :
  tagWorld( w ),
  title( new QLineEdit ),
  textEdit( new QTextEdit ),
  attachments( new Attachments(w) ),
  curItem( 0 )
{
  QFont titleFont( QApplication::font() );
  int size = titleFont.pointSize();
  if( size > -1 )
    titleFont.setPointSize( size + 3 );
  else
    titleFont.setPixelSize( titleFont.pixelSize() + 3 );
  title->setFont( titleFont );
  
  textEdit->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextEditorInteraction);
  
  QToolBar *textToolBar = new QToolBar();
  actBold = textToolBar->addAction( "B" );
  actBold->setCheckable( true );
  actItalic = textToolBar->addAction( "I" );
  actItalic->setCheckable( true );
  QAction *actLink = textToolBar->addAction( "Hyperlink" );

  tagCloud = new TagCloud( tagWorld );

  //LEFT SIDE
  QVBoxLayout *leftLayout = new QVBoxLayout();
  leftLayout->addWidget( textToolBar );
  leftLayout->addWidget( textEdit );
  leftLayout->setSpacing(0);
  leftLayout->setContentsMargins(0,0,0,0);
  QWidget *left = new QWidget();
  left->setLayout( leftLayout );

  QSplitter *splitter = new QSplitter();
  splitter->addWidget( left );
  splitter->addWidget( attachments );
  splitter->setStretchFactor( 0, 1 );
  splitter->setStretchFactor( 1, 0 );
  splitter->setCollapsible( 0, false );
  splitter->setOrientation( Qt::Horizontal );
  
  QVBoxLayout *mainBox = new QVBoxLayout();
  mainBox->addWidget( tagCloud, 0 );
  mainBox->addWidget( title, 0 );
  mainBox->addWidget( splitter, 1 );
  mainBox->setContentsMargins(0,0,0,0);
  //mainBox->setSpacing(0);
  
  setLayout( mainBox );
  //left->setEnabled( false );
  //right->setEnabled( false );
  setEnabled( false );

  connect( textEdit, SIGNAL( currentCharFormatChanged( const QTextCharFormat & ) ),
                    this, SLOT( refreshCharFormat( const QTextCharFormat & ) ) );
  connect( title, SIGNAL( editingFinished () ), this, SLOT( saveItemName() ) );
  connect( actBold, SIGNAL( triggered(bool) ), this, SLOT( setBold(bool) ) );
  connect( actItalic, SIGNAL( triggered(bool) ), this, SLOT( setItalic(bool) ) );
  connect( actLink, SIGNAL( triggered() ), this, SLOT( insertLink() ) );

  connect( tagCloud, SIGNAL(tagActivated(Tag*)), this, SIGNAL(showTag(Tag*)) );
  connect( tagCloud, SIGNAL(itemActivated(Item*)), this, SLOT(setItem(Item*)) );

  connect( tagWorld, SIGNAL(itemTagAdded(Item*,Tag*)),
           this, SLOT(refreshTagAdded(Item*,Tag*)) );
  connect( tagWorld, SIGNAL(itemTagRemoved(Item*,Tag*)),
           this, SLOT(refreshTagRemoved(Item*,Tag*)) );
  connect( tagWorld, SIGNAL(itemDeleted(Item*)),
           this, SLOT(clearDeletedItem(Item*)) );
}

Tag *ItemEditor::currentTag() {return tagCloud->currentTag();}

void ItemEditor::setItem( Item *item )
{
  if( curItem == item ) return;

  if( !item ) {
    title->clear();
    textEdit->clear();
    tagCloud->setItem(0);
    attachments->setItem( 0 );
    setEnabled(false);
    curItem = 0;
    emit currentItemChanged( 0 );
    return;
  }

  save();

  title->setText( item->name() );

  textEdit->document()->setHtml( item->data() );

  tagCloud->setItem( item );

  attachments->setItem( item );

  setEnabled( true );

  curItem = item;

  emit currentItemChanged( item );
}

void ItemEditor::save()
{
  if( curItem ) {
    curItem->setData( textEdit->toHtml() );
    curItem->setName( title->text() );
  }
}

void ItemEditor::setBold( bool bold )
{
  QTextCharFormat format;//textEdit->currentCharFormat() );
  format.setFontWeight( bold ? QFont::Bold : QFont::Normal );
  textEdit->mergeCurrentCharFormat( format );
}

void ItemEditor::setItalic( bool italic )
{
  QTextCharFormat format;// textEdit->currentCharFormat() );
  format.setFontItalic( italic );
  textEdit->mergeCurrentCharFormat( format );
}

void ItemEditor::insertLink()
{
  textEdit->insertHtml( "<a href=\"http://www.google.com\">Google</a>" );
}

void ItemEditor::refreshCharFormat ( const QTextCharFormat & f )
{
  actBold->setChecked( f.fontWeight() == QFont::Bold );
  actItalic->setChecked( f.fontItalic() );
}

void ItemEditor::saveItemName()
{
  if( curItem ) curItem->setName( title->text() );
}

void ItemEditor::refreshTagAdded( Item* item, Tag* tag )
{
  if( item == curItem )
    tagCloud->setItem( item, tag );
}

void ItemEditor::refreshTagRemoved( Item* item, Tag* )
{
  if( item == curItem )
    tagCloud->setItem( item );
}

void ItemEditor::clearDeletedItem( Item* item )
{
  if( item == curItem )
    setItem(0);
}

TagCloud::TagCloud( TagWorld * w, Item* i, Tag* t ) :
  tagWorld( w ),
  mapper( new QSignalMapper(this) ),
  tagDisplay( new QPushButton() ),
  tagMenu( new QMenu(this) ),
  tag( 0 )
{ 
  QToolButton *btnRm = new QToolButton();
  btnRm->setText( "X" );
  btnRm->setAutoRaise( true );
  QToolButton *btnAdd = new QToolButton();
  btnAdd->setIcon( QApplication::style()->standardIcon( QStyle::SP_FileDialogNewFolder ) );
  btnAdd->setAutoRaise( true );
  
  QHBoxLayout *box = new QHBoxLayout();
  box->addWidget( tagDisplay );
  box->addWidget( btnRm );
  box->addWidget( btnAdd );
  box->setContentsMargins(0,0,0,0);
  box->setSpacing(0);
  setLayout(box);
  
  setItem( i, t );
  connect( mapper, SIGNAL(mapped(int)), this, SLOT(tagTriggered(int)) );
  connect( btnRm, SIGNAL(clicked()), this, SLOT( removeTag() ) );
  connect( btnAdd, SIGNAL(clicked()), this, SLOT( addTag() ) );
  connect(tagMenu, SIGNAL(triggered(QAction*)), this, SLOT(parseAction(QAction*)) );
}

void TagCloud::setItem( Item* i, Tag* t )
{
  item = i;

  tagMenu->clear();
  tagDisplay->setText(QString());
  tagDisplay->setMenu(0);

  if( !item ) return;

  tags = item->tags();
  if( !tags.isEmpty() ) {
    if( t == 0 ) t = tag;
    if( tags.contains(t) ) tag = t;
    else( tag = tags[0] );

    tagDisplay->setText( tag->name() );
    tagDisplay->setMenu( tagMenu );

    //tagMenu->addAction("This tag's other items")->setSeparator(true);
    //addSeparator()->setText("This tag's other items");
    QList<Item*> items = tag->items();
    foreach( Item* item, items ) {
      QAction *action = tagMenu->addAction( item->name() );
      action->setData( QVariant::fromValue(item) );
      if( item == this->item ) {
        QFont font;
        font.setBold( true );
        action->setFont( font );
      }
    }

    //tagMenu->addSeparator()->setText("This item's other tags");
    tagMenu->addAction("This tag's other items")->setSeparator(true);
    //QMenu* otherTags = tagMenu->addMenu( "Other tags" );
    foreach( Tag* t, tags ) {
      QAction *action = tagMenu->addAction( t->name() );
      action->setData( QVariant::fromValue(t) );
      if( t == tag ) {
        QFont font;
        font.setBold( true );
        action->setFont( font );
      }
    }

    /*tagMenu->addSeparator();
    QAction *rmAction = tagMenu->addAction( "Remove this tag" );
    rmAction->setData( QVariant::fromValue( tag ) );*/
  }
  else tag = 0;

  /*QAction *actAdd = tagMenu->addAction(
    QApplication::style()->standardIcon( QStyle::SP_FileDialogNewFolder ), "Add tag..." );
  connect( actAdd, SIGNAL(triggered()), this, SLOT(addTag()) );*/
}

void TagCloud::parseAction( QAction* action )
{
  Tag *tag = action->data().value<Tag*>();
  if( tag ) {
    setItem( this->item, tag );
    return;
  }
  Item *item = action->data().value<Item*>();
  if( item ) emit itemActivated( item );
}

void TagCloud::addTag()
{
  AddTagDialog dialog( tagWorld );
  int res = dialog.exec();
  if( res == QDialog::Rejected || !dialog.tag() ) return;
  if( item ) dialog.tag()->appendItem( item );
}

void TagCloud::removeTag()
{
  if( item && tag ) tag->removeItem( item );
}

TagCloud2::TagCloud2( TagWorld* w, Item* i ) :
  tagWorld( w )
{
  btnLeft = new QToolButton();
  btnLeft->setText("<");
  btnLeft->setAutoRaise( true );
  btnRight = new QToolButton();
  btnRight->setText(">");
  btnRight->setAutoRaise( true );
  QToolButton *btnAdd = new QToolButton();
  btnAdd->setText("+");
  btnAdd->setAutoRaise( true );

  tagsBar = new QToolBar();
  tagsBar->addAction( "foo" );
  tagsBar->addAction( "foo" );
  tagWrap = new QWidget();
  tagsBar->setParent( tagWrap );
  tagWrap->setMinimumHeight( tagsBar->sizeHint().height() );

  QHBoxLayout *box = new QHBoxLayout();
  box->addWidget( btnLeft, 0 );
  box->addWidget( tagWrap, 1 );
  box->addWidget( btnRight, 0 );
  box->addWidget( btnAdd, 0 );
  box->setContentsMargins(0,0,0,0);

  setLayout( box );

  connect( btnAdd, SIGNAL(clicked()), this, SLOT(addTag()) );

  setItem( i );
}

void TagCloud2::setItem( Item* i )
{
  item = i;
  tagsBar->clear();
  if( !item ) return;
  QList<Tag*> tags = item->tags();
  foreach( Tag* tag, tags ) {
    tagsBar->addAction( tag->name() );
  }
  tagsBar->resize( tagsBar->sizeHint() );
}

void TagCloud2::addTag()
{
  AddTagDialog dialog( tagWorld );
  int res = dialog.exec();
  if( res == QDialog::Rejected || !dialog.tag() ) return;
  if( item ) dialog.tag()->appendItem( item );
}

/*QSize TagCloud2::sizeHint () const
{
  return tags->sizeHint();
}

QSize TagCloud2::minimumSizeHint () const
{
  QSize size;
  size.setHeight( qMax( tags->sizeHint().height(), btnLeft->sizeHint().height() ) );
  size.setWidth( btnLeft->sizeHint().width() + btnRight->sizeHint().width() );
  return size;
}

void TagCloud2::resizeEvent ( QResizeEvent * event )
{
  if( event->size().width() < tags->sizeHint().width() ) {
    btnLeft->show();
    btnRight->show();
    btnRight->move( event->size().width() - btnRight->size().width(), 0 );
  }
  else {
    btnLeft->hide();
    btnRight->hide();
  }
}*/
