#include "structs.h"
using namespace std;

int main(){
    string input;
    cout << "Enter: ";
    getline(cin, input);

    tokenizer.setString(input);
    tokenizer.start();
    return 0;
}