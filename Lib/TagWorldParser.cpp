#include <QDomDocument>
#include <QFile>

#include "TagWorldParser.hpp"
#include "TagWorld.hpp"

#include <iostream>
using namespace std;

bool TagWorldParser::save( TagWorld* world, const QString& filename )
{
  QDomDocument doc("TagWorld");
  QDomNode docElem = doc.appendChild( doc.createElement("ATagWorld") );
  QMap<Item*, int> map;
  
  
  QList<Item*> items = world->items();
  int c = items.count();
  for( int i=0; i<c; ++i ) {
    Item* item = items[i];
    QDomElement elem = doc.createElement("item");
    elem.setAttribute( "id", i );
    elem.setAttribute( "name", items[i]->name() );

    QList<Attachment> attached = item->attachments();
    foreach( Attachment att, attached ) {
      QDomElement xmlAtt = doc.createElement("attach");
      xmlAtt.setAttribute( "name", att.name );
      xmlAtt.setAttribute( "uri", att.uri );
      elem.appendChild( xmlAtt );
    }

    QDomElement body = doc.createElement("body");
    body.appendChild( doc.createTextNode( items[i]->data() ) );
    elem.appendChild( body );

    docElem.appendChild( elem );

    map.insert( items[i], i );
  }

  Tag* tag = world->rootTag();
  QDomNode curNode = docElem;

  int i = 0;
  while( tag )
  {
    if( i < tag->subTagCount() ) {
      tag = tag->subTagAt(i);
      QDomElement elem = doc.createElement("tag");
      elem.setAttribute( "name", tag->name() );
      curNode.appendChild( elem );
      QList<Item*> items = tag->items();
      foreach( Item* item, items ) {
        QDomElement eItem = doc.createElement("item-ref");
        eItem.setAttribute( "item-id", map[item] );
        elem.appendChild( eItem );
      }
      curNode = elem;
      i = 0;
      continue;
    }
    else {
      if( !tag->parent() ) break;
      i = tag->parent()->indexOfSubTag( tag ) + 1;
      tag = tag->parent();
      curNode = curNode.parentNode();
      continue;
    }
  }

  QFile file (filename);
  if(!file.open(QIODevice::WriteOnly)) {
    //cout << "can't open file!\n";
    return false;
  }
  file.write( doc.toByteArray() );
  file.close();
  return true;
}

TagWorld* TagWorldParser::parse( const QString& filename )
{
  TagWorld* world = new TagWorld();

  QFile file( filename );
  if( !file.open( QIODevice::ReadOnly) ) {
    return world;
  }
  QDomDocument doc("TagWorld");
  if( !doc.setContent( &file ) ) {
    file.close();
    return world;
  }

  QMap<int, Item*> map;

  QDomElement rootElem = doc.documentElement();

  QDomElement elem = rootElem.firstChildElement( "item" );

  while( !elem.isNull() )
  {
    Item *item = world->addItem( elem.attribute( "name" ) );

    QDomElement body = elem.firstChildElement( "body" );
    if( !body.isNull() ) {
      QDomText text = body.firstChild().toText();
      if( !text.isNull() ) item->setData( text.data() );
    }

    QList<Attachment> attached;
    QDomElement uri = elem.firstChildElement( "attach" );
    while( !uri.isNull() ) {
      attached.append( Attachment(uri.attribute("name"), uri.attribute("uri")) );
      uri = uri.nextSiblingElement( "attach" );
    }
    item->setAttachments( attached );

    map.insert( elem.attribute( "id" ).toInt(), item );
    elem = elem.nextSiblingElement( "item" );
  }

  Tag *tag = world->rootTag();
  QDomElement xmlTag = rootElem.firstChildElement( "tag" );

  while( !xmlTag.isNull() )
  {
    Tag *subTag = tag->appendSubTag( xmlTag.attribute( "name" ) );
    QDomElement xmlItemRef = xmlTag.firstChildElement( "item-ref" );
    while( !xmlItemRef.isNull() ) {
      Item *item = map[ xmlItemRef.attribute( "item-id" ).toInt() ];
      if( item ) subTag->appendItem( item );
      xmlItemRef = xmlItemRef.nextSiblingElement( "item-ref" );
    }

    QDomElement xmlSubTag = xmlTag.firstChildElement( "tag" );
    if( !xmlSubTag.isNull() ) {
      xmlTag = xmlSubTag;
      tag = subTag;
      continue;
    }

    xmlTag = xmlTag.nextSiblingElement( "tag" );
    if( !xmlTag.isNull() ) continue;

    xmlTag = xmlTag.parentNode().nextSiblingElement( "tag" );
    tag = tag->parent();
  }

  return world;
}
