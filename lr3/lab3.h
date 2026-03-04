#pragma once
#include <iostream>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <algorithm>

// Класс-шаблон для двумерной выборки
template <class PAR>
class Sample
{
protected:
    // число элементов выборки по x и y
    int m_size_x = 0, m_size_y = 0;
    // массив данных
    std::vector<PAR> m_data;

public:
    // конструктор
    Sample() = default;
    // деструктор
    ~Sample() = default;

    // изменение размера для квадратной выборки
    void Resize(int size);
    // изменение размера для прямоугольной выборки
    void Resize(int size_x, int size_y);

    // размер выборки (для квадратной)
    int GetSize() const;
    // размер выборки по X и Y
    int GetSizeX() const;
    int GetSizeY() const;

    // оператор получения значения элемента с номером i,j
    PAR& operator()(int i, int j);
    // оператор получения const значения элемента с номером i,j
    const PAR& operator()(int i, int j) const;

    // проверка квадратная ли выборка
    bool IsSquare() const;
    // проверка что размер != 0
    bool IsZeroSize() const;
    // проверка равенства размеров с другой выборкой
    bool IsEqualSize(const Sample<PAR>& temp) const;

    // инициализация всех элементов выборки одинаковым значением
    void SetValue(PAR value);
    // получение указателя на всю выборку
    PAR* GetPointer();
    // вычисление максимального значения
    PAR GetMax() const;

    // операции с выборкой и числом
    // сложение всех элементов выборки и числа
    Sample<PAR>& operator+=(PAR value);
    // умножение всех элементов выборки на число
    Sample<PAR>& operator*=(PAR value);
    // деление всех элементов выборки на число
    Sample<PAR>& operator/=(PAR value);

    // операции с двумя выборками
    // присваивание
    Sample<PAR>& operator=(const Sample<PAR>& temp);
    // поэлементное сложение двух выборок
    Sample<PAR>& operator+=(const Sample<PAR>& temp);
    // поэлементное вычитание двух выборок
    Sample<PAR>& operator-=(const Sample<PAR>& temp);
    // поэлементное перемножение двух выборок
    virtual Sample<PAR>& operator*=(const Sample<PAR>& temp);

    // вывод выборки в поток (файл или экран)
    std::ostream& Save(std::ostream& out);
    // перегрузка оператора чтения выборки из файла
    std::istream& Load(std::istream& in);

protected:
    // проверяет квадратная ли выборка, если нет - создает исключение
    void CheckSquare() const;
};

// ===== РЕАЛИЗАЦИЯ МЕТОДОВ =====

template <class PAR>
void Sample<PAR>::Resize(int size)
{
    if (size < 0)
        throw std::invalid_argument("Size cannot be negative");
    m_size_x = size;
    m_size_y = size;
    m_data.resize(size * size);
    std::fill(m_data.begin(), m_data.end(), PAR());
}

template <class PAR>
void Sample<PAR>::Resize(int size_x, int size_y)
{
    if (size_x < 0 || size_y < 0)
        throw std::invalid_argument("Sizes cannot be negative");
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
        throw std::out_of_range("Index out of range");
    return m_data[i * m_size_y + j];
}

template <class PAR>
const PAR& Sample<PAR>::operator()(int i, int j) const
{
    if (i < 0 || i >= m_size_x || j < 0 || j >= m_size_y)
        throw std::out_of_range("Index out of range");
    return m_data[i * m_size_y + j];
}

template <class PAR>
bool Sample<PAR>::IsSquare() const
{
    return m_size_x == m_size_y && m_size_x != 0;
}

template <class PAR>
bool Sample<PAR>::IsZeroSize() const
{
    return m_size_x == 0 || m_size_y == 0;
}

template <class PAR>
bool Sample<PAR>::IsEqualSize(const Sample<PAR>& temp) const
{
    return m_size_x == temp.m_size_x && m_size_y == temp.m_size_y;
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
        throw std::runtime_error("Cannot get max of empty sample");
    return *std::max_element(m_data.begin(), m_data.end());
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator+=(PAR value)
{
    for (auto& elem : m_data)
        elem += value;
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator*=(PAR value)
{
    for (auto& elem : m_data)
        elem *= value;
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator/=(PAR value)
{
    if (value == PAR())
        throw std::invalid_argument("Division by zero");
    for (auto& elem : m_data)
        elem /= value;
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator=(const Sample<PAR>& temp)
{
    if (this == &temp)
        return *this;
    m_size_x = temp.m_size_x;
    m_size_y = temp.m_size_y;
    m_data = temp.m_data;
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator+=(const Sample<PAR>& temp)
{
    if (!IsEqualSize(temp))
        throw std::invalid_argument("Samples must have equal sizes");
    for (size_t i = 0; i < m_data.size(); ++i)
        m_data[i] += temp.m_data[i];
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator-=(const Sample<PAR>& temp)
{
    if (!IsEqualSize(temp))
        throw std::invalid_argument("Samples must have equal sizes");
    for (size_t i = 0; i < m_data.size(); ++i)
        m_data[i] -= temp.m_data[i];
    return *this;
}

template <class PAR>
Sample<PAR>& Sample<PAR>::operator*=(const Sample<PAR>& temp)
{
    if (!IsEqualSize(temp))
        throw std::invalid_argument("Samples must have equal sizes");
    for (size_t i = 0; i < m_data.size(); ++i)
        m_data[i] *= temp.m_data[i];
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
            if (j > 0) out << " ";
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
        throw std::logic_error("Sample must be square for this operation");
}

// Вспомогательная функция вывода для тестирования
template <class T>
inline void PrintSample(const Sample<T>& s, const std::string& name)
{
    std::cout << name << " (" << s.GetSizeX() << "x" << s.GetSizeY() << "):\n";
    for (int i = 0; i < s.GetSizeX(); ++i)
    {
        for (int j = 0; j < s.GetSizeY(); ++j)
        {
            std::cout << std::fixed << std::setprecision(2) << s(i, j) << " ";
        }
        std::cout << "\n";
    }
}
