#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QVector>

// Signal default parameters
const double signalDefaultStep = 0.1;
const int signalDefaultCount = 1000;
const double signalDefaultA = 500;
const double signalDefaultB = 1;
const double signalDefaultSigma = 10;
const double signalDefaultMu = 50;

// Noise default parameters
const int noiseDefaultCount = signalDefaultCount;
const double noiseDefaultMean = 5;
const double noiseDefaultSD = 2;

// Histogram default parameters
const int histogramDefaultBins = 100;

// Signals labels
const QString signalLabel = "Signal";
const QString noiseLabel = "Noise";
const QString sumLabel = "Sum";
const QString convolutionLabel = "Convolution";

// Tab titles
const QString tabSignalTitle = "Signal";
const QString tabNoiseTitle = "Noise";
const QString tabHistogramTitle = "Histogram";
const QString tabProbabilityTitle = "Probability";

// Probability tables number of columns
const int tableProbabilityColumnsCount = 2;

// Probability tables headers
const QVector<QString> tableProbabilityHeaders {
    "Bin",
    "Probability"
};

#endif // CONSTANTS_H
