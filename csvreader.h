#ifndef CSVREADER_H
#define CSVREADER_H

#include <QObject>

class CSVReader
{
public:
    CSVReader();
    ~CSVReader() {}
    bool readCSV(const QString& csvFilePath);
    const QString& cellType();
    const QList<QList<float>> data();
    QList<float> testCsvData(const QString& csvFilePath);
private:
    QString m_cellType;
    QList<QList<float>> m_data;
    const QString getCellTypeFromCSVRow(const QString& CSVRow);
    const QList<float> CSVRowToData(const QString& CSVRow);
    const QString readCellTypeFromCSVRow(const QString& CSVRow);
};

#endif // CSVREADER_H
