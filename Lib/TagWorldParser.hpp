#ifndef TAG_WORLD_PARSER_H
#define TAG_WORLD_PARSER_H

#include <QString>

class TagWorld;

class TagWorldParser
{
  public:
    static TagWorld *parse( const QString& filename );
    static bool save( TagWorld*, const QString& filename );
};

#endif