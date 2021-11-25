#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

fstream file;                           //Global file handler for I/O
vector<string> sqlCommands;             //Set of SQL Commands provided by user

class Tokenizer{                        //Tokenizer to convert entire string sql command to individual sql commands --> sqlCommands
private:
    string sqlQuery;            

public:
    int errorCheck();
    void setString(string);
    void start();
    void tokenize();
    void throwError();
};

class Parser{                           //Parser to convert individual sql command to necessary data formats and call execute
    string input;

    void parse();
    void throwError();
};

void execute();                         //Execute takes Command name ("CREATE", "SELECT"), and call necessary functions to execute the commands

/*DDL*/
void createDB(string tableName, vector<string> colNames, vector<string> colTypes);
void dropDB(string tableName);

/*DML*/
void insert(string tableName, vector<string> colNames, vector<string> rowValues);
void update(string tableName, vector<string> colNames, vector<string> rowValues);
void deleteRow(string tableName, vector<string> colNames, string key);

/*Query*/
void select(string tableName, vector<string> colNames);

/*Errors*/
string errors[3] = {"No Error", "Missing right parenthesis!", "Missing left parenthesis!"};