#include "structs.h"
using namespace std;

int main(){
    while(true){
        string input;
        cout << "Enter: ";
        getline(cin, input);
        
        if(toLower(input) == "quit") break;
        
        tokenizer.setString(input);
        tokenizer.start();
    }
    return 0;
}