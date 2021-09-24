#include "labelshash.h"

LabelsHash::LabelsHash()
{

}

float LabelsHash::add(const QString &string)
{
    this->m_hash.insert(string, this->m_num);
    return this->m_num++;
}

void LabelsHash::clear()
{
    if(!this->m_hash.isEmpty())
    {
        this->m_hash.clear();
        this->m_num = 0;
    }
}

int LabelsHash::size()
{
    return this->m_hash.size();
}

QString LabelsHash::cellType(float label)
{
    return this->m_hash.key(label);
}

