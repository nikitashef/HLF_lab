#include "lab3.h"
#include <sstream>
#include <string>

using std::cout;
using std::endl;

void PrintSeparator()
{
    cout << "\n" << std::string(60, '=') << "\n";
}

int main()
{
    cout << "========== Sample<T> Class Template Testing ==========\n";

    // Тест 1: Resize квадратная выборка
    PrintSeparator();
    cout << "Test 1: Resize (Square)\n";
    Sample<double> s1;
    s1.Resize(3);
    cout << "Created 3x3 sample\n";
    cout << "GetSize(): " << s1.GetSize() << "\n";
    cout << "GetSizeX(): " << s1.GetSizeX() << "\n";
    cout << "GetSizeY(): " << s1.GetSizeY() << "\n";
    cout << "IsSquare(): " << (s1.IsSquare() ? "true" : "false") << "\n";
    cout << "IsZeroSize(): " << (s1.IsZeroSize() ? "true" : "false") << "\n";

    // Тест 2: Resize прямоугольная выборка
    PrintSeparator();
    cout << "Test 2: Resize (Rectangular)\n";
    Sample<double> s2;
    s2.Resize(2, 4);
    cout << "Created 2x4 sample\n";
    cout << "GetSizeX(): " << s2.GetSizeX() << "\n";
    cout << "GetSizeY(): " << s2.GetSizeY() << "\n";
    cout << "IsSquare(): " << (s2.IsSquare() ? "true" : "false") << "\n";
    cout << "IsZeroSize(): " << (s2.IsZeroSize() ? "true" : "false") << "\n";

    // Тест 3: IsZeroSize & IsEqualSize
    PrintSeparator();
    cout << "Test 3: IsZeroSize & IsEqualSize\n";
    Sample<double> s3;
    cout << "Empty sample IsZeroSize(): " << (s3.IsZeroSize() ? "true" : "false") << "\n";
    
    s3.Resize(2, 3);
    cout << "2x3 sample IsZeroSize(): " << (s3.IsZeroSize() ? "true" : "false") << "\n";
    cout << "s2 IsEqualSize(s3): " << (s2.IsEqualSize(s3) ? "true" : "false") << "\n";
    
    Sample<double> s3_copy;
    s3_copy.Resize(2, 3);
    cout << "s3 IsEqualSize(s3_copy): " << (s3.IsEqualSize(s3_copy) ? "true" : "false") << "\n";

    // Тест 4: operator() доступ к элементам
    PrintSeparator();
    cout << "Test 4: operator() - Access and Modify Elements\n";
    s1.SetValue(0.0);
    s1(0, 0) = 1.5;
    s1(1, 1) = 2.5;
    s1(2, 2) = 3.5;
    PrintSample(s1, "s1 after setting elements");

    // Тест 5: SetValue
    PrintSeparator();
    cout << "Test 5: SetValue\n";
    s2.SetValue(7.0);
    PrintSample(s2, "s2 after SetValue(7.0)");

    // Тест 6: GetMax
    PrintSeparator();
    cout << "Test 6: GetMax\n";
    Sample<double> s_max;
    s_max.Resize(2, 3);
    s_max(0, 0) = 1.1;
    s_max(0, 1) = 5.5;
    s_max(0, 2) = 2.2;
    s_max(1, 0) = 3.3;
    s_max(1, 1) = 4.4;
    s_max(1, 2) = 0.1;
    PrintSample(s_max, "s_max");
    cout << "GetMax(): " << s_max.GetMax() << "\n";

    // Тест 7: GetPointer
    PrintSeparator();
    cout << "Test 7: GetPointer\n";
    double* ptr = s_max.GetPointer();
    cout << "Pointer to first element: " << ptr[0] << "\n";
    cout << "All elements via pointer: ";
    for (int i = 0; i < 6; ++i)
    {
        cout << ptr[i] << " ";
    }
    cout << "\n";

    // Тест 8: operator+= с числом
    PrintSeparator();
    cout << "Test 8: operator+= (with scalar)\n";
    Sample<double> s8;
    s8.Resize(2, 2);
    s8.SetValue(1.0);
    PrintSample(s8, "s8 before += 2.0");
    s8 += 2.0;
    PrintSample(s8, "s8 after += 2.0");

    // Тест 9: operator*= с числом
    PrintSeparator();
    cout << "Test 9: operator*= (with scalar)\n";
    Sample<double> s9;
    s9.Resize(2, 2);
    s9.SetValue(2.0);
    PrintSample(s9, "s9 before *= 3.0");
    s9 *= 3.0;
    PrintSample(s9, "s9 after *= 3.0");

    // Тест 10: operator/= с числом
    PrintSeparator();
    cout << "Test 10: operator/= (with scalar)\n";
    Sample<double> s10;
    s10.Resize(2, 2);
    s10.SetValue(10.0);
    PrintSample(s10, "s10 before /= 2.0");
    s10 /= 2.0;
    PrintSample(s10, "s10 after /= 2.0");

    // Тест 11: operator= присваивание
    PrintSeparator();
    cout << "Test 11: operator= (assignment)\n";
    Sample<double> s11_src;
    s11_src.Resize(2, 3);
    s11_src.SetValue(5.0);
    PrintSample(s11_src, "s11_src");
    
    Sample<double> s11_dst;
    s11_dst = s11_src;
    PrintSample(s11_dst, "s11_dst (after assignment)");
    cout << "s11_dst IsEqualSize(s11_src): " << (s11_dst.IsEqualSize(s11_src) ? "true" : "false") << "\n";

    // Тест 12: operator+= с выборкой
    PrintSeparator();
    cout << "Test 12: operator+= (with Sample)\n";
    Sample<double> s12a, s12b;
    s12a.Resize(2, 2);
    s12a.SetValue(1.0);
    s12b.Resize(2, 2);
    s12b.SetValue(2.0);
    PrintSample(s12a, "s12a before");
    PrintSample(s12b, "s12b");
    s12a += s12b;
    PrintSample(s12a, "s12a after += s12b");

    // Тест 13: operator-= с выборкой
    PrintSeparator();
    cout << "Test 13: operator-= (with Sample)\n";
    Sample<double> s13a, s13b;
    s13a.Resize(3, 2);
    s13a.SetValue(5.0);
    s13b.Resize(3, 2);
    s13b.SetValue(2.0);
    PrintSample(s13a, "s13a before");
    PrintSample(s13b, "s13b");
    s13a -= s13b;
    PrintSample(s13a, "s13a after -= s13b");

    // Тест 14: operator*= с выборкой
    PrintSeparator();
    cout << "Test 14: operator*= (with Sample)\n";
    Sample<double> s14a, s14b;
    s14a.Resize(2, 2);
    s14a(0, 0) = 2.0;
    s14a(0, 1) = 3.0;
    s14a(1, 0) = 4.0;
    s14a(1, 1) = 5.0;
    s14b.Resize(2, 2);
    s14b(0, 0) = 1.0;
    s14b(0, 1) = 2.0;
    s14b(1, 0) = 3.0;
    s14b(1, 1) = 4.0;
    PrintSample(s14a, "s14a before");
    PrintSample(s14b, "s14b");
    s14a *= s14b;
    PrintSample(s14a, "s14a after *= s14b");

    // Тест 15: Save и Load
    PrintSeparator();
    cout << "Test 15: Save and Load\n";
    Sample<double> s15_save;
    s15_save.Resize(2, 3);
    s15_save(0, 0) = 1.1;
    s15_save(0, 1) = 2.2;
    s15_save(0, 2) = 3.3;
    s15_save(1, 0) = 4.4;
    s15_save(1, 1) = 5.5;
    s15_save(1, 2) = 6.6;
    PrintSample(s15_save, "s15_save");
    
    // Сохраняем в строку
    std::stringstream ss;
    s15_save.Save(ss);
    std::string saved_data = ss.str();
    cout << "Saved to stream:\n" << saved_data << "\n";
    
    // Загружаем из строки
    std::stringstream ss2(saved_data);
    Sample<double> s15_load;
    s15_load.Load(ss2);
    PrintSample(s15_load, "s15_load (after loading)");

    // Тест 16: Обработка ошибок
    PrintSeparator();
    cout << "Test 16: Error Handling\n";
    
    try {
        Sample<double> s_err;
        s_err.Resize(-1);
    } catch (const std::exception& e) {
        cout << "Caught exception for negative size: " << e.what() << "\n";
    }
    
    try {
        Sample<double> s_err;
        s_err.Resize(2, 3);
        int sz = s_err.GetSize(); // Должно быть прямоугольная!
    } catch (const std::exception& e) {
        cout << "Caught exception for GetSize() on non-square: " << e.what() << "\n";
    }
    
    try {
        Sample<double> s_err;
        s_err.Resize(2, 2);
        s_err /= 0.0;
    } catch (const std::exception& e) {
        cout << "Caught exception for division by zero: " << e.what() << "\n";
    }
    
    try {
        Sample<double> s_err1, s_err2;
        s_err1.Resize(2, 2);
        s_err2.Resize(3, 3);
        s_err1 += s_err2;
    } catch (const std::exception& e) {
        cout << "Caught exception for incompatible sizes: " << e.what() << "\n";
    }
    
    try {
        Sample<double> s_err;
        double val = s_err.GetMax(); // пустая выборка
    } catch (const std::exception& e) {
        cout << "Caught exception for GetMax() on empty sample: " << e.what() << "\n";
    }
    
    try {
        Sample<double> s_err;
        s_err.Resize(2, 3);
        int idx = s_err.GetSize(); // прямоугольная!
    } catch (const std::exception& e) {
        cout << "Caught exception for GetSize() on non-square: " << e.what() << "\n";
    }

    // Тест 17: Проверка с другим типом (int)
    PrintSeparator();
    cout << "Test 17: Template with int type\n";
    Sample<int> s_int;
    s_int.Resize(2, 2);
    s_int.SetValue(5);
    PrintSample(s_int, "s_int");
    s_int += 3;
    PrintSample(s_int, "s_int after += 3");

    // Тест 18: operator() bounds checking
    PrintSeparator();
    cout << "Test 18: Bounds Checking\n";
    try {
        Sample<double> s_bounds;
        s_bounds.Resize(2, 2);
        double val = s_bounds(10, 10); // Out of bounds
    } catch (const std::exception& e) {
        cout << "Caught exception for out of bounds access: " << e.what() << "\n";
    }

    cout << "\n========== All tests completed! ==========\n";
    return 0;
}
