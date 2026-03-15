#include "lab4.h"
#include <sstream>
#include <string>
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

using std::cout;
using std::endl;
using std::cin;

// ============ Реализация класса Matrix ============

void Matrix::MakeIdentity()
{
    CheckSquare();
    SetValue(0.0);
    int size = GetSize();
    for (int i = 0; i < size; ++i)
    {
        operator()(i, i) = 1.0;
    }
}

void Matrix::CheckProductCompatibility(const Matrix& other) const
{
    if (GetSizeY() != other.GetSizeX())
    {
        throw std::invalid_argument("Matrix dimensions are incompatible for multiplication");
    }
}

Matrix Matrix::operator*(const Matrix& other) const
{
    CheckProductCompatibility(other);
    
    Matrix result;
    result.Resize(GetSizeX(), other.GetSizeY());
    result.SetValue(0.0);
    
    for (int i = 0; i < GetSizeX(); ++i)
    {
        for (int j = 0; j < other.GetSizeY(); ++j)
        {
            double sum = 0.0;
            for (int k = 0; k < GetSizeY(); ++k)
            {
                sum += operator()(i, k) * other(k, j);
            }
            result(i, j) = sum;
        }
    }
    
    return result;
}

Sample<double>& Matrix::operator*=(const Sample<double>& temp)
{
    throw std::runtime_error("Use of *= operator for element-wise multiplication is not allowed for matrices. Use * operator for matrix multiplication instead");
}

Sample<double>& Matrix::operator/=(const Sample<double>& temp)
{
    throw std::runtime_error("Use of /= operator for element-wise division is not allowed for matrices");
}

Matrix Matrix::operator~() const
{
    Matrix result;
    result.Resize(GetSizeY(), GetSizeX());

    for (int i = 0; i < GetSizeX(); ++i)
    {
        for (int j = 0; j < GetSizeY(); ++j)
        {
            result(j, i) = operator()(i, j);
        }
    }

    return result;
}

// Вспомогательная функция для вычисления определителя 2x2
static double GetDeterminant2x2(
    double a11, double a12,
    double a21, double a22)
{
    return (a11 * a22) - (a12 * a21);
}

// Вспомогательная функция для вычисления определителя 3x3
static double GetDeterminant3x3(const Matrix& m)
{
    double a11 = m(0, 0);
    double a12 = m(0, 1);
    double a13 = m(0, 2);
    double a21 = m(1, 0);
    double a22 = m(1, 1);
    double a23 = m(1, 2);
    double a31 = m(2, 0);
    double a32 = m(2, 1);
    double a33 = m(2, 2);

    double det1 = GetDeterminant2x2(a22, a23, a32, a33);
    double det2 = GetDeterminant2x2(a21, a23, a31, a33);
    double det3 = GetDeterminant2x2(a21, a22, a31, a32);

    return (a11 * det1) - (a12 * det2) + (a13 * det3);
}

double Matrix::CalculateDeterminant() const
{
    CheckSquare();
    
    int n = GetSize();

    if (n == 1)
    {
        return operator()(0, 0);
    }

    if (n == 2)
    {
        return GetDeterminant2x2(
            operator()(0, 0), operator()(0, 1),
            operator()(1, 0), operator()(1, 1)
        );
    }

    if (n == 3)
    {
        return GetDeterminant3x3(*this);
    }

    throw std::runtime_error("Determinant calculation for n > 3 not implemented");
}


// ============ Реализация класса SampleComplex ============

SampleComplex::SampleComplex(int size)
{
    Resize(size);
}

Sample<double> SampleComplex::GetModulus() const
{
    Sample<double> result;
    result.Resize(GetSizeX(), GetSizeY());
    
    for (int i = 0; i < GetSizeX(); ++i)
    {
        for (int j = 0; j < GetSizeY(); ++j)
        {
            result(i, j) = std::abs(operator()(i, j));
        }
    }
    
    return result;
}

Sample<double> SampleComplex::GetPhase() const
{
    Sample<double> result;
    result.Resize(GetSizeX(), GetSizeY());
    
    for (int i = 0; i < GetSizeX(); ++i)
    {
        for (int j = 0; j < GetSizeY(); ++j)
        {
            result(i, j) = std::arg(operator()(i, j));
        }
    }
    
    return result;
}

Sample<double> SampleComplex::GetIntensity() const
{
    Sample<double> result;
    result.Resize(GetSizeX(), GetSizeY());
    
    for (int i = 0; i < GetSizeX(); ++i)
    {
        for (int j = 0; j < GetSizeY(); ++j)
        {
            double mod = std::abs(operator()(i, j));
            result(i, j) = mod * mod;
        }
    }
    
    return result;
}

Sample<double> SampleComplex::ToRealSample() const
{
    Sample<double> result;
    result.Resize(GetSizeX(), GetSizeY());
    
    for (int i = 0; i < GetSizeX(); ++i)
    {
        for (int j = 0; j < GetSizeY(); ++j)
        {
            result(i, j) = std::real(operator()(i, j));
        }
    }
    
    return result;
}

void SampleComplex::FromRealSample(const Sample<double>& real_sample)
{
    if (GetSizeX() != real_sample.GetSizeX() || GetSizeY() != real_sample.GetSizeY())
    {
        Resize(real_sample.GetSizeX(), real_sample.GetSizeY());
    }

    for (int i = 0; i < GetSizeX(); ++i)
    {
        for (int j = 0; j < GetSizeY(); ++j)
        {
            operator()(i, j) = std::complex<double>(real_sample(i, j), 0.0);
        }
    }
}

void SampleComplex::SaveReal(const std::string& filename) const
{
    std::ofstream file(filename);
    Sample<double> real_part = ToRealSample();
    real_part.Save(file);
    file.close();
}

void SampleComplex::SaveImag(const std::string& filename) const
{
    std::ofstream file(filename);
    file << GetSizeX() << " " << GetSizeY() << "\n";
    for (int i = 0; i < GetSizeX(); ++i)
    {
        for (int j = 0; j < GetSizeY(); ++j)
        {
            if (j > 0) file << " ";
            file << std::fixed << std::setprecision(2) 
                 << std::imag(operator()(i, j));
        }
        file << "\n";
    }
    file.close();
}

void SampleComplex::SaveModulus(const std::string& filename) const
{
    std::ofstream file(filename);
    Sample<double> modulus = GetModulus();
    modulus.Save(file);
    file.close();
}

void SampleComplex::SavePhase(const std::string& filename) const
{
    std::ofstream file(filename);
    Sample<double> phase = GetPhase();
    phase.Save(file);
    file.close();
}

void SampleComplex::SaveIntensity(const std::string& filename) const
{
    std::ofstream file(filename);
    Sample<double> intensity = GetIntensity();
    intensity.Save(file);
    file.close();
}

// Вспомогательная функция для вычисления определителя 2x2 комплексных чисел
static std::complex<double> GetComplexDeterminant2x2(
    const std::complex<double>& a11, const std::complex<double>& a12,
    const std::complex<double>& a21, const std::complex<double>& a22)
{
    return (a11 * a22) - (a12 * a21);
}

// Вспомогательная функция для вычисления определителя 3x3 комплексных чисел
static std::complex<double> GetComplexDeterminant3x3(const SampleComplex& m)
{
    std::complex<double> a11 = m(0, 0);
    std::complex<double> a12 = m(0, 1);
    std::complex<double> a13 = m(0, 2);
    std::complex<double> a21 = m(1, 0);
    std::complex<double> a22 = m(1, 1);
    std::complex<double> a23 = m(1, 2);
    std::complex<double> a31 = m(2, 0);
    std::complex<double> a32 = m(2, 1);
    std::complex<double> a33 = m(2, 2);

    std::complex<double> det1 = GetComplexDeterminant2x2(a22, a23, a32, a33);
    std::complex<double> det2 = GetComplexDeterminant2x2(a21, a23, a31, a33);
    std::complex<double> det3 = GetComplexDeterminant2x2(a21, a22, a31, a32);

    return (a11 * det1) - (a12 * det2) + (a13 * det3);
}

std::complex<double> SampleComplex::CalculateDeterminant() const
{
    CheckSquare();
    
    int n = GetSize();

    if (n == 1)
    {
        return operator()(0, 0);
    }

    if (n == 2)
    {
        return GetComplexDeterminant2x2(
            operator()(0, 0), operator()(0, 1),
            operator()(1, 0), operator()(1, 1)
        );
    }

    if (n == 3)
    {
        return GetComplexDeterminant3x3(*this);
    }

    throw std::runtime_error("Determinant calculation for n > 3 not implemented");
}


// Вспомогательная функция для вывода разделителя
void PrintSeparator()
{
    cout << "\n" << std::string(70, '=') << "\n";
}

// Вспомогательная функция для вывода матрицы вещественных чисел
void PrintMatrix(const Matrix& m, const std::string& name)
{
    cout << name << " (" << m.GetSizeX() << "x" << m.GetSizeY() << "):\n";
    for (int i = 0; i < m.GetSizeX(); ++i)
    {
        for (int j = 0; j < m.GetSizeY(); ++j)
        {
            cout << std::fixed << std::setprecision(2) 
                 << std::setw(10) << m(i, j);
        }
        cout << "\n";
    }
}

// Вспомогательная функция для вывода матрицы комплексных чисел
void PrintComplexMatrix(const SampleComplex& m, const std::string& name)
{
    cout << name << " (" << m.GetSizeX() << "x" << m.GetSizeY() << "):\n";
    for (int i = 0; i < m.GetSizeX(); ++i)
    {
        for (int j = 0; j < m.GetSizeY(); ++j)
        {
            const std::complex<double>& c = m(i, j);
            cout << "(" << std::fixed << std::setprecision(2) 
                 << std::setw(6) << std::real(c) << ","
                 << std::setw(6) << std::imag(c) << "i) ";
        }
        cout << "\n";
    }
}

int main()
{
    cout << "========== Matrix and SampleComplex Classes Testing ==========\n";

    // Тест 1: Матрица вещественных чисел и её операции
    PrintSeparator();
    cout << "Test 1: Matrix Operations\n";
    Matrix m1;
    m1.Resize(2, 2);
    m1(0, 0) = 1.0;
    m1(0, 1) = 2.0;
    m1(1, 0) = 3.0;
    m1(1, 1) = 4.0;
    PrintMatrix(m1, "Matrix m1");
    cout << "Determinant: " << std::fixed << std::setprecision(2) 
         << m1.CalculateDeterminant() << "\n";

    // Тест 2: Операции с матрицей и скаляром
    PrintSeparator();
    cout << "Test 2: Matrix and Scalar Operations\n";
    Matrix m2;
    m2.Resize(2, 2);
    m2.SetValue(2.0);
    cout << "Before *= 3.0:\n";
    PrintMatrix(m2, "m2");
    static_cast<Sample<double>&>(m2) *= 3.0;
    cout << "After *= 3.0:\n";
    PrintMatrix(m2, "m2");

    // Тест 3: Поэлементные операции между матрицами
    PrintSeparator();
    cout << "Test 3: Element-wise Matrix Operations\n";
    Matrix m3;
    m3.Resize(2, 2);
    m3(0, 0) = 1.0;
    m3(0, 1) = 2.0;
    m3(1, 0) = 3.0;
    m3(1, 1) = 4.0;
    
    Matrix m4;
    m4.Resize(2, 2);
    m4(0, 0) = 5.0;
    m4(0, 1) = 6.0;
    m4(1, 0) = 7.0;
    m4(1, 1) = 8.0;
    
    m3 += m4;
    cout << "m3 after += m4:\n";
    PrintMatrix(m3, "m3");

    // Тест 4: Матричное произведение
    PrintSeparator();
    cout << "Test 4: Matrix Product\n";
    Matrix ma;
    ma.Resize(2, 2);
    ma(0, 0) = 1.0;
    ma(0, 1) = 2.0;
    ma(1, 0) = 3.0;
    ma(1, 1) = 4.0;
    
    Matrix mb;
    mb.Resize(2, 2);
    mb(0, 0) = 2.0;
    mb(0, 1) = 0.0;
    mb(1, 0) = 1.0;
    mb(1, 1) = 2.0;
    
    Matrix mc = ma * mb;
    cout << "ma * mb:\n";
    PrintMatrix(mc, "Result");

    // Тест 5: Транспонирование
    PrintSeparator();
    cout << "Test 5: Matrix Transpose\n";
    Matrix m_orig;
    m_orig.Resize(2, 3);
    m_orig(0, 0) = 1.0;
    m_orig(0, 1) = 2.0;
    m_orig(0, 2) = 3.0;
    m_orig(1, 0) = 4.0;
    m_orig(1, 1) = 5.0;
    m_orig(1, 2) = 6.0;
    PrintMatrix(m_orig, "Original");
    
    Matrix m_trans = ~m_orig;
    PrintMatrix(m_trans, "Transposed");

    // Тест 6: Определитель 3x3
    PrintSeparator();
    cout << "Test 6: Determinant of 3x3 Matrix\n";
    Matrix m5;
    m5.Resize(3, 3);
    m5(0, 0) = 1.0;
    m5(0, 1) = 2.0;
    m5(0, 2) = 3.0;
    m5(1, 0) = 0.0;
    m5(1, 1) = 1.0;
    m5(1, 2) = 4.0;
    m5(2, 0) = 5.0;
    m5(2, 1) = 6.0;
    m5(2, 2) = 0.0;
    PrintMatrix(m5, "Matrix m5");
    cout << "Determinant: " << std::fixed << std::setprecision(2) 
         << m5.CalculateDeterminant() << "\n";

    // Тест 7: Единичная матрица
    PrintSeparator();
    cout << "Test 7: Identity Matrix\n";
    Matrix identity;
    identity.Resize(3, 3);
    identity.MakeIdentity();
    PrintMatrix(identity, "Identity 3x3");

    // Тест 8: Матрица комплексных чисел 2x2
    PrintSeparator();
    cout << "Test 8: Complex Matrix 2x2 and Its Determinant\n";
    SampleComplex mc1(2);
    mc1(0, 0) = std::complex<double>(1.0, 0.0);
    mc1(0, 1) = std::complex<double>(2.0, 1.0);
    mc1(1, 0) = std::complex<double>(3.0, 2.0);
    mc1(1, 1) = std::complex<double>(4.0, 0.0);
    PrintComplexMatrix(mc1, "Complex Matrix");
    std::complex<double> det_mc1 = mc1.CalculateDeterminant();
    cout << "Determinant: (" << std::fixed << std::setprecision(2)
         << std::real(det_mc1) << ", " << std::imag(det_mc1) << "i)\n";

    // Тест 9: Матрица комплексных чисел 3x3
    PrintSeparator();
    cout << "Test 9: Complex Matrix 3x3 and Its Determinant\n";
    SampleComplex mc2(3);
    mc2(0, 0) = std::complex<double>(1.0, 0.0);
    mc2(0, 1) = std::complex<double>(2.0, 1.0);
    mc2(0, 2) = std::complex<double>(1.0, 1.0);
    mc2(1, 0) = std::complex<double>(0.0, 1.0);
    mc2(1, 1) = std::complex<double>(3.0, 0.0);
    mc2(1, 2) = std::complex<double>(2.0, 1.0);
    mc2(2, 0) = std::complex<double>(1.0, 2.0);
    mc2(2, 1) = std::complex<double>(0.0, 1.0);
    mc2(2, 2) = std::complex<double>(1.0, 0.0);
    PrintComplexMatrix(mc2, "Complex Matrix 3x3");
    std::complex<double> det_mc2 = mc2.CalculateDeterminant();
    cout << "Determinant: (" << std::fixed << std::setprecision(2)
         << std::real(det_mc2) << ", " << std::imag(det_mc2) << "i)\n";

    // Тест 10: Модуль, фаза, интенсивность
    PrintSeparator();
    cout << "Test 10: Complex Matrix Properties\n";
    SampleComplex mc3(2);
    mc3(0, 0) = std::complex<double>(3.0, 4.0);
    mc3(0, 1) = std::complex<double>(1.0, 0.0);
    mc3(1, 0) = std::complex<double>(0.0, 2.0);
    mc3(1, 1) = std::complex<double>(1.0, 1.0);
    
    Sample<double> modulus = mc3.GetModulus();
    cout << "Modulus:\n";
    for (int i = 0; i < mc3.GetSizeX(); ++i)
    {
        for (int j = 0; j < mc3.GetSizeY(); ++j)
        {
            cout << std::fixed << std::setprecision(2) << std::setw(10) << modulus(i, j);
        }
        cout << "\n";
    }

    // Тест 11: Сравнение матричного произведения с Boost
    PrintSeparator();
    cout << "Test 11: Matrix Multiplication Comparison with Boost\n";

    int size = 512;
    Matrix custom_m1, custom_m2;
    custom_m1.Resize(size, size);
    custom_m2.Resize(size, size);

    // Инициализируем случайными значениями
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            custom_m1(i, j) = 1.0 + (i + j) % 10;
            custom_m2(i, j) = 1.0 + (i * j) % 10;
        }
    }

    // Наше умножение
    auto start = std::chrono::high_resolution_clock::now();
    Matrix custom_result = custom_m1 * custom_m2;
    auto end = std::chrono::high_resolution_clock::now();
    double custom_time = std::chrono::duration<double>(end - start).count();
    cout << "Custom implementation time: " << custom_time << " seconds\n";

#ifdef BOOST_AVAILABLE
    // Boost умножение
    using namespace boost::numeric::ublas;
    matrix<double> boost_m1(size, size);
    matrix<double> boost_m2(size, size);

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            boost_m1(i, j) = 1.0 + (i + j) % 10;
            boost_m2(i, j) = 1.0 + (i * j) % 10;
        }
    }

    start = std::chrono::high_resolution_clock::now();
    matrix<double> boost_result = prod(boost_m1, boost_m2);
    end = std::chrono::high_resolution_clock::now();
    double boost_time = std::chrono::duration<double>(end - start).count();
    cout << "Boost implementation time: " << boost_time << " seconds\n";
    cout << "Speedup: " << (custom_time / boost_time) << "x\n";
#else
    cout << "Boost library not available for comparison\n";
#endif

    // Тест 12: 2D FFT на SampleComplex
    PrintSeparator();
    cout << "Test 12: 2D FFT of 512x512 Complex Sample\n";

#ifdef FFTW_AVAILABLE
    int fft_size = 512;
    SampleComplex fft_input(fft_size);
    fft_input.SetValue(std::complex<double>(1.0, 0.0));

    // Подготовка FFTW
    fftw_complex* in = (fftw_complex*)fft_input.GetPointer();
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_size * fft_size);

    fftw_plan plan = fftw_plan_dft_2d(fft_size, fft_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    start = std::chrono::high_resolution_clock::now();
    fftw_execute(plan);
    end = std::chrono::high_resolution_clock::now();
    double fft_time = std::chrono::duration<double>(end - start).count();

    cout << "2D FFT execution time: " << fft_time << " seconds\n";

    // Вывод результата FFT в файл
    std::ofstream fft_file("fft_result.txt");
    fft_file << fft_size << " " << fft_size << "\n";
    for (int i = 0; i < fft_size; ++i)
    {
        for (int j = 0; j < fft_size; ++j)
        {
            if (j > 0) fft_file << " ";
            fft_file << std::fixed << std::setprecision(2) 
                     << std::abs(out[i * fft_size + j][0]) << "+"
                     << std::abs(out[i * fft_size + j][1]) << "i";
        }
        fft_file << "\n";
    }
    fft_file.close();
    cout << "FFT result saved to fft_result.txt\n";
    cout << "FFT[0,0] (delta function peak): " 
         << std::fixed << std::setprecision(2)
         << out[0][0] << " + " << out[0][1] << "i\n";

    fftw_destroy_plan(plan);
    fftw_free(out);
#else
    cout << "FFTW library not available. Skipping FFT test.\n";
#endif

    PrintSeparator();
    cout << "All tests completed!\n";

    return 0;
}
