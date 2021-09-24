#ifndef RESULTMATRIX_H
#define RESULTMATRIX_H
#include <QList>
#include "labelshash.h"

class ResultMatrix
{
public:
    ResultMatrix(LabelsHash* labelsHash);
    void clear();
    void addMatrix(const QList<float> &correctLabels, const QList<float> &predictLabels);
    void setKernel(const QString& kernel);
    // передаем копии строк, чтобы применить метод replace
    void setC(const QString &c);
    void setGamma(const QString &gamma);
    void setCoef0(const QString &coef_0);
    void setDegree(const QString &degree);
    bool saveToCsv();
private:
    QList<QList<QList<float>>> m_matrices;
    LabelsHash *m_labelsHash;
    QString m_kernel;
    QString m_c;
    QString m_gamma;
    QString m_coef0;
    QString m_degree;

    QString makeTableHeader();
    QString makeTableRow(int tableNum, int index);
    QString makeAverageTableRow(const QList<float> &row, int index);
    QString makeClassifierInfo();
    QList<QList<float>> makeAverageMatrix();
};

#endif // RESULTMATRIX_H
