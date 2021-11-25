#include "structs.h"

int main(){
    string input;
    cin >> input;
    
    Tokenizer tokenizer;                    //Global Tokenizer for input handling

    tokenizer.setString(input);
    tokenizer.start();
    return 0;
}