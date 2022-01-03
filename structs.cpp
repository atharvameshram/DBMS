#include "structs.h"
using namespace std;

Tokenizer tokenizer;                    //Global Tokenizer for input handling
Parser parser;                          //Global Parser object to parse query

string toLower(string s){
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void breakString(std::vector<std::string>& values, std::string line, char delim){
    stringstream ss(line);

    while (ss.good()) {
        string substr;
        getline(ss, substr, delim);
        substr = rtrim(ltrim(substr));
        values.push_back(substr);
    }
}

bool checkIsDigit(string s){
    for(int i=0; i<s.size(); i++){
        if(!isdigit(s[i])) return false;
    }
    return true;
}

bool tableCheck(std::string tableName, std::string& line){
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
    file.close();

    return found;
}

bool dataTypeChecker(string colType, string& key){
    if(toLower(colType) == "int"){
        if(!checkIsDigit(key)){            
            return false;
        }
    }
    else if(toLower(colType) == "char"){
        if(!checkIsDigit(key)){
            key.erase(0,1);
            key.erase(key.size()-1,1);
        }
        else{
            return false;
        }
    }

    return true;
}

void removeParenthesis(std::string& s){
    int found = s.find('(');
    if(found != string::npos){
        s.erase(s.begin() + found);
    }
    
    found = s.rfind(')');
    if(found != string::npos){
        s.erase(s.begin() + found);
    }
}

std::string ltrim(const std::string &s){
    size_t start = s.find_first_not_of(" \n\r\t\f\v");
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s){
    size_t end = s.find_last_not_of(" \n\r\t\f\v");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

int finder(vector<string>& updateColNames, string og_colName, int start = 0){
    for(int i=start; i<updateColNames.size(); i++){
        if(updateColNames[i] == og_colName){ return i; }
    }

    return -1;
}

void Tokenizer::setString(std::string input){
    sqlQuery = input;
};

void Tokenizer::start(){
    int result = errorCheck();

    if(result){
        std::cout << "Error: " << errors[result] << std::endl;          ////
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

        if(temp.size() > 1 && toLower(temp[0]) == "create" && toLower(temp[1]) == "table")
        {
            vector<string> colNames, colTypes, constraints, primaryKeys, fkCols, fkRefTbl, fkRTCols;
            bool breakFlag = false;

            int pKeyLoc = finder(temp, "primary");
            int fKeyLoc = finder(temp, "foreign");

            if(pKeyLoc == -1){
                for(int i=3; i<temp.size(); i++){
                    if(i==3){
                        colNames.push_back(temp[i].substr(1));
                    }
                    else if(i == temp.size() - 1 || i % 2 == 0){
                        std::string colType = temp[i].substr(0, temp[i].size()-1);
                        if(!(colType == "int" || colType == "char")){
                            cout << "Error: " << errors[4] << endl;
                            breakFlag = true;
                            break;
                        }
                        colTypes.push_back(colType);
                    }
                    else{
                        colNames.push_back(temp[i]);
                    }
                }
            }
            else{
                for(int i=3; i<pKeyLoc; i++){
                    if(i==3){
                        colNames.push_back(temp[i].substr(1));
                    }
                    else if(i == pKeyLoc - 1 || i % 2 == 0){
                        std::string colType = temp[i].substr(0, temp[i].size()-1);
                        if(!(colType == "int" || colType == "char")){
                            cout << "Error: " << errors[4] << endl;
                            breakFlag = true;
                            break;
                        }
                        colTypes.push_back(colType);
                    }
                    else{
                        colNames.push_back(temp[i]);
                    }
                }

                for(int i=pKeyLoc+2; i< (fKeyLoc == -1 ? temp.size() : fKeyLoc); i++){
                    removeParenthesis(temp[i]);
                    primaryKeys.push_back(temp[i]);
                }

                while(fKeyLoc != -1){
                    removeParenthesis(temp[fKeyLoc+2]);
                    fkCols.push_back(temp[fKeyLoc+2]);
                    removeParenthesis(temp[fKeyLoc+4]);
                    fkRefTbl.push_back(temp[fKeyLoc+4]);
                    removeParenthesis(temp[fKeyLoc+5]);
                    fkRTCols.push_back(temp[fKeyLoc+5]);
                    fKeyLoc = finder(temp, "foreign", fKeyLoc+5);
                }
            }

            if(breakFlag) continue;

            if(colNames.size() != colTypes.size()){
                cout << "Error: " << errors[4] << " \"" << sqlQuery << "\"" << endl;
            }
            else
                createDB(temp[2], colNames, colTypes, constraints, primaryKeys, fkCols, fkRefTbl, fkRTCols);

        }
        else if(temp.size() > 1 && toLower(temp[0]) == "describe")
        {
            if(temp[1].empty()){
                cout << "Error: " << errors[7] << " \"" << sqlQuery << "\"" << endl;
                return 1;
            }
            describeTable(temp[1]);
        }
        else if(temp.size() > 1 && toLower(temp[0]) == "drop" && toLower(temp[1]) == "table")
        {
            dropDB(temp[2]);
        }
        else if (temp.size() > 1 && toLower(temp[0]) == "help")
        {
            if(toLower(temp[1]) == "tables"){
                file.open("schema.txt");
                string line;
                bool empty = true;

                if(!file){
                    cout << "Error: " << errors[6] << endl;
                }
                else
                {
                    while(getline(file, line)){
                        cout << line.substr(0, line.find("#")) << endl;
                        empty = false;
                    }

                    if(empty) cout << "No tables found." << endl;
                }
                file.close();
            }
            else
            {
                if(temp.size() > 2 && toLower(temp[1]) == "create" && toLower(temp[2]) == "table")
                {
                    cout << "\"CREATE TABLE\" command is part of DDL commands, with which user can create databases by providing Table Name, its attributes, their respective attribute types and \noptional attribute constraints.\nExpected Format: \"CREATE TABLE tablename (attr1 attr1_type attr1_constr, attr2 attr2_type attr2_constr, .....);\"\n" << endl;
                }
                else if(temp.size() > 2 && toLower(temp[1]) == "drop" && toLower(temp[2]) == "table")
                {
                    cout << "\"DROP TABLE\" command is part of DDL commands, with which user can drop or delete databases by simply providing its Table Name.\nExpected Format: \"DROP TABLE tablename;\"\n" << endl;
                }
                else if(toLower(temp[1]) == "insert")
                {
                    cout << "\"INSERT\" command is part of DML commands, with which user can insert new row values in existing databases by providing its Table Name, and a tuple of values to be \ninserted into the table with corresponding data types.\nExpected Format: \"INSERT INTO tablename VALUES (value1, value2, .....);\"\n" << endl;
                }
                else if(toLower(temp[1]) == "update")
                {
                    cout << "\"UPDATE\" command is part of DML commands, with which user can update existing row values in a database by providing its Table Name, column name and corresponding value,\nwith a necessary condition to identify the row(s) that need to be updated.\nExpected Format: \"UPDATE tablename SET column1 = value1, column2 = value2... WHERE condition_list;\"\n" << endl;
                }
                else if(toLower(temp[1]) == "delete")
                {
                    cout << "\"DELETE\" command is part of DML commands, with which user can delete row values in existing databases by providing its Table Name, condition to indentify the row(s).\nExpected Format: \"DELETE FROM tablename WHERE condition_list;\"\n" << endl;
                }
                else if(toLower(temp[1]) == "describe")
                {
                    cout << "\"DESCRIBE\" command is used to query existing database schema by providing its Table Name.\nExpected Format: \"DESCRIBE tablename;\"\n" << endl;
                }
                else if(toLower(temp[1]) == "select")
                {
                    cout << "\"SELECT\" command is used to query row(s) from existing database by providing its Table Name, with optional attribute names to select certain attributes, and an optional\ncondition to identify row(s).\nExpected Format: \"SELECT attr_list FROM table_name WHERE condition_list;\"\n" << endl;
                }
                else if(toLower(temp[1]) == "help")
                {
                    cout << "\"HELP\" command can be used to \n(a) Get list of tables/databases and their schema, with Expected Format: \"HELP TABLES;\"\n(b) Get info on commands and their use, with Expected Format: \"HELP command_name;\"\n" << endl;
                }
                else if(toLower(temp[1]) == "quit")
                {
                    cout << "\"QUIT\" command is used to quit the application with an Expected Format: \"QUIT;\"\n" << endl;
                }
                else{
                    cout << "Error: " << errors[4] << " \"" << sqlQuery << "\"" << endl;
                    return 1;
                }
            }
        }
        else if(temp.size() > 2 && toLower(temp[0]) == "insert" && toLower(temp[1]) == "into")
        {
            if(temp[3] != "values"){
                cout << "Error: " << errors[4] << " \"" << sqlQuery << "\" " << endl;
                return 1;
            }
            else
            {
                std::string values;
                for(int i=4; i<temp.size(); i++)
                    values += temp[i] + " ";
                
                insert(temp[2], values);
            }
        }
        else if(temp.size() > 2 && toLower(temp[0]) == "delete" && toLower(temp[1]) == "from")
        {
            if(temp[3] != "where"){
                cout << "Error: " << errors[4] << " \"" << sqlQuery << "\" " << endl;
                return 1;
            }
            else
            {
                //id = 2
                std::string colName = temp[4];
                std::string value = temp[6];
                deleteRow(temp[2], colName, value);
            }
        }
        else if(temp.size() > 2 && toLower(temp[0]) == "update" && toLower(temp[2]) == "set")
        {
            std::string update_list;
            std::string condition_list;
            int i = 3;
            while(toLower(temp[i]) != "where"){
                update_list += temp[i] + " ";
                i++;
            }

            for(i++; i<temp.size(); i++)
                condition_list += temp[i] + " ";

            update(temp[1], update_list, condition_list);   //TableName, 'col = value, col2 = val2....', 'id = 1.....'
        }
        else if(temp.size() > 3 && toLower(temp[0]) == "select")
        {
            int idx;
            if((idx = finder(temp, "from")) == -1){
                cout << "Error: " << errors[10] << " \"" << sqlQuery << "\" " << endl;
                return 1;    
            }
            
            string tableName = temp[idx+1];
            //////Error handling for table name (bad access probably)
            
            string attr_list;
            string condn_list;

            if(temp[1] != "*"){
                for(int i=1; i<idx; i++)
                    attr_list += temp[i] + " ";
            }
            else{
                if(idx != 2){
                    cout << "Error: " << errors[4] << " \"" << sqlQuery << "\" " << endl;
                    return 1;
                }
                attr_list = temp[1];
            }

            if((idx = finder(temp, "where")) != -1){
                for(int i=idx+1; i<temp.size(); i++)
                    condn_list += temp[i] + " ";
            }

            select(tableName, attr_list, condn_list);
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
    
    if(!tableCheck(tableName, line)){
        cout << "Error: " << errors[5] << " \"" << tableName << "\"" << endl;
        return;
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

void createDB(std::string tableName, std::vector<std::string> colNames, std::vector<std::string> colTypes, std::vector<std::string> constraints, std::vector<std::string> primaryKeys, std::vector<std::string> fkCols, std::vector<std::string> fkRefTable, std::vector<std::string> fkRTCols){
    std::string line;

    if(tableCheck(tableName, line))
    {
        cout << "Error: Table name already exists!" << endl;
        return;                                                                 //Add error handling
    }

    file.open("schema.txt", ios::app);  
    ofstream csvFile(tableName + ".csv");

    file << tableName << "#";

    // int idx = -1, j = 0, k = 0;
    // for(int i=0; i<colNames.size(); i++){
       
    // }

    for(int i=0; i<colNames.size(); i++){
        file << colNames[i] << "#" << colTypes[i] << ((i == colNames.size() - 1) ? "" : "#");           //Handle Char(12) len and stuff
        csvFile << colNames[i] << ((i == colNames.size() - 1) ? "" : ",");
    }

    file << endl;
    csvFile << endl;

    //THROW SUCCESS MESSAGE
    cout << "Table Created." << endl;

    file.close();
    csvFile.close();
}

void dropDB(std::string tableName){
    file.open("schema.txt");

    if(!file)
    {
        cout << "Error: " << errors[6] << endl;
    }
    else
    {
        if(!deleteTableMetadata(tableName)){            //Take result value for error checking
            return;
        }              
        
        remove((tableName + ".csv").c_str());           //Put error handling
        
        //THROW SUCCESS MESSAGE
        cout << "Table dropped successfully." << endl;
    }
}

bool deleteTableMetadata(std::string tableName){        //make return type int
    string line;
    ofstream temp("temp.txt");

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
        if(remove("text.txt") != 0)             //return
            cout << strerror(errno) << endl;
        return false;
    }
    else
    {
        remove("schema.txt");                           //INCLUDE ERROR HANDLING
        rename("temp.txt", "schema.txt");
        return true;
    }
}

void insert(std::string tableName, std::string query){
    std::vector<std::string> colTypes;
    std::string line;

    if(!tableCheck(tableName, line))
    {
        cout << "Error: " << errors[5] << " \"" << tableName << "\"" << endl;
        return;                                                                 //Add error handling
    }

    vector<string> v;
    
    /*stringstream ss(line);

    while (ss.good()) {
        string substr;
        getline(ss, substr, '#');
        v.push_back(substr);
    }
    */

    breakString(v, line, '#');

    for(int i=2; i<v.size(); i+=2)
        colTypes.push_back(v[i]);


    //coltypes = int char int
    //query = (123, 'ABC', 32)

    file.open(tableName+".csv", ios::app);

    /*
    if(!file){
        return;
    }
    */

    vector<string> values;

    rtrim(ltrim(query));
    removeParenthesis(query);

    /*ss.clear();
    ss.str(query);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        substr = rtrim(ltrim(substr));
        values.push_back(substr);       // 123 'ABC' 32
    }
    */

    breakString(values, query, ',');

    if(values.size() != colTypes.size()){
        cout << "Error: " << errors[8] << " \"" << query << "\"" << endl;
        file.close();
        return;
    }
    else{
        string buffer;

        for(int i=0; i<values.size(); i++){
            /*if(toLower(colTypes[i]) == "int"){
                if(!checkIsDigit(values[i])){            
                    cout << "Error: " << errors[9] << " \"" << values[i] << "\"" << endl;
                    return;
                }
            }
            if(toLower(colTypes[i]) == "char"){
                if(!checkIsDigit(values[i])){
                    values[i].erase(0,1);
                    values[i].erase(values[i].size()-1,1);
                }
                else{
                    cout << "Error: " << errors[9] << " \"" << values[i] << "\"" << endl;
                    return;
                }
            }*/
            if(!dataTypeChecker(colTypes[i], values[i])){
                cout << "Error: " << errors[9] << " \"" << values[i] << "\"" << endl;
                file.close();
                return;
            }
            buffer += values[i] + (i == values.size()-1 ? "" : ",");
        }

        file << buffer << endl;     //123,ABC,32
        cout << "Tuple inserted successfully." << endl;
    }
    file.close();
}

void deleteRow(std::string tableName, std::string colName, std::string key){
    if(key.empty() || colName.empty()){
        cout << "Error: " << errors[4] << endl;
        return;
    }

    std::string line;

    if(!tableCheck(tableName, line)){
        cout << "Error: " << errors[5] << " \"" << tableName << "\"" << endl;
        return;                                                                 //Add error handling    
    }
    
    int loc;
    if((loc = line.find(colName)) == string::npos){
        cout << "Error: No such column name exists in table!" << endl;
        return;
    }
    else{
        loc = line.find("#", loc);
        string colType = line.substr(loc+1, line.find('#', loc+1)-loc-1);       //where id = abc

        /*if(toLower(colType) == "int"){
            if(!checkIsDigit(key)){            
                cout << "Error: " << errors[9] << " " << colType << " for column \"" << colName << "\"" << endl;
                return;
            }
        }
        if(toLower(colType) == "char"){
            if(!checkIsDigit(key)){
                key.erase(0,1);
                key.erase(key.size()-1,1);
            }
            else{
                cout << "Error: " << errors[9] << " " << colType << " for column \"" << colName << "\"" << endl;
                return;
            }
        }*/

        if(!dataTypeChecker(colType, key)){
            cout << "Error: " << errors[9] << " " << colType << " for column \"" << colName << "\"" << endl;
            return;
        }
    }

    file.open(tableName+".csv");
    ofstream temp("temp.csv");

    vector<string> values;          // Name Mob I

    getline(file, line); 
    temp << line << endl;
    
    /*stringstream ss(line);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        substr = rtrim(ltrim(substr));
        values.push_back(substr);
    }
    */

    breakString(values, line, ',');

    int i = 0;
    int counter = 0;
    while(values[i] != colName) i++;

    while (getline(file, line))
    {
        values.clear();
        
        /*ss.clear();
        ss.str(line);
        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            substr = rtrim(ltrim(substr));
            values.push_back(substr);
        }
        */
        breakString(values, line, ',');

        if (values[i] != key)
            temp << line << endl;
        else counter++;
    }

    temp.close();
    file.close();

    remove((tableName+".csv").c_str());                           //INCLUDE ERROR HANDLING
    rename("temp.csv", (tableName+".csv").c_str());
    
    cout << counter << " row(s) deleted successfully!" << endl;
}

void update(std::string tableName, std::string update_list, std::string condition_list){
    // Function Table check can be added
    std::string line;

    if(!tableCheck(tableName, line)){
        cout << "Error: " << errors[5] << " \"" << tableName << "\"" << endl;
        return;                                                                 //Add error handling    
    }
    
    //col = val, col2 = val2
    vector<string> update_KV_pair;
    
    /*stringstream ss(update_list);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        substr = rtrim(ltrim(substr));
        update_KV_pair.push_back(substr);
    }
    */
    breakString(update_KV_pair, update_list, ',');

    stringstream ss;
    vector<string> update_colNames;
    vector<string> update_values;
    for(int i=0; i<update_KV_pair.size(); i++){
        ss.clear();
        ss.str(update_KV_pair[i]);
    
        for(int i=0; i<2; i++){
            string substr;
            getline(ss, substr, '=');
            substr = rtrim(ltrim(substr));
            if(i == 0) update_colNames.push_back(substr);
            else update_values.push_back(substr);
        }
    }

    if(update_colNames.size() != update_values.size()){
        cout << "Error: " << errors[4] << endl;
        return;
    }

    for(int i=0; i<update_colNames.size(); i++){
        if(line.find(update_colNames[i].substr(0, update_colNames[i].size()-1)) == string::npos){
            cout << "Error: " << update_colNames[i] << " not found in given table!" << endl;
            return;
        }
    }

    string condn_colName;
    string condn_value;
    ss.clear();
    ss.str(condition_list);     //id = 1
    for(int i=0; i<2; i++){
        string substr;
        getline(ss, substr, '=');
        substr = rtrim(ltrim(substr));
        if(i == 0) condn_colName = substr;
        else condn_value = substr;
    }

    if(condn_value.empty() || condn_colName.empty()){
        cout << "Error: " << errors[4] << endl;
        return;
    }


    file.open(tableName+".csv");
    vector<string> row_values;          // Name Mob I
    ofstream temp("temp.csv");

    getline(file, line); 
    temp << line << endl;
    
    /*ss.clear();
    ss.str(line);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        substr = rtrim(ltrim(substr));
        row_values.push_back(substr);
    }*/
    breakString(row_values, line, ',');

    vector<string> og_colNames(row_values);
    
    int i = 0;
    int counter = 0;
    while(row_values[i] != condn_colName) i++;
    
    while (getline(file, line))
    {                           //id name mob
        row_values.clear();     //1 abc 9876        set name = bmm where id = 1
        
        /*ss.clear();
        ss.str(line);
        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            substr = rtrim(ltrim(substr));
            row_values.push_back(substr);
        }*/
        breakString(row_values, line, ',');
        
        if(row_values[i] != condn_value){
            temp << line << endl;
        }
        else{
            //cout << "row value = " << row_values[i] << endl;
            for(int x=0; x<og_colNames.size(); x++){
                int idx;
                if( (idx = finder(update_colNames, og_colNames[x])) != -1){
                    // if(dataTypeChecker("char", update_values[idx])){
                    //     cout << "Name = " << update_values[idx]; 
                    //     // update_values[idx].erase(0, 1);
                    //     // update_values[idx].erase(update_values.size()-1, 1);
                    // }
                    temp << update_values[idx];
                }
                else
                    temp << row_values[x];
                temp << (x == og_colNames.size()-1 ? "" : ",");
            }
            temp << endl;
            counter++;
        }
    }

    temp.close();
    file.close();

    remove((tableName+".csv").c_str());                           //INCLUDE ERROR HANDLING
    rename("temp.csv", (tableName+".csv").c_str());
    
    cout << counter << " row(s) updated successfully!" << endl;
}

void select(std::string tableName, std::string attr_list, std::string condition_list){
    std::string line;
    if(!tableCheck(tableName, line)){
        cout << "Error: " << errors[5] << " \"" << tableName << "\"" << endl;
        return;                                                                 //Add error handling
    }

    vector<string> values;
    file.close();
    file.open(tableName + ".csv");
    
    if(attr_list == "*"){
        if(condition_list.empty()){
            while(getline(file, line)){
                values.clear();
                breakString(values, line, ',');

                for(auto i : values)
                    cout << std::setw(5) << i;
                cout << endl;
            }
        }
        else{
            string condn_colName;
            string condn_value;
            stringstream ss(condition_list);     //id = 1
            
            for(int i=0; i<2; i++){
                string substr;
                getline(ss, substr, '=');
                substr = rtrim(ltrim(substr));
                if(i == 0) condn_colName = substr;
                else condn_value = substr;
            }

            if(condn_value.empty() || condn_colName.empty()){
                cout << "Error: " << errors[4] << endl;
                return;
            }

            int loc;
            if((loc = line.find(condn_colName)) == string::npos){
                cout << "Error: No such column name exists in table!" << endl;
                return;
            }
            loc = line.find("#", loc);
            string colType = line.substr(loc+1, line.find('#', loc+1)-loc-1);       //where id = abc
        
            if(!dataTypeChecker(colType, condn_value)){
                cout << "Error: " << errors[9] << " " << colType << " for column \"" << condn_colName << "\"" << endl;
                return;
            }

            getline(file, line);
            breakString(values, line, ',');

            int idx = 0;
            while(values[idx] != condn_colName) idx++;

            for(auto i : values)
                cout << std::setw(5) << i;
            cout << endl;

            while(getline(file, line)){
                values.clear();
                breakString(values, line, ',');

                if(values[idx] == condn_value){
                    for(auto i : values)
                        cout << std::setw(5) << i;
                    cout << endl;
                }
            }
        }
    }                                               //attr_list - id, name
    else if(condition_list.empty()){
        vector<string> colNames;
        breakString(colNames, attr_list, ',');      //attr id mob (0, 2)
        
        getline(file, line);
        breakString(values, line, ',');             // og columns id name mob
        
        vector<int> index;
        for(int i=0; i<values.size(); i++){
            if(finder(colNames, values[i]) != -1){
                index.push_back(i);
            }
        }
        //sort(index.begin(), index.end());
        for(int i=0; i<index.size(); i++)
            cout << std::setw(5) << values[index[i]];
        cout << endl;

        while(getline(file, line)){
            values.clear();
            breakString(values, line, ',');
            
            for(int i=0; i<index.size(); i++)
                cout << std::setw(5) << values[index[i]];
            cout << endl;
        }
    }
    else{
        string condn_colName;
        string condn_value;
        stringstream ss(condition_list);     //id = 1
        
        for(int i=0; i<2; i++){
            string substr;
            getline(ss, substr, '=');
            substr = rtrim(ltrim(substr));
            if(i == 0) condn_colName = substr;
            else condn_value = substr;
        }

        if(condn_value.empty() || condn_colName.empty()){
            cout << "Error: " << errors[4] << endl;
            return;
        }

        int loc;
        if((loc = line.find(condn_colName)) == string::npos){
            cout << "Error: No such column name exists in table!" << endl;
            return;
        }
        loc = line.find("#", loc);
        string colType = line.substr(loc+1, line.find('#', loc+1)-loc-1);       //where id = abc
    
        if(!dataTypeChecker(colType, condn_value)){
            cout << "Error: " << errors[9] << " " << colType << " for column \"" << condn_colName << "\"" << endl;
            return;
        }
        
        vector<string> colNames;
        breakString(colNames, attr_list, ',');      //attr id mob (0, 2)
        
        getline(file, line);
        breakString(values, line, ',');             // og columns id name mob
        
        int idx = 0;
        while(values[idx] != condn_colName) idx++;
        
        vector<int> index;
        for(int i=0; i<values.size(); i++){
            if(finder(colNames, values[i]) != -1){
                index.push_back(i);
            }
        }
        //sort(index.begin(), index.end());
        for(int i=0; i<index.size(); i++)
            cout << std::setw(5) << values[index[i]];
        cout << endl;

        while(getline(file, line)){
            values.clear();
            breakString(values, line, ',');
            
            if(values[idx] == condn_value){
                for(int i=0; i<index.size(); i++)
                    cout << std::setw(5) << values[index[i]];
                cout << endl;
            }
        }

    }

    file.close();
}