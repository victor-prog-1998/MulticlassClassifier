#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "open_cv/include/opencv2/opencv.hpp"
#include "open_cv/include/opencv2/ml/ml.hpp"
#include "open_cv/include/opencv2/highgui/highgui.hpp"
#include "open_cv/include/opencv2/ml/ml.hpp"
#include <QList>
#include "normalizer.h"
using namespace cv;
using namespace ml;

namespace CLASSIFIER
{
    enum predictSamples
    {
        trainSamples,
        unknownSamples,
        allSamples
    };
}
class Classifier
{
public:
    Classifier();
    ~Classifier();
    void setKernel(const int kernelType);
    void setC(const double C);
    void setGamma(const double gamma);
    void setCoef0(const double coef0);
    void setDegree(const double degree);
    void setTrainData(const QList<QList<float>> &trainInput, const QList<float> &labels);
//    void setData(const QList<QList<float>> &trainInput, const QList<float> &trainLabels,
//                 const QList<QList<float>> &unknownInput, const QList<float> &unknownLabels);
    void setTermCriteria(const bool maxIterFlag, const bool epsilonFlag, const int maxIter, const double epsilon);
    void train(); // обучение на выборке, которая назначена как обучающая
    void trainOnAllSamples(); // обучение на всех данных (и на обучающих, и на тестовых)

    /*void train(const int percents, const bool splitData = true); НЕ ИСПОЛЬЗУЕТСЯ. Использовался до введения
    системы кросс-валидации. Задается процент наборов, используемых
    при обучении, от числа всех наборов в файлах. Флаг splitData указывает, нужно ли разбивать все данные в соответствии с заданным
    числом процентов, т. е. пересоздвавать матрицы m_train... и m_unknown...*/

    double getC();
    const QList<float> predict(const CLASSIFIER::predictSamples predSamples = CLASSIFIER::allSamples);
    float classifyTestSample(const QList<float> &features);
    const QList<float> correctLabels(const CLASSIFIER::predictSamples predSamples);
    int kernel();

    void setDataForCrossValidation(const int k, const int numOfGroups);

    //  тестовые
    void create();
    void setType(const int t);
private:
    Ptr<SVM> m_svm;
    Mat *m_input = nullptr;
    Mat *m_labels = nullptr;

    Mat *m_trainInput = nullptr;
    Mat *m_trainLabels = nullptr;

    Mat *m_unknownInput = nullptr;
    Mat *m_unknownLabels = nullptr;

    Normalizer *m_normalizer = nullptr;
    PCA *m_pca = nullptr;

    const int m_desiredNumber = 41;


};

#endif // CLASSIFIER_H
