#include <iostream>
#include <fstream>
#include <cmath>
#include "lab1.h"

using namespace std;



void main() 
{
    double coordinate_x = 0, coordinate_y = 0;
    char mode = 2, input_mode = 0, write_en = 0;

    do 
    {

    cout << "\n-----------------------MENU-----------------------";
    cout << "\n\t1 - rectangle, 2 - circle, 0 - exit\n";
    cout << "\nChoose coordinate mode: ";
    cin >> mode;

    if (mode == '0'){cout << "\n\t EXIT FROM PROGRAMM\n"; break;}

    cout << "\n\t1 - input from file, 2 - input from console\n";
    cout << "\nChoose input mode: ";
    cin >> input_mode;
    
    cout << "\nSave results? [y/n]: ";
    cin >> write_en;

    if (input_mode == '1') {
        file_input_coordinates(&coordinate_x, &coordinate_y);
    } else if (input_mode == '2') {
        console_input_coordinates(&coordinate_x, &coordinate_y);
    }

    switch (mode)
    {
        case '1': // прямоугольник
        console_show_ress(point_in_rect(coordinate_x, coordinate_y), coordinate_x, coordinate_y, write_en);
        break;
        
        case '2': // круг
        console_show_ress(point_in_circle(coordinate_x, coordinate_y), coordinate_x, coordinate_y, write_en);
        break;

        default:
            cout << "You choose incorrect mode";
    }

    }while (mode != '0');

}

void console_input_coordinates(double* x, double* y)
{
    cout << "input x coordinate:\n";
    cin >> *x;
    cout << "input y coordinate:\n";
    cin >> *y;
}

void file_input_coordinates(double* x, double* y)
{
    ifstream out_file("./oordinates.txt");
    out_file >> *x >> *y;
}

bool point_in_circle (double x, double y)
{
    return (sqrt(x * x + y * y) <= 1);
}

bool point_in_rect(double x, double y)
{
    if (x > 1 || y > 1) {
        return false;
    } else {
        return true;
    }
}

void console_show_ress(bool res, double x, double y, char save_en)
{
    if (res){
        cout << "\nThe point is INTO shape\n";
        if (save_en == 'y') {
            ofstream out_file ("results.txt");
            out_file << "\nThe point (" << x << ", " << y << ") is INTO shape\n";  
        }
    } else {
        cout << "\nThe point is OUT of shape\n";
        if (save_en == 'y') {
            ofstream out_file ("results.txt");
            out_file << "\nThe point (" << x << ", " << y << ") is OUT of shape\n";  
        }
    }


}

