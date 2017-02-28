#include "dsp1_signal.h"

Signal::Signal() : min(0), max(0), histogramBins(0) {
}

void Signal::setMinMax() {
    auto minmax = std::minmax_element(signal.begin(), signal.end());
    min = *(minmax.first);
    max = *(minmax.second);
}

void Signal::setBySum(const std::initializer_list<Signal> signalsToAdd) {
    size_t minSize = 0;

    for (auto s : signalsToAdd) {
        if (s.getSize() < minSize || minSize == 0) {
            minSize = s.getSize();
        }
    }

    signal.clear();
    signal.reserve(minSize);

    QVector<double> tempSignal;

    for (size_t i = 0; i < minSize; ++i) {
        double sum = 0;

        for (auto s : signalsToAdd) {
            tempSignal = s.getSignal();
            sum += tempSignal[i];
        }

        signal.push_back(sum);
    }

    setMinMax();
}

void Signal::setByConvolution(const Signal& signalA, const Signal& signalB) {
    int minSize = std::min(signalA.getSize(), signalB.getSize());

    signal.clear();
    signal.reserve(minSize);

    QVector<double> signalAData = signalA.getSignal();
    QVector<double> signalBData = signalB.getSignal();

    signal = convolve(signalAData, signalBData);

    setMinMax();
}

QVector<double> Signal::convolve(const QVector<double>& dataA, const QVector<double>& dataB) {
    int minSize = std::min(dataA.size(), dataB.size());
    int convSize = 2*dataA.size() - 1;

    QVector<double> convolution;
    convolution.resize(convSize);

    for (int i = 0; i < convSize; ++i) {
        for (int j = 0; j <= i; ++j) {
            if ((i-j) < minSize && j < minSize) {
                convolution[convSize - 1 - i] += dataA[i-j] * dataB[j];
            }
        }
    }

    return convolution;
}

void Signal::convolveHistograms(Signal& anotherSignal, int bins) {
    // make sure that histograms exist and use the same bins value
    setHistogram(bins);
    anotherSignal.setHistogram(bins);

    QVector<double> convolution = convolve(histogramYAxis, anotherSignal.getHistogramYAxis());

    histogramYAxis.clear();
    histogramYAxis = convolution;

    histogramXAxis.clear();
    histogramXAxis.resize(convolution.size());

    std::iota(histogramXAxis.begin(), histogramXAxis.end(), 0);
}

void Signal::setByFormula(int count, double step, double a, double sigma, double mu) {
    signal.clear();
    signal.reserve(count);

    auto formula = [&, count, step, a, sigma, mu](int k) -> double {
        double result = 0;
        double a_k = 0;
        double cos_f = 0;
        double x_k = k * step;

        a_k = ( a * exp( -(x_k-mu)*(x_k-mu)/(2*sigma*sigma) ) ) / ( sigma*sqrt(2*M_PI) );
        cos_f = cos( pow((x_k-count*step/2), 2) / 20 );

        result = a_k * cos_f;
        return result;
    };

    for(int k = 0; k < count; ++k) {
        signal.push_back(formula(k));
    }

    setMinMax();
}

/* Used as an example of another formula
void Signal::setByTriangle(int count, double height) {
    signal.clear();
    signal.reserve(count);

    auto formula = [&, count, height](int k) -> double {
        double result = 0;
        double halfPeriod = count/2;

        result = (height/halfPeriod) * (halfPeriod - abs(k % count - halfPeriod));

        return result;
    };

    for(int k = 0; k < count; ++k) {
        signal.push_back(formula(k));
    }

    setMinMax();
}
*/

void Signal::setByNoise(int count, double mean, double sd, double lowBoundary, double highBoundary) {
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean, sd);

    signal.clear();

    for (int i = 0; i < count; ++i) {
        double number = distribution(generator);

        if (number >= lowBoundary && number <= highBoundary) {
            signal.push_back(number);
        }
        else {
            --i;
        }
    }


    setMinMax();
}

double Signal::hround(double value, double bin) {
   return ( floor(value/bin)  );
}

void Signal::setHistogram(double bins) {
    QMap<double, double> histogram;
    histogramBins = bins;
    histogramXAxis.clear();
    histogramYAxis.clear();

    histogramBin = fabs( getMax()-getMin() ) / histogramBins;

    for (auto i = 0; i < signal.size(); ++i) {
        ++histogram[hround(signal[i], histogramBin)];
    }

    // fill unused keys values with zeros
    for (double i = hround(getMin(), histogramBin); i <= hround(getMax(), histogramBin); ++i) {
        histogram[i];
    }

    histogramXAxis.reserve(histogramBins);
    histogramYAxis.reserve(histogramBins);

    for (auto const& point : histogram.toStdMap()) {
        histogramXAxis.push_back(point.first * histogramBin);
        histogramYAxis.push_back(point.second / signal.size());
    }
}

const QVector<double> &Signal::getSignal() const {
    return signal;
}

double Signal::getMax() const {
    return max;
}

double Signal::getMin() const {
    return min;
}

const QVector<double>& Signal::getHistogramXAxis() const {
    return histogramXAxis;
}

const QVector<double>& Signal::getHistogramYAxis() const {
    return histogramYAxis;
}

const QVector<double>& Signal::getProbability() const {
    return histogramYAxis;
}

double Signal::getEntropy() const {
    double entropy = 0;

    for (int i = 0; i < histogramYAxis.size(); ++i) {
        if (histogramYAxis[i]) {
            entropy += histogramYAxis[i] * log2(histogramYAxis[i]);
        }
    }

    return ( -entropy );
}

size_t Signal::getSize() const {
    return signal.size();
}
