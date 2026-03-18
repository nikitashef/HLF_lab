#include "param.h"

// Конструктор (вызывает Reset)
Param::Param()
{
    Reset();
}

// Сбрасывает все к параметрам по умолчанию, вычисляет начальные значения шагов
void Param::Reset()
{
    m_dFunctionSize = 0;
    m_bIsCircle = true;
    m_iSize = 512;
    
    // Начальные значения шагов (при нулевом размере функции)
    // Δx = 1 / (N * Δν)
    // Если выбираем Δx как исходный параметр:
    m_dStepFunction = 0.001; // 1 мкм = 0.001 мм
    m_dStepSpectr = 1.0 / (m_iSize * m_dStepFunction); // 1/мм
}

// Установка размера выборки
void Param::Set_Size(int iSize_p)
{
    if (iSize_p <= 0)
    {
        throw std::invalid_argument("Size must be positive");
    }
    m_iSize = iSize_p;
}

// Установка шага по функции
void Param::Set_Step_Function(double dValue_p, int iSize_p)
{
    if (dValue_p <= 0)
    {
        throw std::invalid_argument("Step function must be positive");
    }
    if (iSize_p <= 0)
    {
        throw std::invalid_argument("Size must be positive");
    }
    
    m_dStepFunction = dValue_p;
    m_iSize = iSize_p;
    
    // Пересчет шага по спектру по формуле: Δx = 1 / (N * Δν)
    // => Δν = 1 / (N * Δx)
    m_dStepSpectr = 1.0 / (m_iSize * m_dStepFunction);
}

// Установка шага по спектру
void Param::Set_Step_Spectr(double dValue_p, int iSize_p)
{
    if (dValue_p <= 0)
    {
        throw std::invalid_argument("Step spectrum must be positive");
    }
    if (iSize_p <= 0)
    {
        throw std::invalid_argument("Size must be positive");
    }
    
    m_dStepSpectr = dValue_p;
    m_iSize = iSize_p;
    
    // Пересчет шага по функции по формуле: Δx = 1 / (N * Δν)
    m_dStepFunction = 1.0 / (m_iSize * m_dStepSpectr);
}

// Получение шага по функции
double Param::Get_Step_Function() const
{
    return m_dStepFunction;
}

// Получение шага по спектру
double Param::Get_Step_Spectr() const
{
    return m_dStepSpectr;
}

// Получение размера выборки
int Param::Get_Size() const
{
    return m_iSize;
}
