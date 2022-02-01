#include "csvreader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
CSVReader::CSVReader()
{

}

bool CSVReader::readCSV(const QString &csvFilePath)
{
    QFile file(csvFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream stream(&file);
    stream.setCodec("Windows-1251");
    stream.readLine(); //пропуск названий колонок
    QString CSVRow;
    if(!this->m_data.isEmpty()) this->m_data.clear();
    int counter = 0; //в начале цикла - текущая считываемая строка таблицы, в конце - число считанных строк
    CSVRow = stream.readLine();
    while(!CSVRow.isEmpty()) //строка будет пустой, когда дошли до конца файла
    {
        if(counter == 0) //тип клетки есть смысл считывать только один раз
            this->m_cellType = getCellTypeFromCSVRow(CSVRow);
        this->m_data.push_back(CSVRowToData(CSVRow));
        ++counter;
        CSVRow = stream.readLine();
    }
    file.close();
    return true;
}

const QString &CSVReader::cellType()
{
    return this->m_cellType;
}

const QList<QList<float> > CSVReader::data()
{
    return this->m_data;
}

QList<float> CSVReader::testCsvData(const QString &csvFilePath)
{
    QFile file(csvFilePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << "Ошибка чтения файла";

    QTextStream stream(&file);
    stream.setCodec("Windows-1251");
    stream.readLine(); //пропуск названий колонок

    QString CSVRow;
    CSVRow = stream.readLine();
    file.close();
    return this->CSVRowToData(CSVRow);
}

const QString CSVReader::getCellTypeFromCSVRow(const QString &CSVRow)
{
//I:\testprog\4show\Data\sample\B-ALL\primary\Metamyelocyte\4524-Метамиелоцит-{29c6a7c2-762b-44c0-856f-2c7cb9994dcf}.bmp
   QString imagePath = CSVRow.split(';')[0];
   QStringList strLst = imagePath.split('\\');
   QString str = strLst.last(); //4524-Метамиелоцит-{29c6a7c2-762b-44c0-856f-2c7cb9994dcf}.bmp
   int pos = 0;
   while(str[pos] != '-')
       ++pos;
   ++pos;
   QString name;
   while(str[pos] != '-')
   {
       name += str[pos];
       ++pos;
   }
   return name;
}

const QList<float> CSVReader::CSVRowToData(const QString &CSVRow)
{
    QStringList strList = CSVRow.split(';');
    strList.pop_front(); // удаляем из списка строку с путем к изображению
    QList<float> data;
    for(auto it: strList)
        data.push_back(it.replace(',', ".").toFloat()); //replace нужен, потому что csv испольует `,` как плавающую точку

//    for(int i = 0; i < 700; ++i)
//        data.push_back(strList[i].replace(',', ".").toFloat());

    return data;
}

const QString CSVReader::readCellTypeFromCSVRow(const QString &CSVRow)
{
    QStringList columns = CSVRow.split(';');
    QString imagePath = columns[0]; //1-ый столбец - путь к изображению
    return getCellTypeFromCSVRow(imagePath);
}
