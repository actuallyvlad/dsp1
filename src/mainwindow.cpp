#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constants.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    bars(NULL),
    firstStart(true)
{
    ui->setupUi(this);
    on_buttonSetDefault_clicked();
    on_buttonRun_clicked();
    ui->tabWidget->setCurrentIndex(0);
    firstStart = false;
}

MainWindow::~MainWindow()
{
    ui->plot->clearPlottables();
    delete ui;
}

void MainWindow::createTable(QTableWidget* table, int numOfRows, int numOfCols, const QVector<QString>& horizHeaders) {
    table->setRowCount(numOfRows);
    table->setColumnCount(numOfCols);

    for (int i = 0; i < numOfCols; ++i) {
        table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);

        if (!horizHeaders.empty() && i < horizHeaders.size()) {
            table->setHorizontalHeaderItem(i, new QTableWidgetItem(horizHeaders[i]));
        }
    }

    table->verticalHeader()->hide();

    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


// Only works for numeric types
// Table can only be filled with data of __the same__ type

template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
void MainWindow::fillTable(QTableWidget* table, const std::initializer_list<QVector<T>> colsData) {
    int colIndex = 0;

    for (auto col : colsData) {
        for (int i = 0; i < col.size() && i < table->rowCount(); ++i) {
            table->setItem(i, colIndex, new QTableWidgetItem(QString::number(col[i])));
        }

        ++colIndex;
    }
}

QStringList MainWindow::tableToQStringList(QTableWidget* table) {
    int rows = table->rowCount();
    int columns = table->columnCount();

    QStringList result;
    QString headers;

    for (int i = 0; i < columns; ++i) {
        headers += table->horizontalHeaderItem(i)->data(Qt::DisplayRole).toString() + " ";
    }

    result  << headers << "\n";

    for (int i = 0; i < rows; ++i) {
        QString tempStr = "";

        for (int j = 0; j < columns; ++j) {
            if (table->item(i, j)) {
                tempStr += table->item(i, j)->text() + " ";
            }
        }

        result << tempStr << "\n";
        tempStr.clear();
    }

    return result;
}

void MainWindow::saveTableToFile(QString fileName, QTableWidget* table) {
    if (!fileName.contains(".txt", Qt::CaseInsensitive)) {
        fileName += ".txt";
    }

    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream output(&file);
        QStringList tableData = tableToQStringList(table);

        for (auto row : tableData) {
            output << row;
        }
    }
}

void MainWindow::on_buttonSetDefault_clicked()
{
    int currentTabIndex = ui->tabWidget->currentIndex();
    QString currentTabTitle = ui->tabWidget->tabText(currentTabIndex);

    if (currentTabTitle == tabSignalTitle || firstStart) {
            ui->varSignalStep->setText(QString::number(signalDefaultStep));
            ui->varSignalCount->setText(QString::number(signalDefaultCount));
            ui->varSignalA->setText(QString::number(signalDefaultA));
            ui->varSignalB->setText(QString::number(signalDefaultB));
            ui->varSignalSigma->setText(QString::number(signalDefaultSigma));
            ui->varSignalMu->setText(QString::number(signalDefaultMu));
    }
    if (currentTabTitle == tabNoiseTitle || firstStart) {
            ui->varNoiseMean->setText(QString::number(noiseDefaultMean));
            ui->varNoiseSD->setText(QString::number(noiseDefaultSD));
            ui->varNoiseCount->setText(QString::number(noiseDefaultCount));
    }    
    if (currentTabTitle == tabHistogramTitle || firstStart) {
        ui->varHistogramBins->setText(QString::number(histogramDefaultBins));
        ui->radioSignal->setChecked(true);
    }

}

void MainWindow::plotGraph(const Signal& signal) const {
    QVector<double> x(signal.getSize());
    std::iota(x.begin(), x.end(), 0);

    plotGraph(x, signal.getSignal());
}

void MainWindow::plotGraph(const QVector<double>& xAxis, const QVector<double>& yAxis) const {
    ui->plot->clearPlottables();

    ui->plot->addGraph();
    ui->plot->graph(0)->setData(xAxis, yAxis);

    ui->plot->yAxis->rescale();
    ui->plot->xAxis->rescale();

    ui->plot->replot();
}

void MainWindow::plotHistogram(const Signal& signal) {
    plotBars(signal.getHistogramXAxis(), signal.getHistogramYAxis());
}

void MainWindow::plotBars(const QVector<double>& xAxis, const QVector<double>& yAxis) {
    ui->plot->clearPlottables();

    ui->plot->addGraph();

    bars = new QCPBars(ui->plot->xAxis, ui->plot->yAxis);

    bars->setData(xAxis, yAxis);
    bars->setWidth(1/xAxis.size());
    ui->plot->yAxis->rescale();
    ui->plot->xAxis->rescale();
    ui->plot->replot();
}

void MainWindow::on_buttonRun_clicked()
{
    int currentTabIndex = ui->tabWidget->currentIndex();
    QString currentTabTitle = ui->tabWidget->tabText(currentTabIndex);

    // signal variables
    int signalCount = ui->varSignalCount->text().toInt();
    double signalStep = ui->varSignalStep->text().toDouble();
    double signalA = ui->varSignalA->text().toDouble();
    double signalB = ui->varSignalB->text().toDouble();
    double signalSigma = ui->varSignalSigma->text().toDouble();
    double signalMu = ui->varSignalMu->text().toDouble();

    // noise variables
    double noiseMean = ui->varNoiseMean->text().toDouble();
    double noiseSD = ui->varNoiseSD->text().toDouble();
    int noiseCount = ui->varNoiseCount->text().toDouble();

    // histogram variables
    double histogramBins = ui->varHistogramBins->text().toDouble();

    Signal tempSignal;

    tempSignal.setByFormula(signalCount, signalStep, signalA, signalB, signalSigma, signalMu);
    data[signalLabel] = tempSignal;

    tempSignal.setByNoise(noiseCount, noiseMean, noiseSD);
    data[noiseLabel] = tempSignal;

    tempSignal.setBySum({data[signalLabel], data[noiseLabel]});
    data[sumLabel] = tempSignal;

    tempSignal.setByConvolution(data[signalLabel], data[noiseLabel]);
    data[convolutionLabel] = tempSignal;

    if (currentTabTitle == tabHistogramTitle) {
        QString radio;

        if (ui->radioSignal->isChecked()) {
            radio = signalLabel;
        }
        else if (ui->radioNoise->isChecked()) {
            radio = noiseLabel;
        }
        else if (ui->radioSum->isChecked()) {
            radio = sumLabel;
        }

        data[radio].setHistogram(histogramBins);

        if (ui->radioConvolution->isChecked()) {
            radio = signalLabel;
            data[radio].convolveHistograms(data[noiseLabel], histogramBins);
        }

        ui->fieldEntropy->setText(QString::number(data[radio].getEntropy()));

        plotHistogram(data[radio]);
    }
    else if (ui->checkBoxSumSignNoise->isChecked()) {
        plotGraph(data[sumLabel]);
    }
    else if (ui->checkBoxConvSignNoise->isChecked()) {
        plotGraph(data[convolutionLabel]);
    }
    else if (currentTabTitle == tabSignalTitle) {
        plotGraph(data[signalLabel]);
    }
    else if (currentTabTitle == tabNoiseTitle) {
        plotGraph(data[noiseLabel]);
    }

    // calculate & display probability

    createTable(ui->tableSignProb, data[signalLabel].getSize(), tableProbabilityColumnsCount, tableProbabilityHeaders);
    createTable(ui->tableNoiseProb, data[noiseLabel].getSize(), tableProbabilityColumnsCount, tableProbabilityHeaders);
    createTable(ui->tableSumProb, data[sumLabel].getSize(), tableProbabilityColumnsCount, tableProbabilityHeaders);

    fillTable(ui->tableSignProb, {data[signalLabel].getHistogramXAxis(), data[signalLabel].getProbability()});
    fillTable(ui->tableNoiseProb, {data[noiseLabel].getHistogramXAxis(), data[noiseLabel].getProbability()});
    fillTable(ui->tableSumProb, {data[sumLabel].getHistogramXAxis(), data[sumLabel].getProbability()});
}


void MainWindow::on_buttonSave_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::homePath(),
                                                        tr("Images (*.png)"));

    if (!fileName.contains(".png", Qt::CaseInsensitive)) {
        fileName += ".png";
    }

    ui->plot->savePng(fileName);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    QString currentTabTitle = ui->tabWidget->tabText(index);

    ui->buttonSave->setVisible(true);
    ui->buttonSetDefault->setVisible(true);
    ui->buttonRun->setVisible(true);
    ui->checkBoxSumSignNoise->setVisible(false);
    ui->checkBoxConvSignNoise->setVisible(false);

    if (currentTabTitle == tabSignalTitle || currentTabTitle == tabNoiseTitle) {
        ui->checkBoxSumSignNoise->setVisible(true);
        ui->checkBoxConvSignNoise->setVisible(true);
    }
    else if (currentTabTitle == tabProbabilityTitle) {
        ui->buttonSave->setVisible(false);
        ui->buttonSetDefault->setVisible(false);
        ui->buttonRun->setVisible(false);
    }
}

void MainWindow::on_checkBoxSumSignNoise_stateChanged(int arg1)
{
    if (arg1) {
        ui->checkBoxConvSignNoise->setDisabled(true);
    }
    else {
        ui->checkBoxConvSignNoise->setDisabled(false);
    }
}

void MainWindow::on_checkBoxConvSignNoise_stateChanged(int arg1)
{
    if (arg1) {
        ui->checkBoxSumSignNoise->setDisabled(true);
    }
    else {
        ui->checkBoxSumSignNoise->setDisabled(false);
    }
}

void MainWindow::on_buttonSaveSignProb_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::homePath(),
                                                        tr("Text files (*.txt)"));

    saveTableToFile(fileName, ui->tableSignProb);
}

void MainWindow::on_buttonSaveNoiseProb_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::homePath(),
                                                        tr("Text files (*.txt)"));

    saveTableToFile(fileName, ui->tableNoiseProb);
}

void MainWindow::on_buttonSaveSumProb_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::homePath(),
                                                        tr("Text files (*.txt)"));

    saveTableToFile(fileName, ui->tableSumProb);
}
