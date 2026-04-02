#include "lr6calculation.h"
#include <complex>
#include <cmath>

// Заполнение функции (круг или квадрат)
void Lr6Calculation::fillFunction(const Lr6Param& param)
{
    function.assign(param.N, std::vector<double>(param.N, 0.0));
    double center = param.N / 2.0;
    double half = param.size / 2.0;
    double half2 = half * half;
    for (int i = 0; i < param.N; ++i)
    {
        for (int j = 0; j < param.N; ++j)
        {
            double x = (i - center) * param.dx;
            double y = (j - center) * param.dx;
            if (param.isCircle)
            {
                // Круг
                double r2 = x * x + y * y;
                if (r2 <= half2)
                    function[i][j] = 1.0;
            }
            else
            {
                // Квадрат
                if (x >= -half && x <= half && y >= -half && y <= half)
                    function[i][j] = 1.0;
            }
        }
    }
}

// Быстрое преобразование Фурье по строкам и столбцам
static void fft1d(std::vector<std::complex<double>>& a, bool invert)
{
    int n = (int)a.size();
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(a[i], a[j]);
    }
    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2 * M_PI / len * (invert ? -1 : 1);
        std::complex<double> wlen(cos(ang), sin(ang));
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
}

void Lr6Calculation::calcSpectrum(const Lr6Param& param)
{
    int N = param.N;
    spectrum.assign(N, std::vector<double>(N, 0.0));
    // Копируем функцию в комплексный буфер
    std::vector<std::vector<std::complex<double>>> data(N, std::vector<std::complex<double>>(N));
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            data[i][j] = function[i][j];
    // FFT по строкам
    std::vector<std::complex<double>> buf(N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) buf[j] = data[i][j];
        fft1d(buf, false);
        for (int j = 0; j < N; ++j) data[i][j] = buf[j];
    }
    // FFT по столбцам
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < N; ++i) buf[i] = data[i][j];
        fft1d(buf, false);
        for (int i = 0; i < N; ++i) data[i][j] = buf[i];
    }
    // Вычисляем спектр с fftshift и нормировкой
    double maxVal = 0.0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            int ii = (i + N/2) % N;
            int jj = (j + N/2) % N;
            double val = std::norm(data[i][j]);
            spectrum[ii][jj] = val;
            if (val > maxVal) maxVal = val;
        }
    if (maxVal > 0.0)
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                spectrum[i][j] /= maxVal;
}

void Lr6Calculation::calcSections()
{
    int N = (int)spectrum.size();
    sectionX.resize(N);
    sectionY.resize(N);
    int mid = N / 2;
    for (int i = 0; i < N; ++i) {
        sectionX[i] = spectrum[mid][i];
        sectionY[i] = spectrum[i][mid];
    }
}

void Lr6Calculation::Calculate(const Lr6Param& param)
{
    // Сначала формируем функцию
    fillFunction(param);
    // Потом считаем спектр
    calcSpectrum(param);
    // Потом сечения
    calcSections();
}
