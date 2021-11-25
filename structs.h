#ifndef STRUCTS_H
#define STRUCTS_H
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>

std::fstream file;                                  //Global file handler for I/O
std::vector<std::string> sqlCommands;               //Set of SQL Commands provided by user

class Tokenizer{                                    //Tokenizer to convert entire string sql command to individual sql commands --> sqlCommands
private:
    std::string sqlQuery;            

public:
    int errorCheck();
    void setString(std::string);
    void start();
    void tokenize();
};

class Parser{                                       //Parser to convert individual sql command to necessary data formats and call execute
private:
    std::string sqlQuery;

public:
    void parse();
};

/*DDL*/
void createDB(std::string tableName, std::vector<std::string> colNames, std::vector<std::string> colTypes);
void dropDB(std::string tableName);

/*DML*/
void insert(std::string tableName, std::vector<std::string> colNames, std::vector<std::string> rowValues);
void update(std::string tableName, std::vector<std::string> colNames, std::vector<std::string> rowValues);
void deleteRow(std::string tableName, std::vector<std::string> colNames, std::string key);

/*Query*/
void select(std::string tableName, std::vector<std::string> colNames);

/*Errors*/
std::string errors[4] = {"No Error", "Missing right parenthesis!", "Missing left parenthesis!", "No such SQL Command!"};

#include "structs.cpp"
#endif