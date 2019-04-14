#include "matrix.h"

Matrix::Matrix()
{
    this->rows = 0;
    this->columns = 0;
    this->A = NULL;
}

Matrix::~Matrix()
{
    if(A != nullptr)
    {
        for(int i = 0; i < rows; ++i)
            delete[] A[i];

        delete[] A;
    }
}

Matrix::Matrix(const Matrix &m_matrix)
{
    rows = m_matrix.rows;
    columns = m_matrix.columns;

    A = new int*[rows];

    for(int i = 0; i < rows; ++i)
        A[i] = new int[columns];

    for(int i = 0; i < rows; ++i)
        for(int j = 0; j < columns; ++j)
            A[i][j] = m_matrix.A[i][j];
}

Matrix &Matrix::operator=(const Matrix &m_matrix)
{
    if(A != nullptr)
    {
        for(int i = 0; i < rows; ++i)
            delete[] A[i];

        delete[] A;
    }

    rows = m_matrix.rows;
    columns = m_matrix.columns;

    A = new int*[rows];

    for(int i = 0; i < rows; ++i)
        A[i] = new int[columns];

    for(int i = 0; i < rows; ++i)
        for(int j = 0; j < columns; ++j)
            A[i][j] = m_matrix.A[i][j];

    return *this;
}

Matrix::Matrix(std::string &f_path)
{
    std::ifstream data;
    data.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        data.open(f_path, std::ios::in);
        parse_file(data);
        data.close();
    }
    catch(std::string str)
    {
        throw str;
    }
    catch(std::ifstream::failure &e)
    {
       throw std::string("Exception opening/reading/closing ") + f_path + "\n" + std::string(e.what()) + "\n";
    }
}

void Matrix::parse_file(std::ifstream &file)
{
    std::string line;
    std::vector<std::string> lines;
    
    if(file.is_open())
    {
        while(file.good() && std::getline(file, line))
        {
            lines.push_back(line);
        }
    }

    A = new int*[lines.size()];
    rows = lines.size();
    columns = -1;

    for(int i = 0; i < rows; ++i)
    {
        try
        {
            std::vector<int> *array = parse_line(lines[i]);

            if(columns == -1)
                columns = array->size();
            else if(columns != array->size()){
                    delete array;
                    throw std::string("Неправельный формат входных данных\n");
            }
            A[i] = new int[columns];

            for(int j = 0; j < columns; ++j)
                A[i][j] = (*array)[j];

            delete array;
        }
        catch(std::string str)
        {
            throw str;
        }
    }
}


std::vector<int> *Matrix::parse_line(std::string &line)
{
    std::vector<int> *array = new std::vector<int>();
    std::vector<std::string> words;
    
    boost::split(words, line, boost::is_any_of(" ,"), boost::token_compress_on);

    for(int i = 0; i < words.size(); i++)
    {
        try
        {
            int number = std::stoi(words[i]);
            array->push_back(number);
        }
        catch(std::invalid_argument &e)
        {
            throw std::string("В файле присутсвуют не только лишь цифры\n");
        }
        
    }

    return array;
}


void Matrix::substraction(int i, int j, int x)
{
    if(A != nullptr && i < rows && j < columns)
        A[i][j] -= x;
}

int Matrix::find_min_in_row(int row, int n)
{
    int min = A[row][0];

    for(int  i = 1; i < n && i < columns; ++i)
    {
        if(min > A[row][i])
            min = A[row][i];
    }

    return min;
}

int Matrix::find_min_in_col(int col, int n)
{
    int min = A[0][col];

     for(int  i = 1; i < n && i < rows; ++i)
    {
        if(min > A[i][col])
            min = A[i][col];
    }

    return min;
}

int Matrix::get_row_count()
{
    return rows;
}

int Matrix::get_col_count()
{
    return columns;
}


std::ostream &operator<<(std::ostream &os,  const Matrix &m_matrix)
{
    for(int i = 0; i < m_matrix.rows; ++i)
    {
        for(int j = 0; j < m_matrix.columns; ++j)
            os << m_matrix.A[i][j] << "\t";
        os << std::endl;
    }

    return os;
}


bool Matrix::add_row(int *array, int size)
{
    if(size != this->columns)
        return false;

    int **a;

    a = new int*[rows + 1];

    for(int i = 0; i < rows + 1; ++i)
        a[i] = new int[columns];
    
    for(int i = 0; i < rows - 1; ++i)
        for(int j = 0; j < columns; ++j)
        {
            a[i][j] = A[i][j];
        }

    for(int i = 0; i < columns; ++i)
        a[rows - 1][i] = array[i];

    for(int i = 0; i < columns; ++i)
        a[rows][i] = A[rows - 1][i];

    //delete
    for(int i = 0; i < rows; ++i)
        delete[] A[i];
    
    delete[] A;

    A = a;
    ++rows;
}

bool Matrix::add_col(int *array, int size)
{
    if(size != this->rows)
        return false;

    int **a;

    a = new int*[rows];

    for(int i = 0; i < rows; ++i)
        a[i] = new int[columns + 1];
    
    for(int j = 0; j < columns - 1; ++j)
        for(int i = 0; i < rows; ++i)
            a[i][j] = A[i][j];

    for(int i = 0; i < rows; ++i)
        a[i][columns - 1] = array[i];

    for(int i = 0; i < rows; ++i)
        a[i][columns] = A[i][columns - 1];

    //delete
    for(int i = 0; i < rows; ++i)
        delete[] A[i];
    
    delete[] A;

    A = a;
    ++columns;
}