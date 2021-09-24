#include "datacollector.h"

DataCollector::DataCollector()
{

}

void DataCollector::add(const QList<QList<float>> &csvData, const float label)
{
    this->m_data += csvData;
    for(int i = 0 ; i < csvData.size(); ++i)
        this->m_labels.push_back(label);
}

bool DataCollector::isEmpty()
{
    return ((this->m_data.isEmpty()));
}

const QList<QList<float> > &DataCollector::data()
{
    return this->m_data;
}

const QList<float> &DataCollector::labels()
{
    return this->m_labels;
}

void DataCollector::clear()
{

    if(!this->m_data.isEmpty()) this->m_data.clear();
    if(!this->m_labels.isEmpty()) this->m_labels.clear();
}

void DataCollector::mix()
{
    //Алгоритм Фишера-Йетса
    for(int i = this->m_data.size() - 1; i > 0; --i)
    {
        int j = 0 + qrand() % (i + 1);
        this->m_data.swap(i, j);
        this->m_labels.swap(i, j);
    }
}
