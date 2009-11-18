#include <QApplication>
#include <QTreeView>
#include <QPushButton>
#include <QGridLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QFile>
#include <QToolBar>
#include <QLabel>

#include "../Lib/TagWorld.hpp"
#include "../Lib/TagWorldParser.hpp"

#include "MainInterface.hpp"
#include "TagTreeWidget.hpp"
#include "ItemEditor.hpp"
#include "TagBrowser.hpp"

#include <iostream>
using namespace std;

Q_DECLARE_METATYPE( Tag* );
Q_DECLARE_METATYPE( Item* );

MyApp::MyApp()
  : tags( TagWorldParser::parse( "tags.xml" ) ),
  tagBrowser( new TagBrowser( tags ) ),
  itemEditor( new ItemEditor( tags ) ),
  currentTag( 0 )
{
  QToolBar *tools = new QToolBar();
  tools->setFloatable(false);
  tools->setMovable(false);
  QAction *saveAction = tools->addAction( "Save state" );
  QAction *newItemAction = tools->addAction( "New note" );
  QAction *delItemAction = tools->addAction( "Delete note" );
  QAction *showBrowserAction = tools->addAction( "Show browser" );

  browserDock = new QDockWidget( "My Tag World" );
  browserDock->setWidget( tagBrowser );
  browserDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

  QGridLayout *layout = new QGridLayout();
  layout->addWidget( itemEditor, 0, 0 );

  QWidget *centerWidget = new QWidget();
  centerWidget->setLayout( layout );

  setCentralWidget( centerWidget );
  addDockWidget( Qt::RightDockWidgetArea, browserDock );
  addToolBar( tools );
  
  setWindowTitle("Linked Notes");

  QObject::connect( saveAction, SIGNAL( triggered() ), this, SLOT( save() ) );
  QObject::connect( newItemAction, SIGNAL( triggered() ), this, SLOT( newItem() ) );
  QObject::connect( delItemAction, SIGNAL( triggered() ), this, SLOT( deleteItem() ) );
  QObject::connect( showBrowserAction, SIGNAL( triggered() ), this, SLOT( showBrowser() ) );
  connect( tagBrowser, SIGNAL(itemActivated(Item*)), itemEditor, SLOT(setItem(Item*)) );
  connect( itemEditor, SIGNAL(currentItemChanged(Item*)),
           tagBrowser, SLOT(setCurrentItem(Item*)) );
  connect( itemEditor, SIGNAL(showTag(Tag*)), tagBrowser, SLOT(showTag(Tag*)) );
}

void MyApp::save()
{
  itemEditor->save();
  bool ok = TagWorldParser::save( tags, "tags.xml" );
  if( !ok ) std::cout << "NOOOOO\n";
}

void MyApp::newItem()
{
  Item* item = tags->addItem("Brand New Item");
  if( itemEditor->currentTag() )
    itemEditor->currentTag()->appendItem(item);
  itemEditor->setItem(item);
}

void MyApp::deleteItem()
{
  if( !itemEditor->currentItem() ) return;
  QMessageBox::StandardButton result =
    QMessageBox::warning( this,
                          "Delete note",
                          "Are you sure you want to delete this note?",
                          QMessageBox::Ok | QMessageBox::Cancel,
                          QMessageBox::Cancel );
  if( result == QMessageBox::Ok )
    delete itemEditor->currentItem();
}

void MyApp::showBrowser()
{
  browserDock->show();
}

AddTagDialog::AddTagDialog( TagWorld *w ) :
  tree( new TagTreeWidget(w) ), root( w->rootTag() )
{
  newName = new QLineEdit("New tag");
  QPushButton *btnAdd = new QPushButton("+");
  QPushButton *btnRm = new QPushButton("-");
  //TODO connect( btnAdd, SIGNAL(clicked()), this, SLOT(
  QDialogButtonBox *buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

  QGridLayout *grid = new QGridLayout();
  grid->addWidget( new QLabel( "Select the tag you want to place the item under:" ),
                   0, 0, 1, 3 );
  grid->addWidget( newName, 1, 0 );
  grid->addWidget( btnAdd, 1, 1 );
  grid->addWidget( btnRm, 1, 2 );
  grid->addWidget( tree, 2, 0, 1, 3 );

  grid->addWidget( buttons ,
    3, 0, 1, 3 );

  setLayout( grid );
  
  setWindowTitle( "Add a tag" );

  connect(btnAdd, SIGNAL(clicked()), this, SLOT(add()));
  connect(btnRm, SIGNAL(clicked()), this, SLOT(remove()));
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

Tag* AddTagDialog::tag()
{
  return tree->currentTag();
}

void AddTagDialog::add()
{
  if( newName->text().isEmpty() ) return;
  Tag *tag = tree->currentTag();
  if( !tag ) tag = root;
  Tag *newTag = tag->appendSubTag( newName->text() );
  tree->find( newTag );
}

void AddTagDialog::remove()
{
  delete tree->currentTag();
}

int main(int argc, char*argv[])
{
  QApplication app(argc,argv);

  MyApp myapp;
  myapp.show();

  app.exec();
  return 0;
}


