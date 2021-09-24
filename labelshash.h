#ifndef LABELSHASH_H
#define LABELSHASH_H
#include <QHash>

//Класс для привязки численного значения к каждому типу клетки
class LabelsHash
{
public:
    LabelsHash();
    float add(const QString& string);
    void clear();
    int size();
    QString cellType(float label);

private:
    QHash<QString, float> m_hash;
    float m_num = 0;
};

#endif // LABELSHASH_H
