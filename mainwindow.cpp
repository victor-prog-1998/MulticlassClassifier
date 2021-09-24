#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "resultmatrix.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

//    QString CSVPath = QFileDialog::getOpenFileName();
//    if(this->m_csvReader.readCSV(CSVPath))
//    {
//        QString cellType = this->m_csvReader.cellType();
//        QMessageBox::information(0, "", "CSV-файл прочитан: " + cellType);
//        QString res;
//        QList<QList<float>> data = this->m_csvReader.data();
//        for(QList<float> it: data)
//        {
//            for(float fl: it)
//            {
//                res += '[';
//                QString num;
//                num.setNum(fl, 'f', 10);
//                res += num + ']';
//            }
//            res += '\n';
//        }
    ui->setupUi(this);
    this->setWindowTitle("Метод опорных векторов");
    this->m_strListModel = new StringListModel(this);
    this->ui->listView_files->setModel(this->m_strListModel);
    this->m_classifier.setKernel(SVM::LINEAR);
//    this->ui->label_gamma->hide();
//    this->ui->label_coef0->hide();
//    this->ui->label_degree->hide();
//    this->ui->lineEdit_gamma->hide();
//    this->ui->lineEdit_coef0->hide();
//    this->ui->lineEdit_degree->hide();
    this->setInterfaceForKernel(SVM::LINEAR);
    this->ui->pushButton_readFiles->setEnabled(false);
    this->ui->pushButton_train->setEnabled(false);
    this->ui->pushButton_auto->setEnabled(false);

    this->ui->lineEdit_maxIter->setEnabled(false);
    this->ui->checkBox_epsilon->setChecked(true);

    QString k;
    k.setNum(this->ui->horizontalSlider_trainSetsPercent->value());
    this->ui->lineEdit_trainSetsPercent->setText(k);

}

void MainWindow::updateListView()
{
    this->ui->listView_files->reset();
    this->ui->listView_files->setModel(this->m_strListModel);
}

bool MainWindow::readDataFromCSVFiles()
{
    this->m_dataCollector.clear(); //очистка в случае, если а этих объектах уже хранятся данные
    this->m_labelsHash.clear();
    bool readSuccess = true;
    //объект m_cvsReader чистит сам себя при прочтении очередного CSV-файла
    QStringList readErrorFiles;
    int trainSetsCounter = 0;
    for(auto file: this->m_fileList)
    {
        if(!this->m_csvReader.readCSV(file))
            readErrorFiles.push_back(file);
        else
        {
            QList<QList<float>> data = this->m_csvReader.data();
            float label = this->m_labelsHash.add(this->m_csvReader.cellType());
            this->m_dataCollector.add(data, label);
            trainSetsCounter += data.size();
        }
    }
    if(!readErrorFiles.isEmpty())
    {
        QString errorFilesString;
        for(auto it: readErrorFiles)
            errorFilesString += it + '\n';
        QMessageBox::critical(0, "Ошибка чтения файлов", "Не удалось прочитать следующие файлы:\n" + errorFilesString);
        readSuccess = false;
    }
    else
    {
        this->m_dataCollector.mix(); //перемешиваем наборы, чтобы они не были сгруппированы по классам в порядке следования
        QString trainSetsTotal;
        trainSetsTotal.setNum(trainSetsCounter);
        QMessageBox::information(0, "Информация", "Файлы успешно прочитаны\n"
                                                "Всего обучающих наборов: " + trainSetsTotal);
    }

    return readSuccess;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete this->m_strListModel;
}

//void MainWindow::on_pushButton_clearFileList_clicked()
//{
//    this->m_fileList.clear();
//    this->m_strListModel->clear();
//    this->updateListView();
//    this->ui->pushButton_clearFileList->setEnabled(false);
//    this->ui->pushButton_readFiles->setEnabled(false);
//    //this->ui->pushButton_train->setEnabled(false);
//    if(this->m_dataCollector.isEmpty())
//        this->ui->label_hintLabel->setText("Нажмите \"Выбрать\" для выбора .csv файлов");
//    else
//        this->ui->label_hintLabel->setText("В классификаторе хранятся данные\n"
//                                           "из выбранных ранее .csv файлов.\n"
//                                           "Если необходимо выбрать другие файлы\n"
//                                           "для обучения, нажмите \"Выбрать\".");
//}

void MainWindow::on_pushButton_selectFiles_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames();
    if(!files.isEmpty()) //то есть если пользователь выбрал файлы, а не закрыл окно, так и не выбрав
    {
        if(!CSVFileListIsCorrect(files)) //если среди файлов есть те, что имеют расширение не .csv
        {
            QMessageBox::critical(0, "Некорректный набор файлов", "Среди выбранных файлов обнаружены обнаружены файлы\n"
                                                                  "с расширением, отличным от .csv!\n"
                                                                  "Выберите корректные файлы.");
            return;
        }
        else if(files.size() == 1)
        {
            QMessageBox::critical(0, "Недостаточно тренировочных данных", "Число файлов для обучения должно быть "
                                                                          "не меньше двух!");
            return;
        }
        this->m_fileList = files;
        this->m_strListModel->setStringList(this->m_fileList);
        this->updateListView();
        this->ui->pushButton_readFiles->setEnabled(true);
//        if (this->m_dataCollector.isEmpty())
//            this->ui->pushButton_train->setEnabled(false);

        static bool firstCall = true;
        if(firstCall) //Условие выполняется только при первом (после запуска программы) выборе файлов
        {
            this->ui->pushButton_selectFiles->setText("Выбрать другие");
            firstCall = false;
        }
    }
}


void MainWindow::on_pushButton_train_clicked()
{
    if(this->setClassifierParameters())
    {
        this->m_classifier.trainOnAllSamples();
        QMessageBox::information(0,"Информация","Классификатор обучен. Нажмите \"Классифициировать тестовый образец\",\n"
                                                "чтобы открыть тестовый файл и показать результат классификации.");
        this->ui->pushButton_classifyTestSample->setEnabled(true);
    }
}



void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "Линейное")
    {
        this->m_classifier.setKernel(SVM::LINEAR);
        this->setInterfaceForKernel(SVM::LINEAR);
        qDebug() << "Выбрано линейное ядро";
    }
    else if(arg1 == "Полиномиальное")
    {
        this->m_classifier.setKernel(SVM::POLY);
        this->setInterfaceForKernel(SVM::POLY);
    }
    else if(arg1 == "Радиально-базисные функции")
    {
        this->m_classifier.setKernel(SVM::RBF);
        this->setInterfaceForKernel(SVM::RBF);
    }
    else if(arg1 == "Сигмоидное")
    {
        this->m_classifier.setKernel(SVM::SIGMOID);
        this->setInterfaceForKernel(SVM::SIGMOID);
    }
    else if(arg1 == "Экспоненциальное хи-квадрат")
    {
        this->m_classifier.setKernel(SVM::CHI2);
        this->setInterfaceForKernel(SVM::CHI2);
    }
    else if(arg1 == "Пересечение гистограмм")
    {
        this->m_classifier.setKernel(SVM::INTER);
        this->setInterfaceForKernel(SVM::INTER);
    }
}

void MainWindow::on_pushButton_readFiles_clicked()
{

    if(this->readDataFromCSVFiles())
    {
        this->m_classifier.setTrainData(this->m_dataCollector.data(), this->m_dataCollector.labels());
        this->ui->pushButton_train->setEnabled(true);
        this->ui->pushButton_auto->setEnabled(true);
    }

}

bool MainWindow::CSVFileListIsCorrect(const QStringList &strList)
{
    for(auto it: strList)
    {
        QStringList list = it.split('.');
        QString extension = list.last();
        if(extension != "csv")
            return false;
    }
    return true;
}
/**
 * @brief MainWindow::setInterfaceForKernel
 * @details bubu
 * @param kernel
 */
void MainWindow::setInterfaceForKernel(const int kernel)
{
    /*
     * Линейное - без параметров
     * Полиноминальное - gamma, coef0, degree
     * Радиально-базисные функции - gamma
     * Сигмоидное - gamma, coef0
     * Экспоненциальное хи-квадрат - gamma
     * Пересечение гистограмм - без параметров
     * */
    switch(kernel)
    {
    case SVM::LINEAR:
    {
        this->ui->label_gamma->hide();
        this->ui->label_coef0->hide();
        this->ui->label_degree->hide();
        this->ui->lineEdit_gamma->hide();
        this->ui->lineEdit_coef0->hide();
        this->ui->lineEdit_degree->hide();

        //for auto:

        this->ui->label_gamma_auto->hide();
        this->ui->label_gamma_from->hide();
        this->ui->label_gamma_to->hide();
        this->ui->label_gamma_step->hide();
        this->ui->lineEdit_gamma_from->hide();
        this->ui->lineEdit_gamma_to->hide();
        this->ui->lineEdit_gamma_step->hide();

        this->ui->label_degree_auto->hide();
        this->ui->label_degree_from->hide();
        this->ui->label_degree_to->hide();
        this->ui->label_degree_step->hide();
        this->ui->lineEdit_degree_from->hide();
        this->ui->lineEdit_degree_to->hide();
        this->ui->lineEdit_degree_step->hide();

        this->ui->label_coef0_auto->hide();
        this->ui->label_coef0_from->hide();
        this->ui->label_coef0_to->hide();
        this->ui->label_coef0_step->hide();
        this->ui->lineEdit_coef0_from->hide();
        this->ui->lineEdit_coef0_to->hide();
        this->ui->lineEdit_coef0_step->hide();

        break;
    }
    case SVM::POLY:
    {
        this->ui->label_gamma->show();
        this->ui->label_coef0->show();
        this->ui->label_degree->show();
        this->ui->lineEdit_gamma->show();
        this->ui->lineEdit_coef0->show();
        this->ui->lineEdit_degree->show();

        //for auto:

        this->ui->label_gamma_auto->show();
        this->ui->label_gamma_from->show();
        this->ui->label_gamma_to->show();
        this->ui->label_gamma_step->show();
        this->ui->lineEdit_gamma_from->show();;
        this->ui->lineEdit_gamma_to->show();
        this->ui->lineEdit_gamma_step->show();

        this->ui->label_degree_auto->show();
        this->ui->label_degree_from->show();
        this->ui->label_degree_to->show();
        this->ui->label_degree_step->show();
        this->ui->lineEdit_degree_from->show();
        this->ui->lineEdit_degree_to->show();
        this->ui->lineEdit_degree_step->show();

        this->ui->label_coef0_auto->show();
        this->ui->label_coef0_from->show();
        this->ui->label_coef0_to->show();
        this->ui->label_coef0_step->show();
        this->ui->lineEdit_coef0_from->show();
        this->ui->lineEdit_coef0_to->show();
        this->ui->lineEdit_coef0_step->show();

        break;
    }
    case SVM::RBF:
    {
        this->ui->label_gamma->show();
        this->ui->label_coef0->hide();
        this->ui->label_degree->hide();
        this->ui->lineEdit_gamma->show();
        this->ui->lineEdit_coef0->hide();
        this->ui->lineEdit_degree->hide();

        //for auto:

        this->ui->label_gamma_auto->show();
        this->ui->label_gamma_from->show();
        this->ui->label_gamma_to->show();
        this->ui->label_gamma_step->show();
        this->ui->lineEdit_gamma_from->show();;
        this->ui->lineEdit_gamma_to->show();
        this->ui->lineEdit_gamma_step->show();

        this->ui->label_degree_auto->hide();
        this->ui->label_degree_from->hide();
        this->ui->label_degree_to->hide();
        this->ui->label_degree_step->hide();
        this->ui->lineEdit_degree_from->hide();
        this->ui->lineEdit_degree_to->hide();
        this->ui->lineEdit_degree_step->hide();

        this->ui->label_coef0_auto->hide();
        this->ui->label_coef0_from->hide();
        this->ui->label_coef0_to->hide();
        this->ui->label_coef0_step->hide();
        this->ui->lineEdit_coef0_from->hide();
        this->ui->lineEdit_coef0_to->hide();
        this->ui->lineEdit_coef0_step->hide();

        break;
    }
    case SVM::SIGMOID:
    {
        this->ui->label_gamma->show();
        this->ui->label_coef0->show();
        this->ui->label_degree->hide();
        this->ui->lineEdit_gamma->show();
        this->ui->lineEdit_coef0->show();
        this->ui->lineEdit_degree->hide();

        //for auto:

        this->ui->label_gamma_auto->show();
        this->ui->label_gamma_from->show();
        this->ui->label_gamma_to->show();
        this->ui->label_gamma_step->show();
        this->ui->lineEdit_gamma_from->show();;
        this->ui->lineEdit_gamma_to->show();
        this->ui->lineEdit_gamma_step->show();

        this->ui->label_degree_auto->hide();
        this->ui->label_degree_from->hide();
        this->ui->label_degree_to->hide();
        this->ui->label_degree_step->hide();
        this->ui->lineEdit_degree_from->hide();
        this->ui->lineEdit_degree_to->hide();
        this->ui->lineEdit_degree_step->hide();

        this->ui->label_coef0_auto->show();
        this->ui->label_coef0_from->show();
        this->ui->label_coef0_to->show();
        this->ui->label_coef0_step->show();
        this->ui->lineEdit_coef0_from->show();
        this->ui->lineEdit_coef0_to->show();
        this->ui->lineEdit_coef0_step->show();

        break;
    }
    case SVM::CHI2:
    {
        this->ui->label_gamma->show();
        this->ui->label_coef0->hide();
        this->ui->label_degree->hide();
        this->ui->lineEdit_gamma->show();
        this->ui->lineEdit_coef0->hide();
        this->ui->lineEdit_degree->hide();

        //for auto:

        this->ui->label_gamma_auto->show();
        this->ui->label_gamma_from->show();
        this->ui->label_gamma_to->show();
        this->ui->label_gamma_step->show();
        this->ui->lineEdit_gamma_from->show();;
        this->ui->lineEdit_gamma_to->show();
        this->ui->lineEdit_gamma_step->show();

        this->ui->label_degree_auto->hide();
        this->ui->label_degree_from->hide();
        this->ui->label_degree_to->hide();
        this->ui->label_degree_step->hide();
        this->ui->lineEdit_degree_from->hide();
        this->ui->lineEdit_degree_to->hide();
        this->ui->lineEdit_degree_step->hide();

        this->ui->label_coef0_auto->hide();
        this->ui->label_coef0_from->hide();
        this->ui->label_coef0_to->hide();
        this->ui->label_coef0_step->hide();
        this->ui->lineEdit_coef0_from->hide();
        this->ui->lineEdit_coef0_to->hide();
        this->ui->lineEdit_coef0_step->hide();

        break;
    }
    case SVM::INTER:
    {
        this->ui->label_gamma->hide();
        this->ui->label_coef0->hide();
        this->ui->label_degree->hide();
        this->ui->lineEdit_gamma->hide();
        this->ui->lineEdit_coef0->hide();
        this->ui->lineEdit_degree->hide();

        //for auto:

        this->ui->label_gamma_auto->hide();
        this->ui->label_gamma_from->hide();
        this->ui->label_gamma_to->hide();
        this->ui->label_gamma_step->hide();
        this->ui->lineEdit_gamma_from->hide();
        this->ui->lineEdit_gamma_to->hide();
        this->ui->lineEdit_gamma_step->hide();

        this->ui->label_degree_auto->hide();
        this->ui->label_degree_from->hide();
        this->ui->label_degree_to->hide();
        this->ui->label_degree_step->hide();
        this->ui->lineEdit_degree_from->hide();
        this->ui->lineEdit_degree_to->hide();
        this->ui->lineEdit_degree_step->hide();

        this->ui->label_coef0_auto->hide();
        this->ui->label_coef0_from->hide();
        this->ui->label_coef0_to->hide();
        this->ui->label_coef0_step->hide();
        this->ui->lineEdit_coef0_from->hide();
        this->ui->lineEdit_coef0_to->hide();
        this->ui->lineEdit_coef0_step->hide();

        break;
    }
    }
}

double MainWindow::strToDouble(const QString &str, bool &ok, bool zeroIsValid)
{
    double res = str.toDouble(&ok);
    if(ok) //если нет ошибок ввода
    {
        if(res < 0) ok = false;
        else if((res == 0) && (!zeroIsValid)) ok = false;
    }
    return res;
}

int MainWindow::strToInt(const QString &str, bool &ok)
{
    int res = str.toInt(&ok);
    if(ok)
        if(res <= 0) ok = false;
    return res;
}

bool MainWindow::setClassifierParameters()
{
    QString errorFields;
    bool valueInFieldIsCorrect;
    double C = this->strToDouble(this->ui->lineEdit_C->text(), valueInFieldIsCorrect);
    if(!valueInFieldIsCorrect) errorFields += "\n-параметр C";
    int maxIter;
    double epsilon;
    bool maxIterFlag = false;
    bool epsilonFlag = false;

    if(this->ui->checkBox_maxIter->isChecked())
    {
        maxIterFlag = true;
        maxIter = this->strToInt(this->ui->lineEdit_maxIter->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect) errorFields += "\n-максимальное число итераций";
    }

    if(this->ui->checkBox_epsilon->isChecked())
    {
        epsilonFlag = true;
        epsilon = this->strToDouble(this->ui->lineEdit_epsilon->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect) errorFields += "\n-величина ошибки";
    }

    if((!maxIterFlag) && (!epsilonFlag))
        errorFields += "\n-не выбран критерий остановки";

    switch(this->m_classifier.kernel()) //установка параметров ядер с проверкой корректности их ввода
    {
    case SVM::POLY: //все
    {
        double degree = this->strToDouble(this->ui->lineEdit_degree->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect)
            errorFields += "\n-degree";
        double gamma = this->strToDouble(this->ui->lineEdit_gamma->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect)
            errorFields += "\n-gamma";
        double coef0 = this->strToDouble(this->ui->lineEdit_coef0->text(), valueInFieldIsCorrect, true);
        if(!valueInFieldIsCorrect)
            errorFields += "\n-coef0";
        if(errorFields.isEmpty()) //все введено корректно
        {
            this->m_classifier.setGamma(gamma);
            this->m_classifier.setDegree(degree);
            this->m_classifier.setCoef0(coef0);
        }
        break;
    }
    case SVM::RBF: //gamma
    {
        double gamma = this->strToDouble(this->ui->lineEdit_gamma->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect)
            errorFields += "\n-gamma";
        if(errorFields.isEmpty()) //все введено корректно
        {
            this->m_classifier.setGamma(gamma);
        }
        break;
    }
    case SVM::SIGMOID: //gamma, coef0
    {
        double gamma = this->strToDouble(this->ui->lineEdit_gamma->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect)
            errorFields += "\n-gamma";
        double coef0 = this->strToDouble(this->ui->lineEdit_coef0->text(), valueInFieldIsCorrect, true);
        if(!valueInFieldIsCorrect)
            errorFields += "\n-coef0";
        if(errorFields.isEmpty()) //все введено корректно
        {
            this->m_classifier.setGamma(gamma);
            this->m_classifier.setCoef0(coef0);
        }
        break;
    }
    case SVM::CHI2: //gamma
    {
        double gamma = this->strToDouble(this->ui->lineEdit_gamma->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect)
            errorFields += "\n-gamma";
        if(errorFields.isEmpty()) //все введено корректно
        {
            this->m_classifier.setGamma(gamma);
        }
        break;
    }
    }
    if(!errorFields.isEmpty())
    {
        QMessageBox::critical(0, "Некорректный ввод", "Обнаружены ошибки при указании параметров:" + errorFields);
        return false;
    }
    this->m_classifier.setTermCriteria(maxIterFlag, epsilonFlag, maxIter, epsilon);
    this->m_classifier.setC(C);
    return true;
}

/*
 * Линейное - без параметров
 * Полиноминальное - gamma, coef0, degree
 * Радиально-базисные функции - gamma
 * Сигмоидное - gamma, coef0
 * Экспоненциальное хи-квадрат - gamma
 * Пересечение гистограмм - без параметров
 * */

bool MainWindow::checkAutoParameters()
{
    QString errorFields;
    bool valueInFieldIsCorrect;
    int maxIter;
    double epsilon;
    bool maxIterFlag = false;
    bool epsilonFlag = false;

    if(this->ui->checkBox_maxIter->isChecked())
    {
        maxIterFlag = true;
        maxIter = this->strToInt(this->ui->lineEdit_maxIter->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect) errorFields += "\n-максимальное число итераций";
    }

    if(this->ui->checkBox_epsilon->isChecked())
    {
        epsilonFlag = true;
        epsilon = this->strToDouble(this->ui->lineEdit_epsilon->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect) errorFields += "\n-величина ошибки";
    }

    if((!maxIterFlag) && (!epsilonFlag))
        errorFields += "\n-не выбран критерий остановки";

    //проверка дипазонов и шагов для С **************************************************************************

    double valueFrom;
    double valueTo;
    double valueStep;
    double valueFromIsCorrect;

    valueFrom = this->strToDouble(this->ui->lineEdit_C_from->text(), valueInFieldIsCorrect);
    if(!valueInFieldIsCorrect) errorFields += "\n-параметр С, нижняя граница";
    valueFromIsCorrect = valueInFieldIsCorrect;

    valueTo = this->strToDouble(this->ui->lineEdit_C_to->text(), valueInFieldIsCorrect);
    if(!valueInFieldIsCorrect) errorFields += "\n-параметр С, верхняя граница";
    else if (valueFromIsCorrect)
        if(valueTo < valueFrom)
            errorFields += "\n-параметр С: верхняя граница меньше нижней";

    valueStep = this->strToDouble(this->ui->lineEdit_C_step->text(), valueInFieldIsCorrect);
    if(!valueInFieldIsCorrect) errorFields += "\n-параметр С, шаг";

    //проверка дипазонов и шагов для gamma, coef0, degree *******************************************************

    if(!this->ui->label_gamma_auto->isHidden())         //gamma
    {
        valueFrom = this->strToDouble(this->ui->lineEdit_gamma_from->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect) errorFields += "\n-gamma, нижняя граница";
        valueFromIsCorrect = valueInFieldIsCorrect;

        valueTo = this->strToDouble(this->ui->lineEdit_gamma_to->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect) errorFields += "\n-gamma, верхняя граница";
        else if (valueFromIsCorrect)
            if(valueTo < valueFrom)
                errorFields += "\n-gamma: верхняя граница меньше нижней";

        valueStep = this->strToDouble(this->ui->lineEdit_gamma_step->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect) errorFields += "\n-gamma, шаг";
    }

    if(!this->ui->label_coef0_auto->isHidden())         //coef0
    {
        valueFrom = this->strToDouble(this->ui->lineEdit_coef0_from->text(), valueInFieldIsCorrect, true);
        if(!valueInFieldIsCorrect) errorFields += "\n-coef0, нижняя граница";
        valueFromIsCorrect = valueInFieldIsCorrect;

        valueTo = this->strToDouble(this->ui->lineEdit_coef0_to->text(), valueInFieldIsCorrect, true);
        if(!valueInFieldIsCorrect) errorFields += "\n-coef0, верхняя граница";
        else if (valueFromIsCorrect)
            if(valueTo < valueFrom)
                errorFields += "\n-coef0: верхняя граница меньше нижней";

        valueStep = this->strToDouble(this->ui->lineEdit_coef0_step->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect) errorFields += "\n-coef0, шаг";

    }

    if(!this->ui->label_degree_auto->isHidden())        //degree
    {
        valueFrom = this->strToDouble(this->ui->lineEdit_degree_from->text(), valueInFieldIsCorrect);    //degree
        if(!valueInFieldIsCorrect) errorFields += "\n-degree, нижняя граница";
        valueFromIsCorrect = valueInFieldIsCorrect;

        valueTo = this->strToDouble(this->ui->lineEdit_degree_to->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect) errorFields += "\n-degree, верхняя граница";
        else if (valueFromIsCorrect)
            if(valueTo < valueFrom)
                errorFields += "\n-degree: верхняя граница меньше нижней";

        valueStep = this->strToDouble(this->ui->lineEdit_degree_step->text(), valueInFieldIsCorrect);
        if(!valueInFieldIsCorrect) errorFields += "\n-degree, шаг";
    }



    //*********************************************************************************

    if(!errorFields.isEmpty())
    {
        QMessageBox::critical(0, "Некорректный ввод", "Обнаружены ошибки при указании параметров:" + errorFields);
        return false;
    }
    this->m_classifier.setTermCriteria(maxIterFlag, epsilonFlag, maxIter, epsilon);
    return true;
}

void MainWindow::autoTest()
{
    struct Parameter
    {
      double from;
      double to;
      double step;
    };

    bool gammaFlag = !this->ui->label_gamma_auto->isHidden();
    bool coef0Flag = !this->ui->label_coef0_auto->isHidden();
    bool degreeFlag = !this->ui->label_degree_auto->isHidden();

    Parameter p_C;
    Parameter p_gamma;
    Parameter p_coef0;
    Parameter p_degree;

    p_C.from = this->ui->lineEdit_C_from->text().toDouble();
    p_C.to = this->ui->lineEdit_C_to->text().toDouble();
    p_C.step = this->ui->lineEdit_C_step->text().toDouble();

    if(gammaFlag)
    {
        p_gamma.from = this->ui->lineEdit_gamma_from->text().toDouble();
        p_gamma.to = this->ui->lineEdit_gamma_to->text().toDouble();
        p_gamma.step = this->ui->lineEdit_gamma_step->text().toDouble();
    }

    if(coef0Flag)
    {
        p_coef0.from = this->ui->lineEdit_coef0_from->text().toDouble();
        p_coef0.to = this->ui->lineEdit_coef0_to->text().toDouble();
        p_coef0.step = this->ui->lineEdit_coef0_step->text().toDouble();
    }

    if(degreeFlag)
    {
        p_degree.from = this->ui->lineEdit_degree_from->text().toDouble();
        p_degree.to = this->ui->lineEdit_degree_to->text().toDouble();
        p_degree.step = this->ui->lineEdit_degree_step->text().toDouble();
    }



    QString cString;
    QString resultString;
    QStringList testResults;
    QString accuracyString;



    int kFold = this->ui->horizontalSlider_trainSetsPercent->value();
    CLASSIFIER::predictSamples predSamples;
    switch(this->ui->comboBox_predictSets->currentIndex())
    {
    case 0:
        predSamples = CLASSIFIER::trainSamples;
        break;
    case 1:
        predSamples = CLASSIFIER::unknownSamples;
        break;
    case 2:
        predSamples = CLASSIFIER::allSamples;
        break;
    }



    struct LabelListsPair
    {
        LabelListsPair(const QList<float>& corLabels, const QList<float>& predLabels):
            correctLabels(corLabels),
            predictLabels(predLabels) {}
        QList<float> correctLabels;
        QList<float> predictLabels;
    };

    QList<LabelListsPair> maxAccuracyPairs;

    QString ma, maC, maGamma, maCoef0, maDegree;

    if((this->m_classifier.kernel() == SVM::LINEAR) || (this->m_classifier.kernel() == SVM::INTER)) //без параметров
    {
        float averageAccuracy;
        float maxAccuracy = 0;
        float maxAccuracy_C = 0;
        for(double C = p_C.from; C <= p_C.to; C += p_C.step)
        {
            QList<float> accuracyList;

            this->m_classifier.setC(C);

            QList<LabelListsPair> pairs;

            for(int i = 0; i < kFold; ++i)
            {
                this->m_classifier.setDataForCrossValidation(i, kFold);
                this->m_classifier.train();
                int trueResults = 0;
                QList<float> predList = this->m_classifier.predict(predSamples);
                for(int i = 0; i < predList.size(); ++i)
                    if(predList[i] == this->m_classifier.correctLabels(predSamples)[i])
                        ++trueResults;
                float accuracy = (float)trueResults / predList.size();
                accuracyList.push_back(accuracy);
                // ***
                LabelListsPair tmpPair(this->m_classifier.correctLabels(predSamples), predList);
                pairs.push_back(tmpPair);
            }

            averageAccuracy = averageOfFloatList(accuracyList) * 100;

             if(averageAccuracy > maxAccuracy)
             {
                 maxAccuracy = averageAccuracy;
                 maxAccuracy_C = C;
                 maxAccuracyPairs = pairs;
             }

            cString.setNum(C);
            accuracyString.setNum(averageAccuracy);
            resultString = "C = " + cString + ",\tточность: " + accuracyString + " %";
            testResults.push_back("\n" + resultString);
            qDebug() << resultString;
            this->ui->textEdit_testResults->append(resultString);
            this->ui->textEdit_testResults->repaint();
        }

        ma.setNum(maxAccuracy);
        maC.setNum(maxAccuracy_C);
        QString maxAccuracyStr = "\n\nМаксимальная точность: " + ma + "%\n-C = " + maC;
        testResults.push_back(maxAccuracyStr);
        qDebug() << maxAccuracyStr;
        this->ui->textEdit_testResults->append(maxAccuracyStr);
        this->ui->textEdit_testResults->repaint();
    }

    else if((this->m_classifier.kernel() == SVM::RBF) || (this->m_classifier.kernel() == SVM::CHI2)) //gamma
    {
        float maxAccuracy = 0;
        float maxAccuracy_C = 0;
        float maxAccuracy_gamma = 0;
        float averageAccuracy;


        for(double gamma = p_gamma.from; gamma <= p_gamma.to; gamma += p_gamma.step)
        {
            QString gammaString;
            gammaString.setNum(gamma);
            this->m_classifier.setGamma(gamma);
            for(double C = p_C.from; C <= p_C.to; C += p_C.step)
            {
                this->m_classifier.setC(C);
                QList<float> accuracyList;

                QList<LabelListsPair> pairs; // ***

                for(int i = 0; i < kFold; ++i)
                {
                    this->m_classifier.setDataForCrossValidation(i, kFold);
                    this->m_classifier.train();
                    int trueResults = 0;
                    QList<float> predList = this->m_classifier.predict(predSamples);
                    for(int i = 0; i < predList.size(); ++i)
                        if(predList[i] == this->m_classifier.correctLabels(predSamples)[i])
                            ++trueResults;
                    float accuracy = (float)trueResults / predList.size();
                    accuracyList.push_back(accuracy);
                    // ***
                    LabelListsPair tmpPair(this->m_classifier.correctLabels(predSamples), predList);
                    pairs.push_back(tmpPair);
                }
                averageAccuracy = averageOfFloatList(accuracyList) * 100;

                 if(averageAccuracy > maxAccuracy)
                 {
                     maxAccuracy = averageAccuracy;
                     maxAccuracy_C = C;
                     maxAccuracy_gamma = gamma;
                     maxAccuracyPairs = pairs; // ***
                 }

                cString.setNum(C);
                accuracyString.setNum(averageAccuracy);
                resultString = "gamma = " + gammaString + ", C = " + cString + ",\tточность: " + accuracyString + " %";
                testResults.push_back("\n" + resultString);
                qDebug() << resultString;
                this->ui->textEdit_testResults->append(resultString);
                this->ui->textEdit_testResults->repaint();
            }
        }
        ma.setNum(maxAccuracy);
        maC.setNum(maxAccuracy_C);
        maGamma.setNum(maxAccuracy_gamma);
        QString maxAccuracyStr = "\n\nМаксимальная точность: " + ma + "%\n-C = " + maC + "\n-gamma = " + maGamma;
        testResults.push_back(maxAccuracyStr);
        qDebug() << maxAccuracyStr;
        this->ui->textEdit_testResults->append(maxAccuracyStr);
        this->ui->textEdit_testResults->repaint();
    }

    else if(this->m_classifier.kernel() == SVM::SIGMOID) //gamma, coef0
    {
        float maxAccuracy = 0;
        float maxAccuracy_C = 0;
        float maxAccuracy_gamma = 0;
        float maxAccuracy_coef0 = 0;
        float averageAccuracy;


        for(double coef0 = p_coef0.from; coef0 <= p_coef0.to; coef0 += p_coef0.step)
        {
            QString coef0String;
            coef0String.setNum(coef0);
            this->m_classifier.setCoef0(coef0);
            for(double gamma = p_gamma.from; gamma <= p_gamma.to; gamma += p_gamma.step)
            {
                QString gammaString;
                gammaString.setNum(gamma);
                this->m_classifier.setGamma(gamma);
                for(double C = p_C.from; C <= p_C.to; C += p_C.step)
                {
                    this->m_classifier.setC(C);
                    QList<float> accuracyList;

                    QList<LabelListsPair> pairs; // ***

                    for(int i = 0; i < kFold; ++i)
                    {
                        this->m_classifier.setDataForCrossValidation(i, kFold);
                        this->m_classifier.train();
                        int trueResults = 0;
                        QList<float> predList = this->m_classifier.predict(predSamples);
                        for(int i = 0; i < predList.size(); ++i)
                            if(predList[i] == this->m_classifier.correctLabels(predSamples)[i])
                                ++trueResults;
                        float accuracy = (float)trueResults / predList.size();
                        accuracyList.push_back(accuracy);
                        // ***
                        LabelListsPair tmpPair(this->m_classifier.correctLabels(predSamples), predList);
                        pairs.push_back(tmpPair);
                    }
                    averageAccuracy = averageOfFloatList(accuracyList) * 100;

                     if(averageAccuracy > maxAccuracy)
                     {
                         maxAccuracy = averageAccuracy;
                         maxAccuracy_C = C;
                         maxAccuracy_coef0 = coef0;
                         maxAccuracy_gamma = gamma;

                         maxAccuracyPairs = pairs; // ***
                     }

                    cString.setNum(C);
                    accuracyString.setNum(averageAccuracy);
                    resultString = "coef0 = " + coef0String + ", gamma = " + gammaString +
                            ", C = " + cString + ",\tточность: " + accuracyString + " %";
                    testResults.push_back("\n" + resultString);
                    qDebug() << resultString;
                    this->ui->textEdit_testResults->append(resultString);
                    this->ui->textEdit_testResults->repaint();
                }
            }
        }
        ma.setNum(maxAccuracy);
        maC.setNum(maxAccuracy_C);
        maGamma.setNum(maxAccuracy_gamma);
        maCoef0.setNum(maxAccuracy_coef0);
        QString maxAccuracyStr = "\n\nМаксимальная точность: " + ma + "%\n-C = " + maC + "\n-gamma = " + maGamma + "\n-coef0 = " + maCoef0;
        testResults.push_back(maxAccuracyStr);
        qDebug() << maxAccuracyStr;
        this->ui->textEdit_testResults->append(maxAccuracyStr);
        this->ui->textEdit_testResults->repaint();
    }

    else if(this->m_classifier.kernel() == SVM::POLY) //gamma, coef0, degree
    {
        float maxAccuracy = 0;
        float maxAccuracy_C = 0;
        float maxAccuracy_gamma = 0;
        float maxAccuracy_coef0 = 0;
        float maxAccuracy_degree = 0;
        float averageAccuracy;

        for(double degree = p_degree.from; degree <= p_degree.to; degree += p_degree.step)
        {
            QString degreeString;
            degreeString.setNum(degree);
            this->m_classifier.setDegree(degree);
            for(double coef0 = p_coef0.from; coef0 <= p_coef0.to; coef0 += p_coef0.step)
            {
                QString coef0String;
                coef0String.setNum(coef0);
                this->m_classifier.setCoef0(coef0);
                for(double gamma = p_gamma.from; gamma <= p_gamma.to; gamma += p_gamma.step)
                {
                    QString gammaString;
                    gammaString.setNum(gamma);
                    this->m_classifier.setGamma(gamma);
                    for(double C = p_C.from; C <= p_C.to; C += p_C.step)
                    {
                        this->m_classifier.setC(C);
                        QList<float> accuracyList;

                        QList<LabelListsPair> pairs; // ***

                        for(int i = 0; i < kFold; ++i)
                        {
                            this->m_classifier.setDataForCrossValidation(i, kFold);
                            this->m_classifier.train();
                            int trueResults = 0;
                            QList<float> predList = this->m_classifier.predict(predSamples);
                            for(int i = 0; i < predList.size(); ++i)
                                if(predList[i] == this->m_classifier.correctLabels(predSamples)[i])
                                    ++trueResults;
                            float accuracy = (float)trueResults / predList.size();
                            accuracyList.push_back(accuracy);
                            // ***
                            LabelListsPair tmpPair(this->m_classifier.correctLabels(predSamples), predList);
                            pairs.push_back(tmpPair);
                        }
                        averageAccuracy = averageOfFloatList(accuracyList) * 100;

                         if(averageAccuracy > maxAccuracy)
                         {
                             maxAccuracy = averageAccuracy;
                             maxAccuracy_C = C;
                             maxAccuracy_coef0 = coef0;
                             maxAccuracy_degree = degree;
                             maxAccuracy_gamma = gamma;

                             maxAccuracyPairs = pairs; // ***
                         }

                        cString.setNum(C);
                        accuracyString.setNum(averageAccuracy);
                        resultString = "degree = " + degreeString + ",coef0 = " + coef0String + ", gamma = " + gammaString +
                                ", C = " + cString + ",\tточность: " + accuracyString + " %";
                        testResults.push_back("\n" + resultString);
                        qDebug() << resultString;
                        this->ui->textEdit_testResults->append(resultString);
                        this->ui->textEdit_testResults->repaint();
                    }
                }
            }
        }

        ma.setNum(maxAccuracy);
        maC.setNum(maxAccuracy_C);
        maGamma.setNum(maxAccuracy_gamma);
        maCoef0.setNum(maxAccuracy_coef0);
        maDegree.setNum(maxAccuracy_degree);
        QString maxAccuracyStr = "\n\nМаксимальная точность: " + ma + "%\n-C = " + maC + "\n-gamma = " + maGamma + "\n-coef0 = " + maCoef0
                + "\n-degree = " + maDegree;
        testResults.push_back(maxAccuracyStr);
        qDebug() << maxAccuracyStr;
        this->ui->textEdit_testResults->append(maxAccuracyStr);
        this->ui->textEdit_testResults->repaint();
    }

    QMessageBox messageBox(QMessageBox::Information, "Информация", "Выполнено\nЗаписать матрицу ошибок для комбинации параметров\n"
                                                                   "обеспечивающей максимальную точность?",
                           QMessageBox::Yes | QMessageBox::No);
    messageBox.setButtonText(QMessageBox::Yes, tr("Да"));
    messageBox.setButtonText(QMessageBox::No, tr("Нет"));
    if(messageBox.exec() == QMessageBox::Yes)
    {
        ResultMatrix resultMatrix(&this->m_labelsHash);
        for(auto pair: maxAccuracyPairs)
        {
            resultMatrix.addMatrix(pair.correctLabels, pair.predictLabels);
        }

        resultMatrix.setKernel(this->ui->comboBox->currentText());
        resultMatrix.setC(maC);
        resultMatrix.setGamma(maGamma);
        resultMatrix.setCoef0(maCoef0);
        resultMatrix.setDegree(maDegree);

        if(!resultMatrix.saveToCsv())
            QMessageBox::critical(0, "Ошибка", "Не удалось записать результаты в файл");
        else
            QMessageBox::information(0, "Информация", "Результаты записаны в файл");
    }

//    QFile file("C:/Qt/auto.txt");
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
//    {
//     QMessageBox::critical(0,"","Не удалось записать файл");
//     return;
//    }

//    QTextStream stream(&file);
//    stream << QString("Число классов: ") << this->m_labelsHash.size() << "\n";
//    for(auto it: testResults)
//        stream << it;
//    file.close();
//    QMessageBox::information(0, "Выполнено", "Результаты записаны в файл");
}

float MainWindow::averageOfFloatList(const QList<float> &list)
{
    float sum = 0;
    for(auto it: list)
        sum += it;
    return sum / list.size();
}

//void MainWindow::on_pushButton_predict_clicked()
//{
//    QList<float> pList = this->m_classifier.predict();
//    QFile file("C:/Qt/classifier.txt");
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
//    {
//     QMessageBox::critical(0,"","Не удалось записать файл");
//     return;
//    }
//    QTextStream stream(&file);
//    for(int i = 0; i < pList.size(); ++i)
//        stream << pList[i] << ", " << this->m_dataCollector.labels()[i] << '\n';

//    file.close();
//}

void MainWindow::on_pushButton_auto_clicked()
{
    if(this->checkAutoParameters())
    {
        this->ui->textEdit_testResults->clear();
        this->ui->pushButton_classifyTestSample->setEnabled(false);
        this->autoTest();
    }
}

void MainWindow::on_checkBox_maxIter_toggled(bool checked)
{
    this->ui->lineEdit_maxIter->setEnabled(checked);
}

void MainWindow::on_checkBox_epsilon_toggled(bool checked)
{
    this->ui->lineEdit_epsilon->setEnabled(checked);
}

void MainWindow::on_horizontalSlider_trainSetsPercent_valueChanged(int value)
{
    QString k;
    k.setNum(value);
    this->ui->lineEdit_trainSetsPercent->setText(k);
}

void MainWindow::on_pushButton_classifyTestSample_clicked()
{
    QString filePath = QFileDialog::getOpenFileName();
    if(!filePath.isEmpty())
    {
        QList<float> features = this->m_csvReader.testCsvData(filePath);
        float result = this->m_classifier.classifyTestSample(features);
        QString cellType = this->m_labelsHash.cellType(result);
        QMessageBox::information(0, "Результат классификации", "Класс клетки: " + cellType);
    }
}
