#pragma once
#include <vector>
#include "lr6param.h"

// Класс для вычислений (формирование функции, спектра и сечений)
class Lr6Calculation
{
public:
    // Данные функции (2D)
    std::vector<std::vector<double>> function;
    // Спектр (2D)
    std::vector<std::vector<double>> spectrum;
    // Сечения спектра
    std::vector<double> sectionX;
    std::vector<double> sectionY;

    // Основной метод: вычислить всё по параметрам
    void Calculate(const Lr6Param& param);

private:
    // Внутренние методы
    void fillFunction(const Lr6Param& param);
    void calcSpectrum(const Lr6Param& param);
    void calcSections();
};
