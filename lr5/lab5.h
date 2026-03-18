#ifndef LAB5_H
#define LAB5_H

#include <QDialog>
#include <vector>
#include <complex>
#include "param.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class lab5;
}
QT_END_NAMESPACE

class lab5 : public QDialog
{
    Q_OBJECT

public:
    explicit lab5(QWidget *parent = nullptr);
    ~lab5() override;

private slots:
    // Обработчики сигналов для формы функции
    void onShapeChanged();
    
    // Обработчики для размера выборки
    void onSizeChanged(int index);
    
    // Обработчики для исходного параметра
    void onBaseParamChanged(int index);
    
    // Обработчики для ввода размера функции
    void onFunctionSizeChanged();
    
    // Обработчики для шагов
    void onStepFunctionChanged();
    void onStepSpectrChanged();
    
    // Основные кнопки
    void onCalculate();
    void onRecalculate();
    void onReset();
    void onRedraw();

private:
    // Вспомогательные методы
    void setupConnections();
    void updateUIState();
    void updateStepLabels();
    
    // Безопасное преобразование строки в double
    bool parseDouble(const QString &str, double &value);
    
    // Методы для работы с параметрами
    void applyParamToUI();
    void getParamFromUI();
    
    // Создание геометрической фигуры
    void generateShape(std::vector<std::complex<double>> &data, int size);
    
    // FFT вычисления
    void computeFFT(const std::vector<std::complex<double>> &input, 
                   std::vector<std::complex<double>> &output);
    
    // Отрисовка результатов
    void displayResults();

    Ui::lab5 *ui;
    Param m_param;
    
    // Флаги для отслеживания состояния
    bool m_bBlockSignals = false;
    
    // Хранение результатов вычислений
    std::vector<std::complex<double>> m_inputData;
    std::vector<std::complex<double>> m_outputData;
};
#endif // LAB5_H
