#include "token_test.hpp"
#include "iostream"

// Define the instance of the variable that is declared in the histogram.hpp header
TokenValue yylval;

int main() {

    while(1) {
        TokenType type=(TokenType)yylex();

        if (type==None) {
            return 1;
        }

        if(type==INT_LITERALS){
            std::cout << "Number: " << yylval.numberValue << "\n";

        } else{
            std::cout << "Token type: " << type << "\n";
        }
    }
    return 0;
}
