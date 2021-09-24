#include "normalizer.h"



Normalizer::Normalizer(const Mat &featuresMat)
{
    for(int c = 0; c < featuresMat.cols; ++c)
    {
        double min;
        double max;
        minMaxIdx(featuresMat.col(c), &min, &max);
        this->m_minimums.push_back(min);
        this->m_maximums.push_back(max);
    }
}

void Normalizer::normalizeMat(const Mat &inputMat, Mat &outputMat)
{
    for(int c = 0; c < inputMat.cols; ++c)
    {
        for(int r = 0; r < inputMat.rows; ++r)
        {
            outputMat.at<float>(r, c) = (inputMat.at<float>(r, c) - m_minimums[c]) / (m_maximums[c] - m_minimums[c]);
        }
    }
}
