#include "hungarian.h"
#include <iomanip>
#include <boost/algorithm/string.hpp>

Hungarian::Hungarian(){
    m_rows = 0;
    m_columns = 0;
    m_matrix = nullptr;
}

Hungarian::Hungarian(std::string &f_path)
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

void Hungarian::parse_file(std::ifstream &file)
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

    m_matrix = new int*[lines.size()];
    m_rows = lines.size();
    m_columns = -1;

    for(int i = 0; i < m_rows; ++i)
    {
        try
        {
            std::vector<int> *array = parse_line(lines[i]);

            if(m_columns == -1)
                m_columns = array->size();
            else if(m_columns != array->size()){
                    delete array;
                    throw std::string("Неправельный формат входных данных\n");
            }
            m_matrix[i] = new int[m_columns];

            for(int j = 0; j < m_columns; ++j)
                m_matrix[i][j] = (*array)[j];

            delete array;
        }
        catch(std::string str)
        {
            throw str;
        }
    }
}

std::vector<int> *Hungarian::parse_line(std::string &line)
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

void Hungarian::print()
{
    for(int i = 0; i < m_rows; ++i)
    {
        for(int j = 0; j < m_columns; ++j)
            std::cout <<std::setw(5)<<m_matrix[i][j];
        
        std::cout<<std::endl;
    }
}
