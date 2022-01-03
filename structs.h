#ifndef STRUCTS_H
#define STRUCTS_H
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string.h>
#include <iomanip>

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
    int parse();                        //MANAGE ERROR HANDLING
};

/*DDL*/
void createDB(std::string tableName, std::vector<std::string> colNames, std::vector<std::string> colTypes, std::vector<std::string> constraints, std::vector<std::string> primaryKeys, std::vector<std::string> fkCols, std::vector<std::string> fkRefTable, std::vector<std::string> fkRTCols);
void dropDB(std::string tableName);
bool deleteTableMetadata(std::string tableName);

/*DML*/
void insert(std::string tableName, std::string query);
void update(std::string tableName, std::string update_list, std::string condition_list);
void deleteRow(std::string tableName, std::string colName, std::string key);

/*Query*/
void select(std::string tableName, std::string attr_list, std::string condition_list);
void describeTable(std::string tableName);

/*Errors*/
std::string errors[11] = {"No Error", "Missing right parenthesis!", "Missing left parenthesis!", "No such SQL Command!",
                        "Error in given SQL Command! Please rectify!", "No such table found!", "No schema available!",
                        "Please specify table name", "Too few arguments passed in the insert statement.", "Invalid argument for defined attribute type",
                        "FROM not found in given select statement!"};

std::string toLower(std::string);
void breakString(std::vector<std::string>& values, std::string line, char delim);

void removeParenthesis(std::string& s);
std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);

bool checkIsDigit(std::string s);
bool tableCheck(std::string tableName, std::string& line);
bool dataTypeChecker(std::string colType, std::string& key);

// int finder(std::vector<std::string>& updateColNames, std::string og_colName, int start = 0);

#include "structs.cpp"
#endif