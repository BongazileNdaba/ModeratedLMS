// Fictional sample submission E, version 2 (the intended current version).
#include <iostream>
#include <string>

int main() {
    // Improved attempt: asks for a name and prints a greeting.
    std::string name;
    std::getline(std::cin, name);
    std::cout << "Hello, " << name << '!' << '\n';
    return 0;
}
