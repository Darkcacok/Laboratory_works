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
    // trim from start (in place)
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

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