#include "resultmatrix.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

ResultMatrix::ResultMatrix(LabelsHash *labelsHash):
    m_labelsHash(labelsHash)
{

}

void ResultMatrix::clear()
{
    this->m_matrices.clear();
}

void ResultMatrix::addMatrix(const QList<float> &correctLabels, const QList<float> &predictLabels)
{
    int n = this->m_labelsHash->size(); // число классов и в то же время размер матрицы (матрица квадратная)



    QList<QList<float>> matrix; // матрица для дальнейшей вставки в "список матриц"

    // придание матрице размера (n x n) и заполнение её нулями
    QList<float> row;
    for(int i = 0; i < n; ++i)
        row.push_back(0);

    for(int i = 0; i < n; ++i)
        matrix.push_back(row);

    // вычисления значений элементов матрицы
    for(int i = 0; i < correctLabels.size(); ++i)
    {
        int r = correctLabels[i]; // строка матрицы соответствует истинному значению
        int c = predictLabels[i]; // столбец матрицы соответствует предсказанному значению
        ++matrix[r][c];
    }

//    for(auto r: matrix)
//    {
//        float sum = 0;
//        for(auto c: r)
//            sum += c;
//        for(int c = 0; c < r.size(); ++c)
//            r[c] = r[c] * 100 / sum;
//    }
    for(int r = 0; r < matrix.size(); ++r)
    {
        float sum = 0;
        for(int c = 0; c < matrix[0].size(); ++c)
            sum += matrix[r][c];
        for(int c = 0; c < matrix[0].size(); ++c)
        {
            matrix[r][c] /= sum;
            matrix[r][c] *= 100;
        }
    }

    this->m_matrices.push_back(matrix); // добавление матрицы в "список матриц"
}

void ResultMatrix::setKernel(const QString &kernel)
{
    this->m_kernel = kernel;
}

void ResultMatrix::setC(const QString &c)
{
    this->m_c = c;
    this->m_c.replace('.', ',');
}

void ResultMatrix::setGamma(const QString &gamma)
{
    this->m_gamma = gamma;
    this->m_gamma.replace('.', ',');
}

void ResultMatrix::setCoef0(const QString &coef_0)
{
    this->m_coef0 = coef_0;
    this->m_coef0.replace('.', ',');
}

void ResultMatrix::setDegree(const QString &degree)
{
    this->m_degree = degree;
    this->m_degree.replace('.', ',');
}

bool ResultMatrix::saveToCsv()
{
    QString filePath = QFileDialog::getSaveFileName(0, "Сохранение результата", "", "CSV files (*.csv)");
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream stream(&file);

    stream << this->makeClassifierInfo();

    for(int i = 0; i < this->m_matrices.size(); ++i)
    {
        stream << "Кросс-валидация;группа " + QString::number(i + 1) + '\n';
        stream << this->makeTableHeader() + '\n';
        for(int j = 0; j < this->m_labelsHash->size(); ++j)
            stream << this->makeTableRow(i, j) + '\n';
        stream << "\n\n";
    }

    QList<QList<float>> averageMatrix = this->makeAverageMatrix();
    stream << QString("Средние значения\n");
    stream << this->makeTableHeader() + '\n';
    for(int i = 0; i < averageMatrix.size(); ++i)
        stream << this->makeAverageTableRow(averageMatrix[i], i) + '\n';

    file.close();
    return true;
}

QString ResultMatrix::makeTableHeader()
{
    QString header;
    for(int i = 0; i < this->m_labelsHash->size(); ++i)
        header += ';' + this->m_labelsHash->cellType(i);
    return header;
}

QString ResultMatrix::makeTableRow(int tableNum, int index)
{
    QString tableRow = this->m_labelsHash->cellType((float)index);
    for(auto it: this->m_matrices[tableNum][index])
        tableRow += ';' + QString::number(it) + " %";
    return tableRow;
}

QString ResultMatrix::makeAverageTableRow(const QList<float> &row, int index)
{
    QString tableRow = this->m_labelsHash->cellType((float)index);
    for(auto it: row)
    {
        QString value;
        value.setNum(it);
        value.replace('.', ','); // замена десятичного разделителя на ','
        tableRow += ';' + value + " %";
    }
    return tableRow;

}

QString ResultMatrix::makeClassifierInfo()
{
    QString info = "Ядро;" + this->m_kernel + '\n';
    info += "Параметр С;" + this->m_c + '\n';
    if(!this->m_gamma.isEmpty())
        info += "gamma;" + this->m_gamma + '\n';
    if(!this->m_coef0.isEmpty())
        info += "coef 0;" + this->m_coef0 + '\n';
    if(!this->m_degree.isEmpty())
        info += "degree;" + this->m_degree + '\n';

    info += "\nЧисло групп в кросс-валидации;" + QString::number(this->m_matrices.size()) + "\n\n";

    return info;
}

QList<QList<float>> ResultMatrix::makeAverageMatrix()
{
    QList<QList<float>> averageMatrix;

    int n = this->m_labelsHash->size();
    QList<float> row;
    for(int i = 0; i < n; ++i)
        row.push_back(0);

    for(int i = 0; i < n; ++i)
        averageMatrix.push_back(row);

    for(int r = 0; r < n; ++r)
        for(int c = 0; c < n; ++c)
        {
            float sum = 0;
            for(int k = 0; k < this->m_matrices.size(); ++k)
                sum+= this->m_matrices[k][r][c];
            averageMatrix[r][c] = sum / this->m_matrices.size();
        }

    return averageMatrix;
}

