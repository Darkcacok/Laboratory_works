#ifndef HUNGARIAN_H_
#define HUNGARIAN_H_

#include <iostream>
#include <fstream>
#include <vector>

class Hungarian{
public:
    Hungarian();
    Hungarian(std::string &f_path);
    ~Hungarian();

    void print();

private:
    void parse_file(std::ifstream &file);
    std::vector<int> *parse_line(std::string &line);
    int **m_matrix;
    int m_rows;
    int m_columns;
};

#endif //HUNGARIAN_H_