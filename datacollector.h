#ifndef DATACOLLECTOR_H
#define DATACOLLECTOR_H
#include "csvreader.h"
#include <QList>

class DataCollector
{
public:
    DataCollector();
    void add(const QList<QList<float>> &csvData, const float label);
    bool isEmpty();
    const QList<QList<float>>& data();
    const QList<float>& labels();
    void clear();
    void mix();
private:
    QList<QList<float>> m_data;
    QList<float> m_labels;
};

#endif // DATACOLLECTOR_H
