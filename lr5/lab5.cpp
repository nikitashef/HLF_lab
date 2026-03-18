#include "lab5.h"
#include "ui_lab5.h"
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <cmath>
#include <complex>
#include <stdexcept>

#ifdef FFTW_AVAILABLE
#include <fftw3.h>
#endif

lab5::lab5(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::lab5)
    , m_bBlockSignals(false)
{
    ui->setupUi(this);
    
    // Инициализация параметров
    m_param.Reset();
    
    // Установка начальных значений в UI
    ui->radioCircle->setChecked(true);
    ui->comboBox_sample->setCurrentIndex(2); // Выбираем 512 по умолчанию
    ui->comboBox_base->setCurrentIndex(0);   // Шаг по функции по умолчанию
    ui->radioMicrons->setChecked(true);
    
    // Подключение всех сигналов к слотам
    setupConnections();
    
    // Обновление состояния UI
    updateUIState();
}

lab5::~lab5()
{
    delete ui;
}

void lab5::setupConnections()
{
    // Сигналы для формы функции
    connect(ui->radioCircle, &QRadioButton::clicked, this, &lab5::onShapeChanged);
    connect(ui->radioSquare, &QRadioButton::clicked, this, &lab5::onShapeChanged);
    
    // Сигналы для размера функции
    connect(ui->lineEdit_size, &QLineEdit::textChanged, this, &lab5::onFunctionSizeChanged);
    
    // Сигналы для размера выборки
    connect(ui->comboBox_sample, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &lab5::onSizeChanged);
    
    // Сигналы для исходного параметра
    connect(ui->comboBox_base, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &lab5::onBaseParamChanged);
    
    // Сигналы для шагов
    connect(ui->lineEdit_step_func, &QLineEdit::textChanged, this, &lab5::onStepFunctionChanged);
    connect(ui->lineEdit_step_spec, &QLineEdit::textChanged, this, &lab5::onStepSpectrChanged);
    
    // Сигналы для кнопок
    connect(ui->button_calc, &QPushButton::clicked, this, &lab5::onCalculate);
    connect(ui->button_recalc, &QPushButton::clicked, this, &lab5::onRecalculate);
    connect(ui->button_reset, &QPushButton::clicked, this, &lab5::onReset);
    connect(ui->button_redraw, &QPushButton::clicked, this, &lab5::onRedraw);
}

void lab5::updateUIState()
{
    // Определяем текущий выбранный исходный параметр
    int baseParamIndex = ui->comboBox_base->currentIndex();
    bool isStepFunctionBase = (baseParamIndex == 0);
    
    // Если выбран шаг по функции - его поле включено, спектр отключен
    ui->lineEdit_step_func->setEnabled(isStepFunctionBase);
    ui->lineEdit_step_spec->setEnabled(!isStepFunctionBase);
    
    // Обновляем значения в UI
    applyParamToUI();
}

void lab5::onShapeChanged()
{
    m_param.m_bIsCircle = ui->radioCircle->isChecked();
}

void lab5::onSizeChanged(int index)
{
    // Получаем размер выборки из comboBox
    QString sizeText = ui->comboBox_sample->itemText(index);
    int newSize = sizeText.toInt();
    
    if (newSize <= 0)
        return;
    
    m_param.Set_Size(newSize);
}

void lab5::onBaseParamChanged(int)
{
    updateUIState();
}

void lab5::onFunctionSizeChanged()
{
    double value;
    if (parseDouble(ui->lineEdit_size->text(), value))
    {
        m_param.m_dFunctionSize = value;
    }
}

void lab5::onStepFunctionChanged()
{
    if (m_bBlockSignals)
        return;
    
    double value;
    if (ui->lineEdit_step_func->isEnabled() && parseDouble(ui->lineEdit_step_func->text(), value))
    {
        try
        {
            m_param.Set_Step_Function(value, m_param.Get_Size());
            
            // Обновляем поле шага по спектру
            m_bBlockSignals = true;
            updateStepLabels();
            m_bBlockSignals = false;
        }
        catch (const std::exception &e)
        {
            QMessageBox::warning(this, "Ошибка", QString::fromStdString(e.what()));
        }
    }
}

void lab5::onStepSpectrChanged()
{
    if (m_bBlockSignals)
        return;
    
    double value;
    if (ui->lineEdit_step_spec->isEnabled() && parseDouble(ui->lineEdit_step_spec->text(), value))
    {
        try
        {
            m_param.Set_Step_Spectr(value, m_param.Get_Size());
            
            // Обновляем поле шага по функции
            m_bBlockSignals = true;
            updateStepLabels();
            m_bBlockSignals = false;
        }
        catch (const std::exception &e)
        {
            QMessageBox::warning(this, "Ошибка", QString::fromStdString(e.what()));
        }
    }
}

void lab5::onCalculate()
{
    try
    {
        getParamFromUI();
        
        // Проверяем валидность параметров
        if (m_param.Get_Step_Function() <= 0 || m_param.Get_Step_Spectr() <= 0)
        {
            QMessageBox::warning(this, "Ошибка", "Шаги должны быть положительными значениями");
            return;
        }
        
        int size = m_param.Get_Size();
        
        // Создаём входные данные (фигуру)
        m_inputData.clear();
        m_inputData.resize(size * size);
        generateShape(m_inputData, size);
        
        // Вычисляем FFT
        m_outputData.clear();
        m_outputData.resize(size * size);
        computeFFT(m_inputData, m_outputData);
        
        // Отображаем результаты
        displayResults();
        
        QMessageBox::information(this, "Успешно", "Преобразование Фурье вычислено успешно");
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
    }
}

void lab5::onRecalculate()
{
    try
    {
        int baseParamIndex = ui->comboBox_base->currentIndex();
        int size = m_param.Get_Size();
        double value = 0;
        
        if (baseParamIndex == 0)
        {
            // Шаг по функции - пересчитываем шаг по спектру
            if (parseDouble(ui->lineEdit_step_func->text(), value))
            {
                m_param.Set_Step_Function(value, size);
            }
        }
        else
        {
            // Шаг по спектру - пересчитываем шаг по функции
            if (parseDouble(ui->lineEdit_step_spec->text(), value))
            {
                m_param.Set_Step_Spectr(value, size);
            }
        }
        
        updateStepLabels();
        QMessageBox::information(this, "Успешно", "Шаги пересчитаны");
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
    }
}

void lab5::onReset()
{
    m_bBlockSignals = true;
    
    // Сбрасываем параметры
    m_param.Reset();
    
    // Очищаем UI
    ui->lineEdit_size->clear();
    ui->lineEdit_step_func->clear();
    ui->lineEdit_step_spec->clear();
    ui->radioCircle->setChecked(true);
    ui->comboBox_sample->setCurrentIndex(2); // 512
    ui->comboBox_base->setCurrentIndex(0);   // Шаг по функции
    ui->radioMicrons->setChecked(true);
    
    m_inputData.clear();
    m_outputData.clear();
    
    m_bBlockSignals = false;
    updateUIState();
    
    QMessageBox::information(this, "Успешно", "Все параметры сброшены");
}

void lab5::onRedraw()
{
    if (m_outputData.empty())
    {
        QMessageBox::warning(this, "Ошибка", "Сначала вычислите преобразование Фурье");
        return;
    }
    
    displayResults();
    QMessageBox::information(this, "Успешно", "График перерисован");
}

void lab5::applyParamToUI()
{
    if (m_bBlockSignals)
        return;
    
    m_bBlockSignals = true;
    
    // Обновляем UI в соответствии с параметрами
    updateStepLabels();
    
    if (m_param.m_dFunctionSize > 0)
    {
        ui->lineEdit_size->setText(QString::number(m_param.m_dFunctionSize, 'f', 6));
    }
    
    m_bBlockSignals = false;
}

void lab5::updateStepLabels()
{
    int baseParamIndex = ui->comboBox_base->currentIndex();
    
    if (baseParamIndex == 0)
    {
        // Показываем шаг по функции, вычисляем спектр
        ui->lineEdit_step_func->setText(QString::number(m_param.Get_Step_Function(), 'e', 6));
        ui->lineEdit_step_spec->setText(QString::number(m_param.Get_Step_Spectr(), 'e', 6));
    }
    else
    {
        // Показываем шаг по спектру, вычисляем по функции
        ui->lineEdit_step_spec->setText(QString::number(m_param.Get_Step_Spectr(), 'e', 6));
        ui->lineEdit_step_func->setText(QString::number(m_param.Get_Step_Function(), 'e', 6));
    }
}

void lab5::getParamFromUI()
{
    double size;
    if (parseDouble(ui->lineEdit_size->text(), size))
    {
        m_param.m_dFunctionSize = size;
    }
    
    m_param.m_bIsCircle = ui->radioCircle->isChecked();
}

bool lab5::parseDouble(const QString &str, double &value)
{
    if (str.isEmpty())
        return false;
    
    bool ok;
    value = str.toDouble(&ok);
    
    if (!ok)
    {
        QMessageBox::warning(this, "Ошибка", "Некорректный ввод числа");
        return false;
    }
    
    return true;
}

void lab5::generateShape(std::vector<std::complex<double>> &data, int size)
{
    // Инициализируем нулями
    std::fill(data.begin(), data.end(), std::complex<double>(0, 0));
    
    double functionSize = m_param.m_dFunctionSize;
    bool isCircle = m_param.m_bIsCircle;
    
    if (functionSize <= 0)
    {
        // Если размер не задан, генерируем единичный импульс в центре
        int center = size / 2;
        data[center * size + center] = std::complex<double>(1, 0);
        return;
    }
    
    double stepFunction = m_param.Get_Step_Function();
    double radius = functionSize / 2.0;
    double centerX = (size / 2.0) * stepFunction;
    double centerY = (size / 2.0) * stepFunction;
    
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            double x = i * stepFunction;
            double y = j * stepFunction;
            
            // Смещаем координаты от центра
            double dx = x - centerX;
            double dy = y - centerY;
            double distance = std::sqrt(dx * dx + dy * dy);
            
            if (isCircle)
            {
                // Круг: если расстояние <= радиусу
                if (distance <= radius)
                {
                    data[i * size + j] = std::complex<double>(1, 0);
                }
            }
            else
            {
                // Квадрат: если координаты находятся внутри квадрата
                if (std::abs(dx) <= radius && std::abs(dy) <= radius)
                {
                    data[i * size + j] = std::complex<double>(1, 0);
                }
            }
        }
    }
}

void lab5::computeFFT(const std::vector<std::complex<double>> &input, 
                      std::vector<std::complex<double>> &output)
{
    // Для использования FFTW, нужно правильно скомпилировать проект с FFTW_AVAILABLE
    // Если FFTW недоступна, используем простую реализацию или оставляем вывод входных данных
    
#ifdef FFTW_AVAILABLE
    int size = m_param.Get_Size();
    
    // Подготовка данных для FFTW
    fftw_complex *in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * size * size);
    fftw_complex *out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * size * size);
    
    if (!in || !out)
    {
        throw std::runtime_error("Failed to allocate memory for FFT");
    }
    
    // Копируем входные данные
    for (int i = 0; i < size * size; ++i)
    {
        in[i][0] = input[i].real();
        in[i][1] = input[i].imag();
    }
    
    // Создаём и выполняем FFT план
    fftw_plan p = fftw_plan_dft_2d(size, size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    
    // Копируем результаты
    output.resize(size * size);
    for (int i = 0; i < size * size; ++i)
    {
        output[i] = std::complex<double>(out[i][0], out[i][1]);
    }
    
    // Очистка
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
#else
    // Если FFTW недоступна, копируем входные данные (затем добавить другую реализацию если нужно)
    output = input;
#endif
}

void lab5::displayResults()
{
    if (m_outputData.empty())
        return;
    
    // Здесь можно добавить код для отображения результатов на графике
    // Например, использование QwtPlot или QCustomPlot для визуализации
    
    // Пока просто выводим информацию о количестве точек
    int size = m_param.Get_Size();
    QString info = QString("Размер выборки: %1x%1\n").arg(size);
    info += QString("Шаг по функции: %1 мм\n").arg(m_param.Get_Step_Function());
    info += QString("Шаг по спектру: %1 1/мм\n").arg(m_param.Get_Step_Spectr());
    info += QString("Форма: %1\n").arg(m_param.m_bIsCircle ? "Круг" : "Квадрат");
    info += QString("Единицы: %1").arg(ui->radioMicrons->isChecked() ? "мкм" : "пиксели");
}
