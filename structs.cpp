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

void Parser::parse(){
    for(int i=0; i<sqlCommands.size(); i++){
        sqlQuery = sqlCommands[i];
        
        vector<string> temp;
        stringstream ss(sqlQuery);
        string word;
        while (ss >> word) {
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            temp.push_back(word);
        }

        if(temp[0] == "create" && temp[1] == "table"){
            vector<string> colNames, colTypes;

            for(int i=3; i<temp.size(); i++){
                if(i==3){
                    colNames.push_back(temp[i].substr(1));
                }
                else if(i == temp.size() - 1 || i % 2 == 0){
                    colTypes.push_back(temp[i].substr(0, temp[i].size()-1));
                }
                else{
                    colNames.push_back(temp[i]);
                }
            }

            createDB(temp[2], colNames, colTypes);
        }
        else if(temp[0] == "drop"){
            cout << "drop" << endl;
        }
        else
            cout << "Error: " << errors[3] << " \"" << temp[0] << " " << temp[1] << "\"" << endl;
    }
}

void createDB(std::string tableName, std::vector<std::string> colNames, std::vector<std::string> colTypes){
    file.open("db/schema.txt", ios::app);
    ofstream csvFile(tableName + " .csv");

    file << tableName << "#";
    csvFile << tableName << ",";
    for(int i=0; i<colNames.size(); i++){
        file << colNames[i] << "#" << colTypes[i];                  //Handle Char(12) len and stuff
        csvFile << colNames[i] << "," << colTypes[i];
    }
    file << endl;
    csvFile << endl;

    file.close();
    csvFile.close();
}