#include "stringlistmodel.h"


StringListModel::StringListModel(QObject *parent):
    QAbstractListModel(parent)
{

}

int StringListModel::rowCount(const QModelIndex &parent) const
{
    return this->m_stringList.count();
}

QVariant StringListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
             return QVariant();

         if (index.row() >= this->m_stringList.size())
             return QVariant();

         if (role == Qt::DisplayRole)
             return this->m_stringList.at(index.row());
         else
             return QVariant();
}

void StringListModel::setStringList(const QStringList &stringList)
{
    this->m_stringList = stringList;
}

void StringListModel::clear()
{
    this->m_stringList.clear();
}
