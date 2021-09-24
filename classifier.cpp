#include "classifier.h"
#include <QDebug>

Classifier::Classifier()
{
    this->m_svm = SVM::create();
    this->m_svm->setType(SVM::C_SVC);
    //this->m_svm->setTermCriteria(TermCriteria(TermCriteria::EPS, (int)1e3, 0.4));
}

Classifier::~Classifier()
{
     delete m_input;
     delete m_labels;

     delete m_trainInput;
     delete m_trainLabels;

     delete m_unknownInput;
     delete m_unknownLabels;

    delete this->m_pca;
    delete this->m_normalizer;
}

void Classifier::setKernel(const int kernelType)
{
    this->m_svm->setKernel(kernelType);
}

void Classifier::setC(const double C)
{
    this->m_svm->setC(C);
}

void Classifier::setGamma(const double gamma)
{
    this->m_svm->setGamma(gamma);
}

void Classifier::setCoef0(const double coef0)
{
    this->m_svm->setCoef0(coef0);
}

void Classifier::setDegree(const double degree)
{
    this->m_svm->setDegree(degree);

}

void Classifier::setTrainData(const QList<QList<float> > &trainInput, const QList<float> &labels)
{
     //освобождение памяти на случай, если эта функция вызывается не первый раз
        //после запуска программы
    //this->m_input = new Mat(trainInput.size(), trainInput[0].size(), CV_32F);

    delete m_labels;
    this->m_labels = new Mat(labels.size(), 1, CV_32S);



    Mat tmp(trainInput.size(), trainInput[0].size(), CV_32F); // матрица в сыром виде
    for(int r = 0; r < trainInput.size(); ++r)
        for(int c = 0; c < trainInput[0].size(); ++c)
            tmp.at<float>(r, c) = trainInput[r][c];


    // #####

//    Mat egv;
//    eigen(tmp.t() * tmp, egv);

//    QList<float> eig;
//    for(int i = 0; i < egv.rows; ++i)
//        eig.push_back(egv.at<float>(i, 0));
//    std::sort(eig.begin(), eig.end(), [](int a, int b) {return a > b;});

//    float sum = 0;
//    for(auto it: eig)
//        sum += it;

//    qDebug() << "sum = " << sum;
//    QHash<int, float> hash;
//    for(int m = 1; m < eig.size(); ++m)
//    {
//        float s = 0;
//        for(int i = m; i < eig.size(); ++i)
//            s += eig[i];
//        float e = s / sum;
//        qDebug() << "m = " << m << ": " << e;
//        hash.insert(m, e);
//    }

//    QList<float> list = hash.values();
//    std::sort(list.begin(), list.end(), [](int a, int b) {return a > b;});


//    float epsilon = 0.01;

//    int m = 0;
//    while((list[m] > epsilon) && (m < list.size()))
//        ++m;

//    qDebug() << "Наилучшая размерность пространства: " << hash.key(list[m]);

    // #####

    delete this->m_pca;
    this->m_pca = new PCA(tmp, noArray(), CV_PCA_DATA_AS_ROW, m_desiredNumber);
    Mat compressed;
    this->m_pca->project(tmp, compressed);

    delete this->m_normalizer;
    this->m_normalizer = new Normalizer(compressed);

    delete m_input;
    this->m_input = new Mat(trainInput.size(), m_desiredNumber, CV_32F);
    this->m_normalizer->normalizeMat(compressed, *this->m_input);


/*
        Mat means, sigmas;  //matrices to save all the means and standard deviations
            for (int i = 0; i < compressed.cols; i++){  //take each of the features in vector
                Mat mean; Mat sigma;
                meanStdDev(compressed.col(i), mean, sigma);  //get mean and std deviation
                means.push_back(mean);
                sigmas.push_back(sigma);
                this->m_input->col(i) = (compressed.col(i) - mean) / sigma;  //normalization
            }
*/

//    Mat *normalized = new Mat(trainInput.size(), trainInput[0].size(), CV_32F);

//    //normalize(*tmp, *m_input, 1.0, 0.1, NORM_MINMAX);
//    normalize(*tmp, *normalized, 1.0, 0.1, NORM_MINMAX);



//    PCA pca(*normalized, noArray(), CV_PCA_DATA_AS_ROW, desiredNumber);

//    this->m_input = new Mat(trainInput.size(), desiredNumber, CV_32F);
//    pca.project(*normalized, *this->m_input);

    //-----------------------------------------------------------------

    for(int r = 0; r < labels.size(); ++r)
        m_labels->at<float>(r, 0) = labels[r];
}

void Classifier::setTermCriteria(const bool maxIterFlag, const bool epsilonFlag, const int maxIter, const double epsilon)
{
    if(maxIterFlag)
    {
        if(epsilonFlag)
            this->m_svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, maxIter, epsilon));
        else
            this->m_svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, maxIter, epsilon));
    }
    else
        this->m_svm->setTermCriteria(TermCriteria(TermCriteria::EPS, maxIter, epsilon));
}

void Classifier::train()
{
    //this->m_svm->train(*this->m_input, ROW_SAMPLE, *this->m_labels);
    this->m_svm->train(*this->m_trainInput, ROW_SAMPLE, *this->m_trainLabels);
    //this->m_svm->train(TrainData::create(*this->m_trainInput, ROW_SAMPLE, *this->m_trainLabels), SVM::UPDATE_MODEL);

}

void Classifier::trainOnAllSamples()
{
    this->m_svm->train(*this->m_input, ROW_SAMPLE, *this->m_labels);
}

/* НЕ ИСПОЛЬЗУЕТСЯ
 *
void Classifier::train(const int percents, const bool splitData)
{
    if(splitData)
    {
        if(this->m_trainInput) delete this->m_trainInput;
        if(this->m_trainLabels) delete this->m_trainLabels;
        if(this->m_unknownInput) delete this->m_unknownInput;
        if(this->m_unknownLabels) delete this->m_unknownLabels;

        this->m_trainInput = new Mat;
        this->m_trainLabels = new Mat;
        this->m_unknownInput = new Mat;
        this->m_unknownLabels = new Mat;

        for(int r = 0; r < m_input->rows; ++r)
        {
            int k = 0 + qrand() % 100;
            if(k < percents)
            {
                this->m_trainInput->push_back(this->m_input->row(r));
                this->m_trainLabels->push_back(this->m_labels->row(r));
            }
            else
            {
                this->m_unknownInput->push_back(this->m_input->row(r));
                this->m_unknownLabels->push_back(this->m_labels->row(r));
            }
        }
    }

    this->m_svm->train(*this->m_trainInput, ROW_SAMPLE, *this->m_trainLabels);
    //this->m_svm->train(TrainData::create(*this->m_trainInput, ROW_SAMPLE, *this->m_trainLabels), SVM::UPDATE_MODEL);
}
*/

double Classifier::getC()
{
    return this->m_svm->getC();
}



const QList<float> Classifier::predict(const CLASSIFIER::predictSamples predSamples)
{
    QList<float> result;
    union
    {
        int in;
        float out;
    }correctFloat;

    switch(predSamples)
    {
    case CLASSIFIER::trainSamples:
        for(int r = 0; r < this->m_trainInput->rows; ++r)
        {
            correctFloat.in = this->m_svm->predict(this->m_trainInput->row(r));
            float res = correctFloat.out;
            result.push_back(res);
        }
        break;
    case CLASSIFIER::unknownSamples:
        for(int r = 0; r < this->m_unknownInput->rows; ++r)
        {
            correctFloat.in = this->m_svm->predict(this->m_unknownInput->row(r));
            float res = correctFloat.out;
            result.push_back(res);
        }
        break;
    case CLASSIFIER::allSamples:
        for(int r = 0; r < this->m_input->rows; ++r)
        {
            correctFloat.in = this->m_svm->predict(this->m_input->row(r));
            float res = correctFloat.out;
            result.push_back(res);
        }
        break;
    }

    return result;
}

float Classifier::classifyTestSample(const QList<float> &features)
{
    Mat tmp(1, features.size(), CV_32F); // матрица в сыром виде
    for(int i = 0; i < features.size(); ++i)
        tmp.at<float>(0, i) = features[i];

    Mat compressed; // сжатая методом главных компонент
    this->m_pca->project(tmp, compressed);

    Mat processedMat(1, m_desiredNumber, CV_32F); // готовая матрица
    this->m_normalizer->normalizeMat(compressed, processedMat); // нормализация

    union
    {
        int in;
        float out;
    }correctFloat;

    correctFloat.in = this->m_svm->predict(processedMat);
    return correctFloat.out;
}

const QList<float> Classifier::correctLabels(const CLASSIFIER::predictSamples predSamples)
{
    QList<float> labels;
    switch(predSamples)
    {
    case CLASSIFIER::trainSamples:
        for(int r = 0; r < this->m_trainLabels->rows; ++r)
            labels.push_back(this->m_trainLabels->at<float>(r, 0));
        break;
    case CLASSIFIER::unknownSamples:
        for(int r = 0; r < this->m_unknownLabels->rows; ++r)
            labels.push_back(this->m_unknownLabels->at<float>(r, 0));
        break;
    case CLASSIFIER::allSamples:
        for(int r = 0; r < this->m_labels->rows; ++r)
            labels.push_back(this->m_labels->at<float>(r, 0));
        break;
    }
    return labels;
}

int Classifier::kernel()
{
    return this->m_svm->getKernelType();
}

void Classifier::setDataForCrossValidation(const int k, const int numOfGroups)
{
    if(k >= numOfGroups)
    {
        qDebug() << "Номер группы выходит из диапазона";
        return;
    }
    if(this->m_trainInput) delete this->m_trainInput;
    if(this->m_trainLabels) delete this->m_trainLabels;
    if(this->m_unknownInput) delete this->m_unknownInput;
    if(this->m_unknownLabels) delete this->m_unknownLabels;

    this->m_trainInput = new Mat;
    this->m_trainLabels = new Mat;
    this->m_unknownInput = new Mat;
    this->m_unknownLabels = new Mat;

    int n = this->m_input->rows / numOfGroups; //число наборов в группе
    int from; //начиная с какого индекса назначаются неизвестные наборы (наборы в k-ой группе)
    int to;   //по какой индекс (не включительно)
    (k == 0) ? from = 0 : from = n * k;
    (k == numOfGroups - 1) ? to = m_input->rows : to = from + n;

    for(int i = 0; i < this->m_input->rows; ++i)
    {
        if((i >= from) && (i < to))
        {
            this->m_unknownInput->push_back(this->m_input->row(i));
            this->m_unknownLabels->push_back(this->m_labels->row(i));
        }
        else
        {
            this->m_trainInput->push_back(this->m_input->row(i));
            this->m_trainLabels->push_back(this->m_labels->row(i));
        }
    }
}
 //  тестовые
void Classifier::create()
{
    this->m_svm->create();
}
void Classifier::setType(const int t)
{
    this->m_svm->setType(t);
}
