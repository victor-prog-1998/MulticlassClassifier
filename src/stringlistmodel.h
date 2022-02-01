#ifndef STRINGLISTMODEL_H
#define STRINGLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>

class StringListModel : public QAbstractListModel
{
public:
    StringListModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    void setStringList(const QStringList& stringList);
    void clear();

private:
    QStringList m_stringList;
};

#endif // STRINGLISTMODEL_H
