#pragma once

#include <QDialog>
#include <QVector>

#include "param.h"
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
    Param *m_param;

    std::vector<std::vector<double>> m_spectrum;
    std::vector<double> m_sectionX;
    std::vector<double> m_sectionY;

    // Optimization: Cache FFTW plan and buffers
    int m_cachedN = -1;
    fftw_plan m_cachedPlan = nullptr;
    fftw_complex* m_cachedIn = nullptr;
    fftw_complex* m_cachedOut = nullptr;

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