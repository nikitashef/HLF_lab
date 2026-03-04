


//-------------------------------------------------------------------------------------------------
// базовый класс для выборки (квадратной или прямоугольной)
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
    bool IsEqualSize(const Sample<PAR> &temp) const;

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
    Sample<PAR>& operator= (const Sample<PAR> &temp);
    // поэлементное сложение двух выборок
    Sample<PAR>& operator+=(const Sample<PAR> &temp);
    // поэлементное вычитание двух выборок
    Sample<PAR>& operator-=(const Sample<PAR>& temp);
    // поэлементное перемножение двух выборок
    virtual Sample<PAR>& operator*=(const Sample<PAR> &temp);

    // вывод выборки в поток (файл или экран)
    std::ostream& Save(std::ostream& out);
    // перегрузка оператора чтения выборки из файла 
    std::istream& Load(std::istream& out);

protected:
    // проверяет квадратная ли выборка, если нет - создает исключение
    void CheckSquare() const;
};
//-------------------------------------------------------------------------------------------------
