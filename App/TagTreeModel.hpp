#ifndef TAG_TREE_MODEL_H
#define TAG_TREE_MODEL_H

#include <QAbstractItemModel>

class TagWorld;
class Tag;

class TagTreeModel : public QAbstractItemModel
{
  Q_OBJECT;

  public:
    TagTreeModel( TagWorld *world );
    void createTag( QString name, Tag* parent );
    void destroyTag( Tag* );
    QList<Tag*> selectedTags();
    /*TODO inline (doesn't compile???)*/ Tag* tagFromIndex( const QModelIndex & ) const;
  public slots:
    void destroySelectedTags() {}
  private:
    QModelIndex index ( int, int, const QModelIndex & ) const;
    QModelIndex parent ( const QModelIndex & index ) const;
    QVariant data ( const QModelIndex &, int ) const;
    int columnCount ( const QModelIndex & ) const;
    int rowCount ( const QModelIndex & ) const;

    QModelIndex index ( Tag * ) const;

    TagWorld *_world;
};

#endif