#include "gitconf_tree_model.h"

node* GitconfTreeModel::getNode(const QModelIndex& index) const {
    return (node*)index.internalPointer();
}

QVariant GitconfTreeModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) {
        return QVariant();
    }
    auto node = getNode(index);
    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        if(index.column() == 0) {
            if(node->type == node_type::comment) {
                return QVariant("#");
            } else {
                return QVariant(node->caption.c_str());
            }
        } else if(index.column() == 1) {
            return QVariant(node->val.c_str());
        }
    }
    return QVariant();
}

QVariant GitconfTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }
    if(section == 0) {
        return QVariant(u8"name");
    }
    if(section == 1) {
        return QVariant(u8"value");
    }
    return QVariant();
}

QModelIndex GitconfTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if(!parent.isValid()) {
        if(row < rootNode_->children.size()) {
            return createIndex(row, 0, rootNode_->children[row]);
        }
    } else {
        auto ptr = (node*)parent.internalPointer();
        auto child = ptr->children[row];
        return createIndex(row, column, child);
    }
    return QModelIndex();
}

QModelIndex GitconfTreeModel::parent(const QModelIndex &index) const {
    if(!index.isValid()) {
        return QModelIndex();
    }
    auto ptr = (node*)index.internalPointer();
    if(ptr->parent == rootNode_) {
        return QModelIndex();
    }
    return createIndex(ptr->parent->index, 0, ptr->parent);
}

int GitconfTreeModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid()) {
        auto ptr = (node*)parent.internalPointer();
        return ptr->children.size();
    } else {
        return rootNode_->children.size();
    }
    return 0;
}

int GitconfTreeModel::columnCount(const QModelIndex &parent) const {
    return 2;
}

Qt::ItemFlags GitconfTreeModel::flags(const QModelIndex &index) const {
    if(index.isValid()) {
        auto ptr = (node*)index.internalPointer();
        if(ptr->type == node_type::keyvalue) {
            return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
        }
        return QAbstractItemModel::flags(index);
    } else {
        return Qt::NoItemFlags;
    }
}

bool GitconfTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(index.column() != 1) {
        return false;
    }
    auto ptr = (node*)index.internalPointer();
    if(ptr->type == node_type::keyvalue) {
        ptr->val = value.toString().toStdString();
        return true;
    }
    return false;
}

bool GitconfTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) {
    return false;
}

bool GitconfTreeModel::insertColumns(int position, int columns, const QModelIndex &parent) {
    if(parent.isValid()) {
        auto ptr = (node*)parent.internalPointer();
        if(ptr->type == node_type::catagory) {
            auto child = new node(node_type::keyvalue, "");
            ptr->insertChild(position, child);
        }
        else if(ptr->type == node_type::root) {
            auto child = new node(node_type::catagory, "");
            ptr->insertChild(position, child);
        } else {
            return false;
        }
        return true;
    }
    return false;
}

bool GitconfTreeModel::removeColumns(int position, int columns, const QModelIndex &parent) {
    if(!parent.isValid()) {
        return false;
    }
    auto ptr = (node*)parent.internalPointer();
    return ptr->removeChild(position);
}

bool GitconfTreeModel::insertRows(int position, int rows, const QModelIndex &parent) {
    if(rows != 1) {
        return false;
    }
    auto ptr = rootNode_;
    if(parent.isValid()) {
        ptr = (node*)parent.internalPointer();
    }
    beginInsertRows(parent, position, position);
    ptr->insertChild(position, new node(node_type::comment,""));
    endInsertRows();
    return true;
}

bool GitconfTreeModel::removeRows(int position, int rows, const QModelIndex &parent) {
    if(rows != 1) {
        return false;
    }
    auto ptr = rootNode_;
    if(parent.isValid()) {
        ptr = (node*)parent.internalPointer();
    }
    beginRemoveRows(parent, position, position);
    ptr->removeChild(position);
    endRemoveRows();
    return true;
}
