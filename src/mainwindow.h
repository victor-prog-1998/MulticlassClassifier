#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "csvreader.h"
#include "stringlistmodel.h"
#include "labelshash.h"
#include "datacollector.h"
#include "classifier.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void updateListView();
    bool readDataFromCSVFiles();
    ~MainWindow();

private slots:
//    void on_pushButton_clearFileList_clicked();

    void on_pushButton_selectFiles_clicked();



    void on_pushButton_train_clicked();


    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_readFiles_clicked();

    //void on_pushButton_predict_clicked();

    void on_pushButton_auto_clicked();

    void on_checkBox_maxIter_toggled(bool checked);

    void on_checkBox_epsilon_toggled(bool checked);

    void on_horizontalSlider_trainSetsPercent_valueChanged(int value);

    void on_pushButton_classifyTestSample_clicked();

private:
    Ui::MainWindow *ui;
    CSVReader m_csvReader;
    QStringList m_fileList;
    StringListModel *m_strListModel;
    LabelsHash m_labelsHash;

    QList<float> labels;
    DataCollector m_dataCollector;
    Classifier m_classifier;
    bool CSVFileListIsCorrect(const QStringList& strList);

    void setInterfaceForKernel(const int kernel);
    double strToDouble(const QString& str, bool& ok, bool zeroIsValid = false); /*Аналог метода QString::toDouble.
    Устанавливает ok в true, если число с плавающей точкой введено корректно и является положительным. Установка параметра
    zeroIsValid в true делает допустимым также и нулевое значение (которое допустимо для параметра coef0)
*/
    int strToInt(const QString& str, bool &ok);
    bool setClassifierParameters(); /*устанавливает параметры классификатора на основе введенных пользователем
    значений. При обнаружении ошибок ввода вернет false и выведет список параметров с ошибками*/
    bool checkAutoParameters(); /*Проверяет корректность ввода параметров для автоматической проверки, в том числе и критерий остановки,
                                        который задает в случае отсутствия ошибок*/
    void autoTest();
    float averageOfFloatList(const QList<float> &list); // возвращает среднее значение списка float
};

#endif // MAINWINDOW_H
