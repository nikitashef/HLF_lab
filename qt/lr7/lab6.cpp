#include "lab6.h"
#include "ui_lab6.h"

#include "param.h"
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

    int N = 100;
    x.reserve(N); y.reserve(N);
    for (int i = 0; i < N; i++)
    {
        x.push_back(i);
        y.push_back(m_param->m_bIsCircle ? 1.0 : 0.0);
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

    int nx = (int)m_spectrum.size();
    if (nx == 0) {
        // пусто — ничего не рисуем
        ui->plot_spectrum->replot();
        return;
    }
    int ny = (int)m_spectrum[0].size();

    map->data()->setSize(nx, ny);
    map->data()->setRange(QCPRange(0, nx-1), QCPRange(0, ny-1));

    // Заполнение цветовой карты масштабированными по логарифму значениями спектра для лучшей визуализации
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            double val = m_spectrum[i][j];
            // Применение логарифмической шкалы (1 + log10) для улучшения видимости пиков низкой интенсивности
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
    int N = (int)m_sectionX.size();
    if (N == 0) {
        ui->plot_section->replot();
        return;
    }

    QVector<double> x(N), sx(N), sy(N);
    for (int i = 0; i < N; ++i) {
        x[i] = i;
        sx[i] = m_sectionX[i];
        sy[i] = m_sectionY[i];
    }

    ui->plot_section->graph(0)->setData(x, sx);
    ui->plot_section->graph(1)->setData(x, sy);

    ui->plot_section->rescaleAxes();
    ui->plot_section->replot();
}

void lab6::onCalculate()
{
    readUI();

    int N = m_param->Get_Size();

    double size = m_param->m_dFunctionSize;
    double dx = m_param->Get_Step_Function();
    double center = N / 2.0;

    // Оптимизация 1: Предварительное вычисление пороговых значений
    double sizeHalf = size / 2.0;
    double sizeHalfSq = sizeHalf * sizeHalf;  // Избегаем sqrt при сравнении для круга

    // 3. Compute 2D FFT
#ifdef FFTW_AVAILABLE
    // Использование FFTW для вычисления FFT
    // Оптимизация 4: Кэширование плана FFTW и буферов между вызовами
    if (m_cachedN != N || !m_cachedPlan) {
        // Инвалидация кэша, если размер изменился
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

        // Выделение новых буферов
        m_cachedIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N * N);
        m_cachedOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N * N);
        m_cachedPlan = fftw_plan_dft_2d(N, N, m_cachedIn, m_cachedOut, FFTW_FORWARD, FFTW_ESTIMATE);
        m_cachedN = N;
    }

    fftw_complex* in = m_cachedIn;
    fftw_complex* out = m_cachedOut;
    fftw_plan plan = m_cachedPlan;

    // Оптимизация 2: Заполнение буфера FFTW напрямую, избегая промежуточного буфера Sample<double>
    // и уменьшение копирований в памяти. Заполняем только ненулевые значения.
    memset(in, 0, sizeof(fftw_complex) * N * N);

    if (m_param->m_bIsCircle)
    {
        // Для круга: используем квадрат расстояния, чтобы избежать вызова sqrt
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                double x = (i - center) * dx;
                double y = (j - center) * dx;
                double distSq = x * x + y * y;
                if (distSq <= sizeHalfSq)
                {
                    int idx = i * N + j;
                    in[idx][0] = 1.0;
                    in[idx][1] = 0.0;
                }
            }
        }
    }
    else
    {
        // Для прямоугольника: избегаем вызовов fabs
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                double x = (i - center) * dx;
                double y = (j - center) * dx;
                if (x >= -sizeHalf && x <= sizeHalf &&
                    y >= -sizeHalf && y <= sizeHalf)
                {
                    int idx = i * N + j;
                    in[idx][0] = 1.0;
                    in[idx][1] = 0.0;
                }
            }
        }
    }

    // Выполнение FFT с использованием кэшированного плана
    fftw_execute(plan);

    // Обработка выходных данных: вычисление интенсивности и применение fftshift
    m_spectrum.resize(N);
    for (int i = 0; i < N; ++i)
        m_spectrum[i].resize(N);

    double maxVal = 0.0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int idx = i * N + j;
            double re = out[idx][0];
            double im = out[idx][1];
            double val = re * re + im * im;

            // Применение fftshift
            int ii = (i + N / 2) % N;
            int jj = (j + N / 2) % N;
            m_spectrum[ii][jj] = val;

            if (val > maxVal) maxVal = val;
        }
    }

    // Нормализация
    if (maxVal > 0) {
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                m_spectrum[i][j] /= maxVal;
    }

    // Примечание: План и буферы кэшируются, освобождаются только в деструкторе

#else
    // Внутренняя реализация FFT (резервный вариант, когда FFTW недоступна)
    auto is_power_of_two = [](int x){ return x>0 && ( (x & (x-1)) == 0 ); };

    // 1. Создаем входную выборку
    Sample<double> input;
    input.Resize(N);

    // Оптимизация 3: Пропускаем полную инициализацию нулями - заполняем только нужные значения
    double sizeHalf = size / 2.0;
    double sizeHalfSq = sizeHalf * sizeHalf;

    if (m_param->m_bIsCircle)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                double x = (i - center) * dx;
                double y = (j - center) * dx;
                double distSq = x * x + y * y;
                input(i, j) = (distSq <= sizeHalfSq) ? 1.0 : 0.0;
            }
        }
    }
    else
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                double x = (i - center) * dx;
                double y = (j - center) * dx;
                input(i, j) = (x >= -sizeHalf && x <= sizeHalf &&
                               y >= -sizeHalf && y <= sizeHalf) ? 1.0 : 0.0;
            }
        }
    }

    // 1D in-place Cooley-Tukey FFT
    auto fft1d = [&](std::vector<std::complex<double>>& a, bool invert)
    {
        int n = (int)a.size();

        // Обращение битов
        for (int i = 1, j = 0; i < n; ++i) {
            int bit = n >> 1;
            for (; j & bit; bit >>= 1) j ^= bit;
            j ^= bit;
            if (i < j) std::swap(a[i], a[j]);
        }

        for (int len = 2; len <= n; len <<= 1) {
            double ang = 2 * M_PI / len * (invert ? -1 : 1);
            std::complex<double> wlen(std::cos(ang), std::sin(ang));
            for (int i = 0; i < n; i += len) {
                std::complex<double> w(1.0, 0.0);
                for (int j = 0; j < len/2; ++j) {
                    std::complex<double> u = a[i+j];
                    std::complex<double> v = a[i+j+len/2] * w;
                    a[i+j] = u + v;
                    a[i+j+len/2] = u - v;
                    w *= wlen;
                }
            }
        }

        if (invert) {
            for (int i = 0; i < n; ++i)
                a[i] /= (double)n;
        }
    };

    // Резервное ДПФ (медленное) для произвольных размеров
    auto dft2d_slow = [&](const Sample<double>& in, std::vector<std::vector<double>>& out)
    {
        out.assign(N, std::vector<double>(N, 0.0));
        double maxVal = 0.0;
        for (int u = 0; u < N; ++u) {
            for (int v = 0; v < N; ++v) {
                std::complex<double> sum(0.0, 0.0);
                for (int x = 0; x < N; ++x) {
                    for (int y = 0; y < N; ++y) {
                        double angle = -2.0 * M_PI * ( (double)u * x / N + (double)v * y / N );
                        std::complex<double> w(std::cos(angle), std::sin(angle));
                        sum += std::complex<double>(in(x,y), 0.0) * w;
                    }
                }
                double val = std::norm(sum);
                int uu = (u + N/2) % N;
                int vv = (v + N/2) % N;
                out[uu][vv] = val;
                if (val > maxVal) maxVal = val;
            }
        }
        // normalize
        if (maxVal > 0) for (int i=0;i<N;i++) for (int j=0;j<N;j++) out[i][j] /= maxVal;
    };

    m_spectrum.resize(N);
    for (int i=0;i<N;i++) m_spectrum[i].resize(N);

    if (is_power_of_two(N)) {
        // Подготовка буфера для комплексных чисел
        std::vector<std::complex<double>> row(N), col(N);

        // 2D FFT: строки
        std::vector<std::vector<std::complex<double>>> data(N, std::vector<std::complex<double>>(N));
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                data[i][j] = std::complex<double>(input(i,j), 0.0);

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) row[j] = data[i][j];
            fft1d(row, false);
            for (int j = 0; j < N; ++j) data[i][j] = row[j];
        }

        // столбцы
        for (int j = 0; j < N; ++j) {
            for (int i = 0; i < N; ++i) col[i] = data[i][j];
            fft1d(col, false);
            for (int i = 0; i < N; ++i) data[i][j] = col[i];
        }

        double maxVal = 0.0;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                double val = std::norm(data[i][j]);
                int ii = (i + N/2) % N;
                int jj = (j + N/2) % N;
                m_spectrum[ii][jj] = val;
                if (val > maxVal) maxVal = val;
            }
        }
        if (maxVal > 0) for (int i=0;i<N;i++) for (int j=0;j<N;j++) m_spectrum[i][j] /= maxVal;
    } else {
        // Резервное медленное ДПФ
        dft2d_slow(input, m_spectrum);
    }
#endif

    // Сечения
    m_sectionX.resize(N);
    m_sectionY.resize(N);
    int mid = N/2;
    for (int i = 0; i < N; ++i) {
        m_sectionX[i] = m_spectrum[mid][i];
        m_sectionY[i] = m_spectrum[i][mid];
    }

    drawFunction();
    drawSpectrum();
    drawSections();
}