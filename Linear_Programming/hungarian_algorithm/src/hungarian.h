#ifndef HUNGARIAN_H_
#define HUNGARIAN_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>

#include "matrix.h" 
#include "constants.h"




class Hungarian{
public:

    //Matrix help_matrix;

    Hungarian(Matrix &m_matrix);
    ~Hungarian();

    void solve();

    Matrix C;
    int row;
    int col;
    int **C0;
    int **help_matrix;
    int *v_stocks; // |
    int *h_stocks; // --
    int *v_residuals;
    int *h_residuals;
    Mark *row_mark;
    Mark *col_mark;
    Mark **m_matrix;
    std::vector<std::pair<int,int>> chain;
    void get_solve();


private:
    bool check_balance(int *customers, int *sellers);
    void preliminary_stage();
    void mark_column_matrix();//5
    bool find_not_busy_zero(int *indx_i, int *indx_j); //6 && 7. Ищем не занятый 0
    bool find_zero_in_busy(int indx_i);//8
    void build_chain(int indx_i, int indx_j); //9
    void recount(); //10 && 11
    void find_min_from_not_busy_and_sub();//12
    int find_min_in_row(int **a, int n, int size);
    int find_min_in_col(int **a, int n, int size);
    bool find_star(int indx);
    bool find_prime(int indx);
    int check_residuals();

    void clean_init_mark();


    //Print functions test
    void print_C();
    void print_x();
    void print_C_mark();
};

#endif //HUNGARIAN_H_