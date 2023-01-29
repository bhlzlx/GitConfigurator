#pragma once

#include <qabstractitemmodel>
#include <qmodelindex>
#include <qvariant>
#include "gitconf_parser.h"

class GitconfTreeModel : public QAbstractItemModel {
    Q_OBJECT
public:
   Q_SIGNALS:
        void userAddData(bool addData);
        void changePointerData(int32_t pointer) const;
        void getFindNameIndex(const QModelIndex index, bool isChange = false) const;
private:
    node* rootNode_;
public:
    GitconfTreeModel(decltype(rootNode_) data, QObject* parent = nullptr)
        : QAbstractItemModel(parent)
        , rootNode_(data)
    {
    }

    ~GitconfTreeModel() {}

    node* getNode(const QModelIndex& index) const;
    QVariant data( const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;

};