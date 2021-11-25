#include "structs.h"
using namespace std;

Tokenizer tokenizer;                    //Global Tokenizer for input handling
Parser parser;

void Tokenizer::setString(std::string input){
    sqlQuery = input;
};

void Tokenizer::start(){
    int result = errorCheck();

    if(result){
        std::cout << "Error: " << errors[result] << std::endl; //////////////
        return;
    }

    parser.parse();
}

int Tokenizer::errorCheck(){

    int parenthesisCount = 0;

    for(int i=0; i<sqlQuery.length(); i++){
        if(sqlQuery[i] == '(') parenthesisCount++;
        else if(sqlQuery[i] == ')') parenthesisCount--;
    }

    if(parenthesisCount > 0) return 1;
    else if(parenthesisCount < 0) return 2;

    //Multiple SQL Commands
    int start = 0;
    int end = sqlQuery.find(";");
    while(end != std::string::npos){
        sqlCommands.push_back(sqlQuery.substr(start, end));
        start = end + 1;
        end = sqlQuery.find(";", start);
    }

    sqlCommands.push_back(sqlQuery.substr(start));

    return 0;
}

void Parser::setString(std::string input){
    sqlQuery = input;
};

void Parser::parse(){

}