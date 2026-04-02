#include "lab6.h"
#include "ui_lab6.h"

#include "lr6param.h"
#include "lr6calculation.h"
#include "lab4.h"   // Sample / FFT
#include "qcustomplot.h"

#ifdef FFTW_AVAILABLE
#include <fftw3.h>
#endif

lab6::lab6(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::lab5)
{
    ui->setupUi(this);

    m_param = new Param();

    connect(ui->button_calc, &QPushButton::clicked, this, &lab6::onCalculate);
    connect(ui->button_reset, &QPushButton::clicked, this, &lab6::onReset);
    connect(ui->button_recalc, &QPushButton::clicked, this, &lab6::onRecalculate);
    connect(ui->button_redraw, &QPushButton::clicked, this, &lab6::onRedraw);

    connect(ui->comboBox_base, &QComboBox::currentIndexChanged,
            this, &lab6::onBaseParamChanged);

    connect(ui->comboBox_sample, &QComboBox::currentIndexChanged,
            this, &lab6::onSizeChanged);

    connect(ui->radioCircle, &QRadioButton::toggled,
            this, &lab6::onShapeChanged);

    connect(ui->lineEdit_step_func, &QLineEdit::editingFinished,
            this, &lab6::onStepFunctionChanged);

    connect(ui->lineEdit_step_spec, &QLineEdit::editingFinished,
            this, &lab6::onStepSpectrChanged);

    connect(ui->lineEdit_size, &QLineEdit::editingFinished,
            this, &lab6::onFunctionSizeChanged);

    updateUI();
}

lab6::~lab6()
{
#ifdef FFTW_AVAILABLE
    // Очистка кэшированных ресурсов FFTW
    if (m_cachedPlan) {
        fftw_destroy_plan(m_cachedPlan);
        m_cachedPlan = nullptr;
    }
    if (m_cachedIn) {
        fftw_free(m_cachedIn);
        m_cachedIn = nullptr;
    }
    if (m_cachedOut) {
        fftw_free(m_cachedOut);
        m_cachedOut = nullptr;
    }
#endif
    delete m_param;
    delete ui;
}

////////////////////////////////////////////////////////////

// Чтение параметров из интерфейса
void lab6::readUI()
{
    m_param->size = ui->lineEdit_size->text().toDouble();
    m_param->isCircle = ui->radioCircle->isChecked();
    int size = ui->comboBox_sample->currentText().toInt();
    if (ui->comboBox_base->currentIndex() == 0)
    {
        m_param->SetDx(ui->lineEdit_step_func->text().toDouble(), size);
    }
    else
    {
        m_param->SetDnu(ui->lineEdit_step_spec->text().toDouble(), size);
    }
}

// Обновление интерфейса по параметрам
void lab6::updateUI()
{
    ui->lineEdit_step_func->setText(QString::number(m_param->dx));
    ui->lineEdit_step_spec->setText(QString::number(m_param->dnu));
}

////////////////////////////////////////////////////////////

// removed duplicate placeholder for lab6::onCalculate();
// the full implementation is defined later in this file

void lab6::onReset()
{
    m_param->Reset();
    updateUI();
}

void lab6::onRecalculate()
{
    readUI();
    updateUI();
}

void lab6::onRedraw()
{
    drawFunction();
    drawSpectrum();
    drawSections();
}

////////////////////////////////////////////////////////////

void lab6::onShapeChanged()
{
    m_param->isCircle = ui->radioCircle->isChecked();
}

void lab6::onSizeChanged()
{
    readUI();
}

void lab6::onBaseParamChanged()
{
    bool isDx = ui->comboBox_base->currentIndex() == 0;

    ui->lineEdit_step_func->setEnabled(isDx);
    ui->lineEdit_step_spec->setEnabled(!isDx);
}

void lab6::onStepFunctionChanged()
{
    int N = ui->comboBox_sample->currentText().toInt();
    m_param->SetDx(ui->lineEdit_step_func->text().toDouble(), N);
    updateUI();
}

void lab6::onStepSpectrChanged()
{
    int N = ui->comboBox_sample->currentText().toInt();
    m_param->SetDnu(ui->lineEdit_step_spec->text().toDouble(), N);
    updateUI();
}

void lab6::onFunctionSizeChanged()
{
    m_param->size = ui->lineEdit_size->text().toDouble();
}

////////////////////////////////////////////////////////////

void lab6::drawFunction()
{
    ui->plot_function->clearGraphs();
    ui->plot_function->addGraph();
    QVector<double> x, y;
    int N = m_param->N;
    x.reserve(N); y.reserve(N);
    // Рисуем сечение функции по центру
    int mid = N / 2;
    for (int i = 0; i < N; i++)
    {
        x.push_back(i);
        y.push_back(m_calc->function[mid][i]);
    }
    ui->plot_function->graph(0)->setData(x, y);
    ui->plot_function->replot();
}

void lab6::drawSpectrum()
{
    ui->plot_spectrum->clearPlottables();
    QCPColorMap *map = new QCPColorMap(
        ui->plot_spectrum->xAxis,
        ui->plot_spectrum->yAxis);
    int nx = (int)m_calc->spectrum.size();
    if (nx == 0) {
        ui->plot_spectrum->replot();
        return;
    }
    int ny = (int)m_calc->spectrum[0].size();
    map->data()->setSize(nx, ny);
    map->data()->setRange(QCPRange(0, nx-1), QCPRange(0, ny-1));
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            double val = m_calc->spectrum[i][j];
            double logVal = std::log10(1.0 + val * 100.0) / std::log10(101.0);
            map->data()->setCell(i, j, logVal);
        }
    }
    ui->plot_spectrum->rescaleAxes();
    ui->plot_spectrum->replot();
}

void lab6::drawSections()
{
    ui->plot_section->clearGraphs();
    ui->plot_section->addGraph();
    ui->plot_section->addGraph();
    int N = (int)m_calc->sectionX.size();
    if (N == 0) {
        ui->plot_section->replot();
        return;
    }
    QVector<double> x(N), sx(N), sy(N);
    for (int i = 0; i < N; ++i) {
        x[i] = i;
        sx[i] = m_calc->sectionX[i];
        sy[i] = m_calc->sectionY[i];
    }
    ui->plot_section->graph(0)->setData(x, sx);
    ui->plot_section->graph(1)->setData(x, sy);
    ui->plot_section->rescaleAxes();
    ui->plot_section->replot();
}

void lab6::onCalculate()
{
    readUI();
    m_calc->Calculate(*m_param);
    drawFunction();
    drawSpectrum();
    drawSections();
}
