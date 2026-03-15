#pragma once
#include <iostream>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <chrono>

// Условное подключение FFTW
#ifdef FFTW_AVAILABLE
#include <fftw3.h>
#endif

// Условное подключение Boost
#ifdef BOOST_AVAILABLE
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#endif

// Класс-шаблон для двумерной выборки
template <class PAR>
class Sample
{
protected:
    int m_size_x = 0;
    int m_size_y = 0;
    std::vector<PAR> m_data;

public:
    Sample() = default;
    ~Sample() = default;

    // Изменение размера для квадратной выборки
    void Resize(int size);
    
    // Изменение размера для прямоугольной выборки
    void Resize(int size_x, int size_y);

    // Размер выборки (для квадратной)
    int GetSize() const;
    
    // Размер выборки по X и Y
    int GetSizeX() const;
    int GetSizeY() const;

    // Оператор получения значения элемента с номером i,j
    PAR& operator()(int i, int j);
    
    // Оператор получения const значения элемента с номером i,j
    const PAR& operator()(int i, int j) const;

    // Проверка квадратная ли выборка
    bool IsSquare() const;
    
    // Проверка что размер != 0
    bool IsZeroSize() const;
    
    // Проверка равенства размеров с другой выборкой
    bool IsEqualSize(const Sample<PAR>& temp) const;

    // Инициализация всех элементов выборки одинаковым значением
    void SetValue(PAR value);
    
    // Получение указателя на всю выборку
    PAR* GetPointer();
    
    // Вычисление максимального значения
    PAR GetMax() const;

    // Операции с выборкой и числом
    // Сложение всех элементов выборки и числа
    Sample<PAR>& operator+=(PAR value);
    
    // Умножение всех элементов выборки на число
    Sample<PAR>& operator*=(PAR value);
    
    // Деление всех элементов выборки на число
    Sample<PAR>& operator/=(PAR value);

    // Операции с двумя выборками
    // Присваивание
    Sample<PAR>& operator=(const Sample<PAR>& temp);
    
    // Поэлементное сложение двух выборок
    Sample<PAR>& operator+=(const Sample<PAR>& temp);
    
    // Поэлементное вычитание двух выборок
    Sample<PAR>& operator-=(const Sample<PAR>& temp);
    
    // Поэлементное перемножение двух выборок (виртуальная для переопределения)
    virtual Sample<PAR>& operator*=(const Sample<PAR>& temp);
    
    // Поэлементное деление двух выборок (виртуальная для переопределения)
    virtual Sample<PAR>& operator/=(const Sample<PAR>& temp);

    // Вывод выборки в поток (файл или экран)
    std::ostream& Save(std::ostream& out);
    
    // Чтение выборки из потока (файл или экран)
    std::istream& Load(std::istream& in);

protected:
    // Проверяет квадратная ли выборка, если нет - создает исключение
    void CheckSquare() const;
};


// Реализация методов класса Sample

template <class PAR>
void Sample<PAR>::Resize(int size)
{
    if (size < 0)
    {
        throw std::invalid_argument("Size cannot be negative");
    }
    m_size_x = size;
    m_size_y = size;
    m_data.resize(size * size);
    std::fill(m_data.begin(), m_data.end(), PAR());
}

template <class PAR>
void Sample<PAR>::Resize(int size_x, int size_y)
{
    if (size_x < 0 || size_y < 0)
    {
        throw std::invalid_argument("Sizes cannot be negative");
    }
    m_size_x = size_x;
    m_size_y = size_y;
    m_data.resize(size_x * size_y);
    std::fill(m_data.begin(), m_data.end(), PAR());
}

template <class PAR>
int Sample<PAR>::GetSize() const
{
    CheckSquare();
    return m_size_x;
}

template <class PAR>
int Sample<PAR>::GetSizeX() const
{
    return m_size_x;
}

template <class PAR>
int Sample<PAR>::GetSizeY() const
{
    return m_size_y;
}

template <class PAR>
PAR& Sample<PAR>::operator()(int i, int j)
{
    if (i < 0 || i >= m_size_x || j < 0 || j >= m_size_y)
    {
        throw std::out_of_range("Index out of range");
    }
    return m_data[i * m_size_y + j];
}

template <class PAR>
const PAR& Sample<PAR>::operator()(int i, int j) const
{
    if (i < 0 || i >= m_size_x || j < 0 || j >= m_size_y)
    {
        throw std::out_of_range("Index out of range");
    }
    return m_data[i * m_size_y + j];
}

template <class PAR>
bool Sample<PAR>::IsSquare() const
{
    return (m_size_x == m_size_y) && (m_size_x != 0);
}

template <class PAR>
bool Sample<PAR>::IsZeroSize() const
{
    return (m_size_x == 0) || (m_size_y == 0);
}

template <class PAR>
bool Sample<PAR>::IsEqualSize(const Sample<PAR>& temp) const
{
    return (m_size_x == temp.m_size_x) && (m_size_y == temp.m_size_y);
}

template <class PAR>
void Sample<PAR>::SetValue(PAR value)
{
    std::fill(m_data.begin(), m_data.end(), value);
}

template <class PAR>
PAR* Sample<PAR>::GetPointer()
{
    return m_data.data();
}

template <class PAR>
PAR Sample<PAR>::GetMax() const
{
    if (m_data.empty())
    {
        throw std::runtime_error("Cannot get max of empty sample");
    }
    return *std::max_element(m_data.begin(), m_data.end());
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator+=(PAR value)
{
    for (auto& elem : m_data)
    {
        elem += value;
    }
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator*=(PAR value)
{
    for (auto& elem : m_data)
    {
        elem *= value;
    }
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator/=(PAR value)
{
    if (value == PAR())
    {
        throw std::invalid_argument("Division by zero");
    }
    for (auto& elem : m_data)
    {
        elem /= value;
    }
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator=(const Sample<PAR>& temp)
{
    if (this == &temp)
    {
        return *this;
    }
    m_size_x = temp.m_size_x;
    m_size_y = temp.m_size_y;
    m_data = temp.m_data;
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator+=(const Sample<PAR>& temp)
{
    if (!IsEqualSize(temp))
    {
        throw std::invalid_argument("Samples must have equal sizes");
    }
    for (size_t i = 0; i < m_data.size(); ++i)
    {
        m_data[i] += temp.m_data[i];
    }
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator-=(const Sample<PAR>& temp)
{
    if (!IsEqualSize(temp))
    {
        throw std::invalid_argument("Samples must have equal sizes");
    }
    for (size_t i = 0; i < m_data.size(); ++i)
    {
        m_data[i] -= temp.m_data[i];
    }
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator*=(const Sample<PAR>& temp)
{
    if (!IsEqualSize(temp))
    {
        throw std::invalid_argument("Samples must have equal sizes");
    }
    for (size_t i = 0; i < m_data.size(); ++i)
    {
        m_data[i] *= temp.m_data[i];
    }
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator/=(const Sample<PAR>& temp)
{
    if (!IsEqualSize(temp))
    {
        throw std::invalid_argument("Samples must have equal sizes");
    }
    for (size_t i = 0; i < m_data.size(); ++i)
    {
        m_data[i] /= temp.m_data[i];
    }
    return *this;
}

template <class PAR>
std::ostream& Sample<PAR>::Save(std::ostream& out)
{
    out << m_size_x << " " << m_size_y << "\n";
    for (int i = 0; i < m_size_x; ++i)
    {
        for (int j = 0; j < m_size_y; ++j)
        {
            if (j > 0)
            {
                out << " ";
            }
            out << std::fixed << std::setprecision(2) << operator()(i, j);
        }
        out << "\n";
    }
    return out;
}

template <class PAR>
std::istream& Sample<PAR>::Load(std::istream& in)
{
    int size_x, size_y;
    in >> size_x >> size_y;
    Resize(size_x, size_y);
    for (int i = 0; i < size_x; ++i)
    {
        for (int j = 0; j < size_y; ++j)
        {
            in >> operator()(i, j);
        }
    }
    return in;
}

template <class PAR>
void Sample<PAR>::CheckSquare() const
{
    if (!IsSquare())
    {
        throw std::logic_error("Sample must be square for this operation");
    }
}


// Класс Matrix - специализация Sample<double> для работы с матрицами вещественных чисел
class Matrix : public Sample<double>
{
public:
    Matrix() = default;
    ~Matrix() = default;

    // Создание единичной матрицы (квадратная выборка)
    void MakeIdentity();

    // Матричное произведение
    Matrix operator*(const Matrix& other) const;

    // Переопределение operator*= для генерации исключения
    virtual Sample<double>& operator*=(const Sample<double>& temp) override;

    // Переопределение operator/= для генерации исключения
    virtual Sample<double>& operator/=(const Sample<double>& temp) override;

    // Транспонирование
    Matrix operator~() const;

    // Вычисление определителя для квадратной матрицы
    double CalculateDeterminant() const;

private:
    // Проверка возможности матричного произведения
    void CheckProductCompatibility(const Matrix& other) const;
};


// Класс SampleComplex - специализация Sample<std::complex<double>> для работы с комплексными матрицами
class SampleComplex : public Sample<std::complex<double>>
{
public:
    SampleComplex() = default;
    ~SampleComplex() = default;

    // Полный конструктор для квадратной выборки
    SampleComplex(int size);

    // Вычисление модуля (абсолютного значения) каждого элемента
    Sample<double> GetModulus() const;

    // Вычисление фазы каждого элемента
    Sample<double> GetPhase() const;

    // Вычисление интенсивности (квадрат модуля) каждого элемента
    Sample<double> GetIntensity() const;

    // Преобразование из SampleComplex в Sample<double> (только вещественная часть)
    Sample<double> ToRealSample() const;

    // Преобразование из Sample<double> в SampleComplex
    void FromRealSample(const Sample<double>& real_sample);

    // Функции вывода в файл
    void SaveReal(const std::string& filename) const;
    void SaveImag(const std::string& filename) const;
    void SaveModulus(const std::string& filename) const;
    void SavePhase(const std::string& filename) const;
    void SaveIntensity(const std::string& filename) const;

    // Вычисление определителя для квадратной матрицы комплексных чисел
    std::complex<double> CalculateDeterminant() const;
};
