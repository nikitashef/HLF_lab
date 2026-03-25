#include "lab6.h"
#include "ui_lab5.h"

#include "param.h"
#include "lab4.h"   // Sample / FFT
#include "qcustomplot.h"

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
    delete m_param;
    delete ui;
}

////////////////////////////////////////////////////////////

void lab6::readUI()
{
    m_param->m_dFunctionSize = ui->lineEdit_size->text().toDouble();
    m_param->m_bIsCircle = ui->radioCircle->isChecked();

    int size = ui->comboBox_sample->currentText().toInt();

    if (ui->comboBox_base->currentIndex() == 0)
    {
        m_param->Set_Step_Function(
            ui->lineEdit_step_func->text().toDouble(), size);
    }
    else
    {
        m_param->Set_Step_Spectr(
            ui->lineEdit_step_spec->text().toDouble(), size);
    }
}

void lab6::updateUI()
{
    ui->lineEdit_step_func->setText(
        QString::number(m_param->Get_Step_Function()));

    ui->lineEdit_step_spec->setText(
        QString::number(m_param->Get_Step_Spectr()));
}

////////////////////////////////////////////////////////////

void lab6::onCalculate()
{
    readUI();

    // 🔴 ВАЖНО: здесь ты должен использовать свой FFT из lr4
    // пример (замени на свой код):
    /*
    Sample<double> input;
    SampleComplex output;

    // заполнение input (круг/квадрат)
    // FFT
    // output.GetIntensity() → в m_spectrum
    */

    drawFunction();
    drawSpectrum();
    drawSections();
}

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
    m_param->m_bIsCircle = ui->radioCircle->isChecked();
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

    m_param->Set_Step_Function(
        ui->lineEdit_step_func->text().toDouble(), N);

    updateUI();
}

void lab6::onStepSpectrChanged()
{
    int N = ui->comboBox_sample->currentText().toInt();

    m_param->Set_Step_Spectr(
        ui->lineEdit_step_spec->text().toDouble(), N);

    updateUI();
}

void lab6::onFunctionSizeChanged()
{
    m_param->m_dFunctionSize =
        ui->lineEdit_size->text().toDouble();
}

////////////////////////////////////////////////////////////

void lab6::drawFunction()
{
    ui->plot_function->clearGraphs();
    ui->plot_function->addGraph();

    QVector<double> x, y;

    for (int i = 0; i < 100; i++)
    {
        x.push_back(i);
        y.push_back(m_param->m_bIsCircle ? 1 : 0);
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

    map->data()->setSize(100, 100);
    map->data()->setRange(QCPRange(0,100), QCPRange(0,100));

    ui->plot_spectrum->replot();
}

void lab6::drawSections()
{
    ui->plot_section->clearGraphs();

    ui->plot_section->addGraph();
    ui->plot_section->addGraph();

    QVector<double> x(100), y(100);

    for (int i = 0; i < 100; i++)
    {
        x[i] = i;
        y[i] = sin(i);
    }

    ui->plot_section->graph(0)->setData(x, y);
    ui->plot_section->graph(1)->setData(x, y);

    ui->plot_section->replot();
}

void lab6::onCalculate()
{
    readUI();

    int N = m_param->Get_Size();

    // 1. Создаем входную выборку
    Sample<double> input;
    input.Resize(N);

    input.SetValue(0.0);

    double size = m_param->m_dFunctionSize;
    double dx = m_param->Get_Step_Function();

    double center = N / 2.0;

    // 2. Заполняем круг / квадрат
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            double x = (i - center) * dx;
            double y = (j - center) * dx;

            if (m_param->m_bIsCircle)
            {
                double r = sqrt(x * x + y * y);
                if (r <= size / 2.0)
                    input(i, j) = 1.0;
            }
            else
            {
                if (fabs(x) <= size / 2.0 &&
                    fabs(y) <= size / 2.0)
                    input(i, j) = 1.0;
            }
        }
    }

#ifdef FFTW_AVAILABLE

    // 3. Подготовка FFTW
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N * N);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N * N);

    // 4. Копирование данных
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int idx = i * N + j;
            in[idx][0] = input(i, j);
            in[idx][1] = 0.0;
        }
    }

    // 5. FFT
    fftw_plan plan = fftw_plan_dft_2d(
        N, N,
        in, out,
        FFTW_FORWARD,
        FFTW_ESTIMATE);

    fftw_execute(plan);

    // 6. Интенсивность + fftshift
    m_spectrum.resize(N);
    for (int i = 0; i < N; i++)
        m_spectrum[i].resize(N);

    double maxVal = 0.0;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int idx = i * N + j;

            double re = out[idx][0];
            double im = out[idx][1];

            double val = re * re + im * im;

            // fftshift
            int ii = (i + N / 2) % N;
            int jj = (j + N / 2) % N;

            m_spectrum[ii][jj] = val;

            if (val > maxVal) maxVal = val;
        }
    }

    // 7. Нормировка
    if (maxVal > 0)
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                m_spectrum[i][j] /= maxVal;
    }

    // 8. Сечения
    m_sectionX.resize(N);
    m_sectionY.resize(N);

    int mid = N / 2;

    for (int i = 0; i < N; i++)
    {
        m_sectionX[i] = m_spectrum[mid][i];
        m_sectionY[i] = m_spectrum[i][mid];
    }

    // 9. Очистка
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

#else
    // если FFTW нет — просто заглушка
    m_spectrum.clear();
#endif

    drawFunction();
    drawSpectrum();
    drawSections();
}