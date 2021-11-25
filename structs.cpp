#include "structs.h"

void Tokenizer::setString(string input){
    sqlQuery = input;
}

void Tokenizer::start(){

    int result = errorCheck();
    if(result){
        cout << "Error :" << errors[result] << endl; //////////////
        return;
    }

}

int Tokenizer::errorCheck(){

    int parenthesisCount = 0;

    for(int i=0; i<sqlQuery.length(); i++){
        if(sqlQuery[i] == '(') parenthesisCount++;
        else if(sqlQuery[i] == ')') parenthesisCount--;
    }

    if(parenthesisCount > 0) return 1;
    else if(parenthesisCount < 0) return 2;

    //MULTIPLE SQL COMMANDS
    int start = 0;
    int end = sqlQuery.find(";");
    while(end != string::npos){
        sqlCommands.push_back(sqlQuery.substr(start, end));
        start = end + 1;
        end = sqlQuery.find(";", start);
    }

    sqlCommands.push_back(sqlQuery.substr(start));

    return 0;
}