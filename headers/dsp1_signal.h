#ifndef DSP1_SIGNAL_H
#define DSP1_SIGNAL_H

#include <QVector>

class Signal
{
    public:
        Signal();
        void setBySum(const std::initializer_list<Signal> signalsToAdd);
        void setByConvolution(const Signal& signalA, const Signal& signalB);
        void setByFormula(int count, double step, double a, double sigma, double mu);
        void setByNoise(int count, double mean, double sd, double lowBoundary, double highBoundary);
        void setHistogram(double bins);
        const QVector<double>& getSignal() const;
        const QVector<double>& getHistogramXAxis() const;
        const QVector<double>& getHistogramYAxis() const;
        void convolveHistograms(Signal &anotherSignal, int bins);
        const QVector<double>& getProbability() const;
        double getEntropy() const;
        double getMin() const;
        double getMax() const;
        size_t getSize() const;
    private:
        QVector<double> convolve(const QVector<double>& dataA, const QVector<double>& dataB);
        double hround(double value, double bin);
        void setMinMax();

        QVector<double> signal;
        QVector<double> probability;
        QVector<double> histogramXAxis;
        QVector<double> histogramYAxis;
        double min;
        double max;
        double histogramBins;
};

#endif // DSP1_SIGNAL_H
