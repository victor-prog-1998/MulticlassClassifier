#ifndef NORMALIZER_H
#define NORMALIZER_H
#include <QList>
#include "open_cv/include/opencv2/opencv.hpp"
using namespace cv;

class Normalizer
{
public:
    Normalizer(const Mat& featuresMat); // featuresMat - матрица, на основе которой вычисляются минимумы и максимумы каждого стоблца
    void normalizeMat(const Mat& inputMat, Mat& outputMat); // нормализует матрицу. outputMat - ссылка на матрицу, в которую записывается
                                                                            // результат нормализации

private:
    QList<double> m_minimums; // число элементов списках соответствует числу столбцов матрицы признаков
    QList<double> m_maximums;

};

#endif // NORMALIZER_H
