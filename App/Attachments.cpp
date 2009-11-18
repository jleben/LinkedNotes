#include <QVBoxLayout>
#include <QGridLayout>
#include <QToolBar>
#include <QAction>
#include <QDialogButtonBox>
#include <QLabel>
#include <QFileDialog>
#include <QPushButton>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QMenu>

#include "../Lib/TagWorld.hpp"

#include "Attachments.hpp"

Attachments::Attachments( TagWorld *w, Item* i ) :
  world( w ), item( i )
{
  attList = new QListWidget();
  
  QToolBar *tools = new QToolBar();
  actions = new QActionGroup(this);
  addAction = actions->addAction( "+" );
  rmAction = actions->addAction( "-" );
  editAction = actions->addAction( "Edit" );
  openAction = actions->addAction( "Open" );
  tools->addActions(actions->actions());
  
  QVBoxLayout *box = new QVBoxLayout();
  box->addWidget(tools);
  box->addWidget(attList);
  box->setContentsMargins(0,0,0,0);
  box->setSpacing(0);
  
  setLayout( box );
  
  attList->setContextMenuPolicy( Qt::CustomContextMenu );
  
  setWindowTitle( "New attachment" );

  connect(addAction, SIGNAL(triggered()), this, SLOT(add()));
  connect(rmAction, SIGNAL(triggered()), this, SLOT(remove()));
  connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));
  connect(attList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
          this, SLOT(edit()));
  connect(attList, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(contextMenu(const QPoint &)));

  setItem( i );
}

void Attachments::setItem( Item* i )
{
  item = i;
  refresh();
}

QList<Attachment> Attachments::attachments()
{
  QList<Attachment> attached;
  int i; int c = attList->count();
  for( i=0; i<c; ++i )
    attached.append( Attachment(
      attList->item(i)->text(),
      attList->item(i)->data( URI_ROLE ).toString() ) );
  return attached;
}

void Attachments::add()
{
  if( !item ) return;

  AttDialog dialog;
  int result = dialog.exec();

  if( result == QDialog::Rejected
      //|| dialog.name().isEmpty()
      || dialog.uri().isEmpty()
    ) return;

  QListWidgetItem *item = new QListWidgetItem( attList );
  item->setText( !dialog.name().isEmpty() ? dialog.name() : dialog.uri() );
  item->setToolTip( dialog.uri() );
  item->setData( URI_ROLE, dialog.uri() );

  this->item->setAttachments( attachments() );
}

void Attachments::remove()
{
  QListWidgetItem *listItem = attList->currentItem();
  if( !listItem || !item ) return;

  delete listItem;

  item->setAttachments( attachments() );
}

void Attachments::edit()
{
  QListWidgetItem *listItem = attList->currentItem();
  if( !listItem ) return;

  AttDialog dialog( Attachment( listItem->text(), listItem->data(URI_ROLE).toString() ) );
  int result = dialog.exec();

  if( result == QDialog::Rejected
      || ( dialog.name().isEmpty() && dialog.uri().isEmpty() )
    ) return;

  listItem->setText( !dialog.name().isEmpty() ? dialog.name() : dialog.uri() );
  listItem->setToolTip( dialog.uri() );
  listItem->setData( URI_ROLE, dialog.uri() );
}

void Attachments::open( )
{
  QString uri = attList->currentItem()->data( URI_ROLE ).toString();
  QDesktopServices::openUrl( QUrl( uri ) );
}

void Attachments::refresh()
{
  attList->clear();
  if(!item) return;
  QList<Attachment> attached = item->attachments();
  foreach( Attachment att, attached ) {
    QListWidgetItem *item = new QListWidgetItem( attList );
    item->setText( !att.name.isEmpty() ? att.name : att.uri );
    item->setToolTip( att.uri );
    item->setData( URI_ROLE, att.uri );
  }
}

void Attachments::contextMenu(const QPoint &pos)
{
  QMenu menu(this);
  menu.setTitle("Attachment"); //FIXME doesn't work
  menu.menuAction()->setText("Attachment"); //FIXME whata hack, this doesn't work either!
  menu.addActions( actions->actions() );
  menu.exec( attList->viewport()->mapToGlobal(pos) );
}

AttDialog::AttDialog( const Attachment& attach )
{
  _name = new QLineEdit( attach.name );
  _uri = new QLineEdit( attach.uri );

  QDialogButtonBox *buttons = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
  QPushButton *btnOpen = new QPushButton();
  btnOpen->setIcon( QApplication::style()->standardIcon( QStyle::SP_DialogOpenButton ) );
  
  QGridLayout *grid = new QGridLayout();
  grid->addWidget( new QLabel("Name:"), 0, 0 );
  grid->addWidget( _name, 0, 1, 1, 2 );
  grid->addWidget( new QLabel("Path:"), 1, 0 );
  grid->addWidget( _uri, 1, 1 );
  grid->addWidget( btnOpen, 1, 2 );
  grid->addWidget( buttons, 2, 0, 1, 3 );
  
  setLayout(grid);
  setWindowTitle("Attachment");
  
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  connect(btnOpen, SIGNAL(clicked()), this, SLOT(open()));
}

void AttDialog::open()
{
  QString filename = QFileDialog::getOpenFileName( this, "Select a file to attach" );
  if( filename.isEmpty() ) return;
  _uri->setText( filename );
}