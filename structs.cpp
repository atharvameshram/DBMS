#include "structs.h"
using namespace std;

Tokenizer tokenizer;                    //Global Tokenizer for input handling
Parser parser;                          //Global Parser object to parse query

string toLower(string s){
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

bool checkIsDigit(string s){
    for(int i=0; i<s.size(); i++){
        if(!isdigit(s[i])) return false;
    }
    return true;
}

std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(" \n\r\t\f\v");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
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
    std::string line;

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

    if(found)
    {
        cout << "Error: Table name already exists!" << endl;
        return;                                                                 //Add error handling
    }

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
    file.open("schema.txt");

    if(!file)
    {
        cout << "Error: " << errors[6] << endl;
    }
    else
    {
        deleteTableMetadata(tableName);                 //Take result value for error checking
        remove((tableName + ".csv").c_str());           //Put error handling
        
        //THROW SUCCESS MESSAGE
        cout << "Table dropped successfully." << endl;
    }
}

void deleteTableMetadata(std::string tableName){        //make return type int
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
        remove("text.txt");                                                     //return
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
        return;                                                                 //Add error handling
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
    //query = (123, 'ABC', 32)

    file.open(tableName+".csv", ios::app);

    vector<string> values;
    query.erase(0,1);
    query.erase(query.size()-1, 1);

    stringstream ss(query);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        values.push_back(substr);       // 123 'ABC' 32
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

        file << buffer << endl;     //123,ABC,32
        cout << "Tuple inserted successfully." << endl;
    }
}

void deleteRow(std::string tableName, std::string colName, std::string key){
    if(key.empty() || colName.empty()){
        cout << "Error: " << errors[4] << endl;
        return;
    }

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

    file.close();

    if(!found)
    {
        cout << "Error: " << errors[5] << " \"" << tableName << "\"" << endl;
        return;                                                                 //Add error handling
    }
    else if(line.find(colName) == string::npos){
        cout << "Error: No such column name exists in table!" << endl;
        return;
    }

    file.open(tableName+".csv");

    vector<string> values;          // Name Mob I

    ofstream temp("temp.csv");

    getline(file, line); 
    temp << line << endl;
    stringstream ss(line);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        values.push_back(substr);
    }

    int i = 0;
    int counter = 0;
    while(values[i] != colName) i++;

    while (getline(file, line))
    {
        values.clear();
        ss.clear();
        ss.str(line);
        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            values.push_back(substr);
        }

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

    if(!found)
    {
        cout << "Error: " << errors[5] << " \"" << tableName << "\"" << endl;
        return;                                                                 //Add error handling
    }
    
    //col = val, col2 = val2
    vector<string> update_KV_pair;
    stringstream ss(update_list);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        update_KV_pair.push_back(substr);
    }

    vector<string> update_colNames;
    vector<string> update_values;
    for(int i=0; i<update_KV_pair.size(); i++){
        ss.clear();
        ss.str(update_KV_pair[i]);
    
        for(int i=0; i<2; i++){
            string substr;
            getline(ss, substr, '=');
            substr = rtrim(substr);
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
        substr = rtrim(substr);
        if(i == 0) condn_colName = substr;
        else condn_value = substr;
    }

    if(condn_value.empty() || condn_colName.empty()){
        cout << "Error: " << errors[4] << endl;
        return;
    }


    file.open(tableName+".csv");

    vector<string> row_values;          // Name Mob I
    vector<string> og_colNames;
    ofstream temp("temp.csv");


    getline(file, line); 
    temp << line << endl;
    ss.clear();
    ss.str(line);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        substr = rtrim(substr);
        row_values.push_back(substr);
        og_colNames.push_back(substr);
    }
    
    int i = 0;
    int counter = 0;
    while(row_values[i] != condn_colName) i++;
    
    while (getline(file, line))
    {                           //id name mob
        row_values.clear();     //1 abc 9876        set name = bmm where id = 1
        ss.clear();
        ss.str(line);
        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            row_values.push_back(substr);
        }
        
        if(row_values[i] != condn_value){
            temp << line << endl;
        }
        else{
            for(int x=0; x<og_colNames.size(); x++){
                if(find(update_colNames.begin(), update_colNames.end(), og_colNames[x]) != update_colNames.end())
                    temp << update_values[x] << ",";
                else
                    temp << row_values[x] << ",";
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