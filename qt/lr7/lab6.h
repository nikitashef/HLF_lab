#pragma once

#include <QDialog>
#include <QVector>

#include "lr6param.h"
#include "lr6calculation.h"
#include "lab4.h"

#ifdef FFTW_AVAILABLE
#include <fftw3.h>
#endif

namespace Ui { class lab5; }

class lab6 : public QDialog
{
    Q_OBJECT

public:
    explicit lab6(QWidget *parent = nullptr);
    ~lab6();


private:
    Ui::lab5 *ui;
    Lr6Param *m_param;
    Lr6Calculation *m_calc;

    void readUI();
    void updateUI();

    void drawFunction();
    void drawSpectrum();
    void drawSections();

private slots:
    void onCalculate();
    void onReset();
    void onRecalculate();
    void onRedraw();

    void onShapeChanged();
    void onSizeChanged();
    void onBaseParamChanged();
    void onStepFunctionChanged();
    void onStepSpectrChanged();
    void onFunctionSizeChanged();
};