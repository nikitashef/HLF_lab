#pragma once
#include <cmath>

// Класс для хранения параметров задачи
class Lr6Param
{
public:
    // Размер стороны квадрата или диаметр круга
    double size = 0;
    // Это круг (true) или квадрат (false)
    bool isCircle = true;
    // Размер выборки
    int N = 128;
    // Шаг по функции
    double dx = 1.0;
    // Шаг по спектру
    double dnu = 1.0;

    // Сброс параметров к значениям по умолчанию
    void Reset()
    {
        size = 10.0;
        isCircle = true;
        N = 128;
        dx = 1.0;
        dnu = 1.0 / (N * dx);
    }

    // Установить шаг по функции и пересчитать шаг по спектру
    void SetDx(double val, int n)
    {
        N = n;
        dx = val;
        dnu = 1.0 / (N * dx);
    }

    // Установить шаг по спектру и пересчитать шаг по функции
    void SetDnu(double val, int n)
    {
        N = n;
        dnu = val;
        dx = 1.0 / (N * dnu);
    }
};
