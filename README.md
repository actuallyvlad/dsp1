## <img src="images/logo.png" width="128px">  DSP1 - Digital Signal Processing tool 

DSP1 is a project for the Information Theory course at the ITMO University. It allows users to study signals (that are defined by the formula below), their interactions with gaussian noise and the resulted probabilities of such interactions.

Formula used to produce signal output:

![Formula image](images/formula.png)

## Screenshots

![Signal Tab](images/signal_example.png)

Noise is automatically cropped to fit within signal boundaries:

![Noise Tab](images/noise_example.png)

![Histogram Tab](images/histogram_example.png)

You can export probabilities to a .txt file for later analysis:

![Probability Tab](images/probability_example.png)

## Third-party Components

The project is developed using [Qt](https://www.qt.io/) (Qt Creator + Qt Designer). Plotting is done using [QCustomPlot](http://www.qcustomplot.com/).
