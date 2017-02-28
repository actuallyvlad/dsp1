#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dsp1_signal.h"
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_buttonSetDefault_clicked();

    void on_buttonRun_clicked();

    void on_buttonSave_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_checkBoxSumSignNoise_stateChanged(int arg1);

    void on_checkBoxConvSignNoise_stateChanged(int arg1);

    void on_buttonSaveSignProb_clicked();

    void on_buttonSaveNoiseProb_clicked();

    void on_buttonSaveSumProb_clicked();

private:
    Ui::MainWindow *ui;
    QCPBars* bars;
    QMap<QString, Signal> data;

    bool firstStart;

    void plotGraph(const Signal& signal) const;
    void plotGraph(const QVector<double>& xAxis, const QVector<double>& yAxis) const;
    void plotHistogram(const Signal& signal);
    void plotBars(const QVector<double>& xAxis, const QVector<double>& yAxis);

    void createTable(QTableWidget* table, int numOfRows, int numOfCols, const QVector<QString>& horizHeaders);
    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    void fillTable(QTableWidget* table, const std::initializer_list<QVector<T>> colsData);
    void saveTableToFile(QString fileName, QTableWidget* table);
    QStringList tableToQStringList(QTableWidget* table);
};

#endif // MAINWINDOW_H
