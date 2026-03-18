#ifndef PARAM_H
#define PARAM_H

#include <stdexcept>
#include <cmath>

class Param
{
private:
    // Шаг по функции [мм]
    double m_dStepFunction = 0;
    // Шаг по спектру [1/мм]
    double m_dStepSpectr = 0;
    // Размер выборки
    int m_iSize = 512;

public:
    // Размер стороны квадрата или диаметр круга [мм]
    double m_dFunctionSize = 0;
    // Это круг? (true - круг, false - квадрат)
    bool m_bIsCircle = true;

    // Конструктор (вызывает Reset)
    Param();

    // Деструктор - по умолчанию
    ~Param() = default;

    // Сбрасывает все к параметрам по умолчанию, вычисляет начальные значения шагов
    void Reset();

    // Размер выборки
    void Set_Size(int iSize_p);

    // Шаги связаны между собой и с размером выборки
    // При установке одного из них шаги сразу пересчитываются
    // Δx = 1 / (N * Δν)
    void Set_Step_Function(double dValue_p, int iSize_p);
    void Set_Step_Spectr(double dValue_p, int iSize_p);

    // Получение шага по функции и спектру
    double Get_Step_Function() const;
    double Get_Step_Spectr() const;
    
    // Получение размера выборки
    int Get_Size() const;
};

#endif // PARAM_H
