#include <iostream>
#include <string>


int main(){
    int number = 52;
    float floatNumber = 52.0f;
    std::string helloString = "Hello World";

    std::cout << sizeof(number) << " " << sizeof(floatNumber) << " " << sizeof(helloString);
}