#include "structs.h"
using namespace std;

Tokenizer tokenizer;                    //Global Tokenizer for input handling
Parser parser;

string toLower(string s){
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void Tokenizer::setString(std::string input){
    sqlQuery = input;
};

void Tokenizer::start(){
    int result = errorCheck();

    if(result){
        std::cout << "Error: " << errors[result] << std::endl; //////////////
        return;
    }

    if(parser.parse()){
        //Perform Error handling
    }

    sqlCommands.clear();
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

int Parser::parse(){
    for(int i=0; i<sqlCommands.size(); i++){
        sqlQuery = sqlCommands[i];

        // Include Preprocessing of '()' to space

        vector<string> temp;
        stringstream ss(sqlQuery);
        string word;
        while (ss >> word) {
            temp.push_back(word);
        }

        if(toLower(temp[0]) == "describe")
        {
            if(temp[1].empty()){
                cout << "Error: " << errors[7] << " \"" << sqlQuery << "\"" << endl;
                return 1;
            }
            describeTable(temp[1]);
        }
        else if(toLower(temp[0]) == "create" && toLower(temp[1]) == "table")
        {
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

            if(colNames.size() != colTypes.size()){
                cout << "Error: " << errors[4] << " \"" << sqlQuery << "\"" << endl;
            }
            else
                createDB(temp[2], colNames, colTypes);

        }
        else if(toLower(temp[0]) == "drop" && toLower(temp[1]) == "table")
        {
            dropDB(temp[2]);
        }
        else if(toLower(temp[0]) == "insert" && toLower(temp[1]) == "into")
        {
            if(temp[3] != "values"){
                cout << "Error: " << errors[4] << " \"" << sqlQuery << "\" " << endl;
                return 1;
            }
            else
            {
                string values;
                for(int i=4; i<temp.size(); i++)
                    values += temp[i] + " ";
                insert(temp[2], values);
            }
        }
        else{
            cout << "Error: " << errors[3] << " \"" << sqlQuery << "\" " << endl;
            return 1;               //ISSUE Error.... Include Flag triggering
        }
    }

    return 0;
}

void describeTable(std::string tableName){
    string line;
    file.open("schema.txt");

    bool found = false;
    
    while (getline(file, line))
    {
        if (line.substr(0, line.find("#")) == tableName)
        {
            found = true;
            break;
        }
    }
    //TableName#ATTR1#TYPE1#ATTR2#TYPE2

    file.close();

    if(!found)
    {
        cout << "Error: " << errors[5] << " \"" << tableName << "\"" << endl;
    }
    else
    {
        int i = -1;
        stringstream ss(line);
    
        while (ss.good()) {
            string substr;
            getline(ss, substr, '#');
            
            if(i == -1){
                i = 0;
                continue;
            }
            else if(i == 0){
                cout << substr << " -- ";
                i++;
            }
            else{
                cout << substr << endl;
                i--;
            }
        }
        cout << endl;
    }
}

void createDB(std::string tableName, std::vector<std::string> colNames, std::vector<std::string> colTypes){
    file.open("schema.txt", ios::app);
    ofstream csvFile(tableName + ".csv");
    
    file << tableName << "#";
    for(int i=0; i<colNames.size(); i++){
        file << colNames[i] << "#" << colTypes[i] << ((i == colNames.size() - 1) ? "" : "#");           //Handle Char(12) len and stuff
        csvFile << colNames[i] << ((i == colNames.size() - 1) ? "" : ",");
    }
    file << endl;
    csvFile << endl;

    //THROW SUCCESS MESSAGE
    cout << "Database Created." << endl;

    file.close();
    csvFile.close();
}

void dropDB(std::string tableName){
    ifstream FILE("schema.txt");

    if(!FILE)
    {
        cout << "Error: " << errors[6] << endl;
    }
    else
    {
        FILE.close();
        
        deleteTableMetadata(tableName);                 //Take result value for error checking
        remove((tableName + ".csv").c_str());           //Put error handling
        
        //THROW SUCCESS MESSAGE
        cout << "Table dropped successfully." << endl;
    }
}

void deleteTableMetadata(std::string tableName){        //make return type int
    string line;
    ofstream temp("temp.txt");
    file.open("schema.txt");

    bool found = false;                     //Check if table name exists in the schema.txt file

    while (getline(file, line))
    {
        if (line.substr(0, line.find("#")) != tableName)
            temp << line << endl;
        else
            found = true;
    }

    temp.close();
    file.close();

    if(!found)
    {
        cout << "Error: " << errors[5] << " \"" << tableName << "\"" << endl;
        remove("text.txt");             //return
    }
    else
    {
        remove("schema.txt");                           //INCLUDE ERROR HANDLING
        rename("temp.txt", "schema.txt");
    }
}

void insert(std::string tableName, std::string query){
    std::vector<std::string> colTypes;
    std::string line;
    file.open("schema.txt");

    bool found = false;

    while (getline(file, line))
    {
        if (line.substr(0, line.find("#")) == tableName)
        {
            found = true;
            break;
        }
    }
    //TableName#ATTR1#TYPE1#ATTR2#TYPE2

    file.close();

    if(!found)
    {
        cout << "Error: " << errors[5] << " \"" << tableName << "\"" << endl;
    }
    else
    {
        vector<string> v;
        stringstream ss(line);
    
        while (ss.good()) {
            string substr;
            getline(ss, substr, '#');
            v.push_back(substr);
        }

        for(int i=2; i<v.size(); i+=2)
            colTypes.push_back(v[i]);
    }

    //coltypes = int char int
    //query = (123 'ABC' 32)

    file.open(tableName+".csv", ios::app);

    vector<string> values;
    query.erase(0,1);
    query.erase(query.size()-1, 1);

    stringstream ss(query);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        values.push_back(substr);
    }

    if(values.size() != colTypes.size()){
        cout << "Error: " << errors[8] << " \"" << query << "\"" << endl;
    }
    else{
        string buffer;

        for(int i=0; i<values.size(); i++){
            if(toLower(colTypes[i]) == "int"){
                if(checkIsDigit(values[i]))
                {
                    buffer += values[i] + ",";
                }
                else{
                    cout << "Error: " << errors[9] << " \"" << values[i] << "\"" << endl;
                    return;
                }
            }
            if(toLower(colTypes[i]) == "char"){
                if(!checkIsDigit(values[i])){
                    values[i].erase(0,1);
                    values[i].erase(values[i].size()-1,1);
                    buffer += values[i] + ",";
                }
                else{
                    cout << "Error: " << errors[9] << " \"" << values[i] << "\"" << endl;
                    return;
                }
            }
        }

        file << buffer << endl;
        cout << "Tuple inserted successfully." << endl;
    }
}

bool checkIsDigit(string s){
    for(int i=0; i<s.size(); i++){
        if(!isdigit(s[i])) return false;
    }
    return true;
}