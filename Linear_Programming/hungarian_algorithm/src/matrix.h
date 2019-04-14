#ifndef MATRIX_H_
#define MATRIX_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

class Matrix
{
private:

    void parse_file(std::ifstream &file);
    std::vector<int> *parse_line(std::string &line);


public:
    int **A;
    int rows;
    int columns;
    Matrix();
    ~Matrix();
    Matrix(std::string &f_path);
    Matrix(const Matrix &m_matrix);
    Matrix &operator=(const Matrix &m_matrix);

    void substraction(int i, int j, int x);
    bool add_row(int *array, int size);
    bool add_col(int *array, int size);
    int find_min_in_row(int row, int n);
    int find_min_in_col(int col, int n);
    int get_row_count();
    int get_col_count();

    friend std::ostream &operator<<(std::ostream &os, Matrix const &m_matrix);
};




#endif //MATRIX_H_