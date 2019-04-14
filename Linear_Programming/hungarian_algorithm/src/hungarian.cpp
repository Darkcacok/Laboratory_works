#include "hungarian.h"
#include "cmath"
#include <iomanip>
#include <boost/algorithm/string.hpp>

Hungarian::Hungarian(Matrix &m_matrix)
{
    C = m_matrix;
    row = m_matrix.get_row_count();
    col = m_matrix.get_col_count();
    int customers, sellers;
    bool flag = check_balance(&customers, &sellers);

    if(!flag)
    {
        if(customers < sellers)
        {
            int *array = new int[row];
            for(int i = 0; i < row - 1; ++i)
                array[i] = 0;
            array[row - 1] = abs(customers - sellers);

            C.add_col(array, row);
        }
        if(customers > sellers)
        {
            int *array = new int[col];
            for(int i = 0; i < col - 1; ++i)
                array[i] = 0;
            array[col - 1] = abs(customers - sellers);

            C.add_row(array, col);
        }
        row = C.get_row_count();
        col = C.get_col_count();
    }

    C0 = new int *[row - 1];
    help_matrix = new int *[row - 1];
    v_stocks = new int[row - 1];
    v_residuals = new int[row - 1];
    h_stocks = new int[col - 1];
    h_residuals = new int[col - 1];

    row_mark = new Mark[row - 1];
    col_mark = new Mark[col - 1];
    this->m_matrix = new Mark*[row - 1];

    for (int i = 0; i < row - 1; ++i)
    {
        C0[i] = new int[col - 1];
        help_matrix[i] = new int[col - 1];
        this->m_matrix[i] = new Mark[col - 1];
    }

    for (int i = 0; i < row - 1; ++i)
    {
        for (int j = 0; j < col - 1; ++j)
            C0[i][j] = C.A[i][j];
    }

    for (int i = 0; i < row - 1; ++i)
    {
        v_residuals[i] = v_stocks[i] = C.A[i][col - 1];
    }

    for (int i = 0; i < col - 1; ++i)
    {
        h_residuals[i] = h_stocks[i] = C.A[row - 1][i];
    }

    clean_init_mark();
}

bool Hungarian::check_balance(int *customers, int *sellers)
{
    *customers = *sellers = 0;

    for(int i = 0; i < row; ++i)
        (*sellers)+= C.A[i][col - 1];
    
    for(int i = 0; i < col; ++i)
        (*customers)+=C.A[row - 1][i];

    if((*customers) == (*sellers))
    {
        std::cout << "Сбалансированная транспортная задача: кол-во товаров у продавцов = кол-во требуемых товаров у потребителей, " << *customers << " = " << *sellers << std::endl;
        return true;   
    }

    if((*customers) < (*sellers))
    {
        //col = col + 1;
        std::cout << "Не сбалансированная транспортная задача: кол-во товаров у продавцов > кол-во требуемых товаров у потребителей, " << *sellers << " > " << *customers << std::endl;
        return false;
    }

    if((*customers) > (*sellers))
    {
        //row = row + 1;
        std::cout << "Не сбалансированная транспортная задача: кол-во товаров у продавцов < кол-во требуемых товаров у потребителей, " << *sellers << " > " << *customers << std::endl;
        return false;
    }
}

void Hungarian::solve()
{
    //Вычитаем минимальный элемнт
    preliminary_stage();


    //Строим начальную мартцу перевозок
    for (int j = 0; j < col - 1; ++j)
        for (int i = 0; i < row - 1; ++i)
        {
            if(v_residuals[i] == 0 || h_residuals[j] == 0)
            {
                help_matrix[i][j] = 0;
                continue;
            }
            if (C0[i][j] != 0)
                help_matrix[i][j] = 0;
            else
            {
                if (v_residuals[i] <= h_residuals[j])
                {
                    help_matrix[i][j] = v_residuals[i];
                    v_residuals[i] = v_residuals[i] - help_matrix[i][j];
                    h_residuals[j] = h_residuals[j] - help_matrix[i][j];
                }
                else
                {
                    help_matrix[i][j] = h_residuals[j];
                    h_residuals[j] = 0;
                    v_residuals[i] = v_residuals[i] - help_matrix[i][j];
                }
            }
        }




    while( check_residuals() != 0)
    {

        std::cout << "C = " << std::endl;
        print_C();
        std::cout << std::endl << "X = " << std::endl;
        print_x();
        std::cout << "Cуммарная невязка равна = " << check_residuals() << std::endl;
        std::cout << "|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|" << std::endl;

        //5. Помечаем знаком + столбцы матрицы
        mark_column_matrix();
        std::cout << std::endl << "C = " << std::endl;
        print_C_mark();

         //6.
        int indx_i, indx_j;
        bool flag = find_not_busy_zero(&indx_i, &indx_j);
        while(!flag)
        {
            flag = find_not_busy_zero(&indx_i, &indx_j);
        }


        std::cout << "C = " << std::endl;
        print_C_mark();
        std::cout << std::endl;
        //9
        build_chain(indx_i, indx_j); 
        print_chain();
        //10
        recount();        
    }
     std::cout << std::endl << "X = " << std::endl;
    print_x();
    std::cout << "Cуммарная невязка равна = " << check_residuals() << std::endl;
    std::cout << "|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|" << std::endl;
}

void Hungarian::preliminary_stage()
{
    for (int i = 0; i < row - 1; ++i)
    {
        int min = find_min_in_row(C0, i, col - 1);

        for (int j = 0; j < col - 1; ++j)
            C0[i][j] -= min;
    }

    for (int i = 0; i < col - 1; ++i)
    {
        int min = find_min_in_col(C0, i, row - 1);

        for (int j = 0; j < row - 1; ++j)
            C0[j][i] -= min;
    }
}

void Hungarian::mark_column_matrix()
{
     for(int i = 0; i < col - 1; ++i)
    {
        if(h_residuals[i] == 0)
            col_mark[i] = Mark::plus;
    }
}

bool Hungarian::find_not_busy_zero(int *indx_i, int *indx_j)
{
    for(int i = 0; i < row - 1; ++i)
        for(int j = 0; j < col - 1; ++j)
        {
            if(col_mark[j] == Mark::plus || row_mark[i] == Mark::plus)
                continue;
            
            if(C0[i][j] != 0)
                continue;

            m_matrix[i][j] = Mark::prime;
            *indx_i = i;
            *indx_j = j;


            if(v_residuals[i] == 0)
            {
                row_mark[i] = Mark::plus;
                //8
                if(find_zero_in_busy(i))
                {
                    i = 0;
                    j = -1;
                    //find_not_busy_zero(indx_i, indx_j);
                }
            std::cout << std::endl << "C = " << std::endl;
            print_C_mark();
                
            }
            else
            {
                return true;
            }
        }

    //12
    find_min_from_not_busy_and_sub();
    return false;
}

bool Hungarian::find_zero_in_busy(int indx_i)
{
    bool flag = false;
    for(int j = 0; j < col - 1; ++j)
    {
        if(col_mark[j] == Mark::plus)
        {
            if(C0[indx_i][j] == 0 && help_matrix[indx_i][j] > 0)
            {
                flag = true;
                m_matrix[indx_i][j] = Mark::star;
                col_mark[j] = Mark::non;
            }
        }
    }

    return flag;
}

void Hungarian::build_chain(int indx_i, int indx_j)
{
    chain.push_back(std::make_pair(indx_i, indx_j));

    while(find_star(chain[chain.size() - 1].second))
    {
        if(!find_prime(chain[chain.size() - 1].first))
            break;
    }
}

void Hungarian::recount()
{
    std::vector<int> min;

    for(int i = 0; i < chain.size(); ++i)
    {
        if(m_matrix[chain[i].first][chain[i].second] == Mark::star)
            min.push_back(help_matrix[chain[i].first][chain[i].second]);
    }

    min.push_back(v_residuals[chain[0].first]);
    min.push_back(h_residuals[chain[chain.size() - 1].second]);

    print_mins_el(min);

    std::vector<int>::iterator result = std::min_element(std::begin(min), std::end(min));
    int min_el = min[std::distance(std::begin(min), result)];

    std::cout << "Минимальный элемент равен: " << min_el << std::endl;


    for(int i = 0; i < chain.size(); ++i)
    {
        if(m_matrix[chain[i].first][chain[i].second] == Mark::prime)
             help_matrix[chain[i].first][chain[i].second] +=min_el;
        if(m_matrix[chain[i].first][chain[i].second] == Mark::star)
            help_matrix[chain[i].first][chain[i].second] -=min_el;
    }
    
    //Пересчитывает невзяки
    for (int i = 0; i < row - 1; ++i)
    {
        v_residuals[i] = v_stocks[i];
    }

    for (int i = 0; i < col - 1; ++i)
    {
        h_residuals[i] = h_stocks[i];
    }

    for(int i = 0; i < row - 1; ++i)
        for(int j = 0; j < col - 1; ++j)
            v_residuals[i] -= help_matrix[i][j];
    
    for(int j = 0; j < col - 1; ++j)
        for(int i = 0; i < row - 1; ++i)
            h_residuals[j] -= help_matrix[i][j];

    clean_init_mark();
    chain.clear();
}

void Hungarian::find_min_from_not_busy_and_sub()
{

    std::cout << "Не нашли не занятых нулей. Пересчитываем матрицу. ";

    std::vector<int> min;

    for(int i = 0; i < row - 1; ++i)
        for(int j = 0; j < col - 1; ++j)
        {
            if(row_mark[i] != Mark::plus && col_mark[j] != Mark::plus)
                min.push_back(C0[i][j]);
        }

    std::vector<int>::iterator result = std::min_element(std::begin(min), std::end(min));
    int min_el = min[std::distance(std::begin(min), result)];

    std::cout << "Минимальный элемент равен: " << min_el << std::endl;

    for(int i = 0; i < row - 1; ++i)
        for(int j = 0; j < col - 1; ++j)
        {
            if(row_mark[i] != Mark::plus)
                C0[i][j] -= min_el;
            if(col_mark[j] == Mark::plus)
                C0[i][j] += min_el;
        }
    
    std::cout << "C = " << std::endl;
    print_C_mark();
    std::cout << std::endl;
}


bool Hungarian::find_star(int indx)
{

    for(int i = 0; i < row - 1; ++i)
    {
         if(m_matrix[i][indx] == Mark::star)
        {
            chain.push_back(std::make_pair(i, indx));
            return true;
        }
    }

    return false;
}

bool Hungarian::find_prime(int indx)
{

    for(int i = 0; i < col - 1; ++i)
    {
                if(m_matrix[indx][i] == Mark::prime)
        {
            chain.push_back(std::make_pair(indx, i));
            return true;
        }
    }

    return false;
}


int Hungarian::find_min_in_col(int **a, int n, int size)
{
    //int min = a[0][n];
    bool flag = true;
    int min;

    for (int i = 0; i < size; ++i)
    {
        if(flag)
        {
            min = a[i][n];
            flag = false;
        }
        
        if (min > a[i][n] && !flag)
            min = a[i][n];
    }

    if(flag)
        min = 0;

    return min;
}

int Hungarian::find_min_in_row(int **a, int n, int size)
{
    //int min = a[n][0];

    bool flag = true;
    int min;

    for (int i = 0; i < size; ++i)
    {
        if(flag)
        {
            min = a[n][i];
            flag = false;
        }

        if (min > a[n][i] && !flag)
            min = a[n][i];
    }


    if(flag)
        min = 0;

    return min;
}

int Hungarian::check_residuals()
{
    int sum = 0;
    
    for(int i = 0; i < row - 1; ++i)
        sum+=v_residuals[i];

    for(int i = 0; i < col - 1; ++i)
        sum+=h_residuals[i];

    return sum;
}




void Hungarian::clean_init_mark()
{
    for(int i = 0; i < row - 1; ++i)
    {
        for(int j = 0; j < col -1; ++j)
        {
            m_matrix[i][j] = Mark::non;
        }
    }

    for(int i = 0; i < row - 1; ++i)
        row_mark[i] = Mark::non;

    for(int i = 0; i < col - 1; ++i)
        col_mark[i] = Mark::non;
}


void Hungarian::print_C()
{
    for(int i = 0; i < row - 1; ++i)
    {
        for(int l = 0; l < col - 1; ++l){
            std::cout << "+";
            for(int k = 0; k < 5; ++k)
                std::cout << "-";
        }
        std::cout << "+" << std::endl;

        for(int j = 0; j < col - 1; ++j)
        {
            std::cout << "|";
            std::cout << std::setw(5) << std::right << C0[i][j];
        }
        std::cout << "|" << std::endl;
    }

    for(int l = 0; l < col - 1; ++l){
            std::cout << "+";
            for(int k = 0; k < 5; ++k)
                std::cout << "-";
        }

    std::cout << "+" << std::endl;

}

void Hungarian::print_x()
{
    for(int i = 0; i < row - 1; ++i)
    {

        for(int l = 0; l < col; ++l){
            std::cout << "+";
            for(int k = 0; k < 5; ++k)
                std::cout << "-";
        }
        std::cout << "+" << std::endl;

        for(int j = 0; j < col - 1; ++j)
        {
            std::cout << "|";
            std::cout << std::setw(5) << std::right << help_matrix[i][j];
        }
        std::cout << "||" << std::setw(4) << v_residuals[i];
        std::cout << "|" << std::endl;
    }

    for(int indx = 0; indx < 2; ++indx)
    {
        for(int l = 0; l < col; ++l){
            std::cout << "+";
            for(int k = 0; k < 5; ++k)
                std::cout << "-";
        }
        std::cout << "+" << std::endl;
    }

    for(int i = 0; i < col - 1; ++i)
    {
            std::cout << "|";
            std::cout << std::setw(5) << std::right << h_residuals[i];
    }
    std::cout << "|" << std::setw(6) << std::right << "|" << std::endl;

    for(int l = 0; l < col; ++l){
            std::cout << "+";
            for(int k = 0; k < 5; ++k)
                std::cout << "-";
        }
        std::cout << "+" << std::endl;
}

void Hungarian::print_C_mark()
{
    for(int l = 0; l < col; ++l){
            std::cout << "+";
            for(int k = 0; k < 5; ++k)
                std::cout << "-";
        }
    std::cout << "+" << std::endl;

    std::cout << "|";
    for(int i = 0; i < col - 1; ++i)
    {
        if(col_mark[i] == Mark::plus)
        {
            std::cout << std::setw(5) << std::right << "+";
            std::cout << "|";
        }
        else
        {
            std::cout << std::setw(6) << std::right << "|";
        }
    }
    std::cout << std::setw(6) << std::right << "|";
    std::cout << std::endl;

    for(int l = 0; l < col; ++l){
            std::cout << "+";
            for(int k = 0; k < 5; ++k)
                std::cout << "-";
        }
    std::cout << "+" << std::endl;

    for(int i = 0; i < row - 1; ++i)
    {
        for(int j = 0; j < col - 1; ++j)
        {
            std::cout << "|";
            switch (m_matrix[i][j])
            {
                case Mark::prime:
                std::cout << std::setw(5) << std::right << std::to_string(C0[i][j]) + "'";
                break;
                case Mark::star:
                std::cout << std::setw(5) << std::right << std::to_string(C0[i][j]) + "*";
                break;
                case Mark::non:
                std::cout << std::setw(5) << std::right << C0[i][j];
            }
        }
        std::cout << "|";
        if(row_mark[i] == Mark::plus)
        {
            std::cout << std::setw(5) << std::right << "+";
            std::cout << "|";
        }
        else
            std::cout << std::setw(6) << std::right << "|";
        std::cout << std::endl;

        for(int l = 0; l < col; ++l){
            std::cout << "+";
            for(int k = 0; k < 5; ++k)
                std::cout << "-";
        }
        std::cout << "+" << std::endl;
    }
}

void Hungarian::print_chain()
{
    std::cout << "Цепочка = ";

    for(int i = 0; i < chain.size() - 1; ++i)
    {
       std::cout <<"[" << chain[i].first + 1 << "][" << chain[i].second + 1 << "]";
       if(i%2 == 0)
       {
           std::cout << "'";
       }
       else
       {
           std::cout << "*";
       }

       std::cout << " --> ";
    }

    std::cout <<"[" << chain[chain.size() - 1].first + 1<< "][" << chain[chain.size() - 1].second + 1<< "]";

    if( (chain.size() - 1)%2 == 0)
    {
           std::cout << "'";
    }
    else
    {
           std::cout << "*";
    }

    std::cout << std::endl;
}

void Hungarian::print_mins_el(std::vector<int> mins)
{
    std::cout << "min{";
    for(int i = 0; i < mins.size() - 1; ++i)
    {
        std::cout << mins[i] << ", ";
    }

    std::cout << mins[mins.size() - 1]<< "}" << std::endl;
}

void Hungarian::get_solve()
{
    std::cout << "Решение:" << std::endl;
    print_x();
    std::cout << "Оптимальное решение = ";

    int x_opt = 0;
    for(int i = 0; i < row - 1; ++i)
        for(int j = 0; j < col - 1; ++j)
        {
            x_opt += C.A[i][j]*help_matrix[i][j];
        }
    
    std::cout << x_opt << std:: endl;
}