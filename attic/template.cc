#include <iostream>

class Foo {
public:
    template <class T> void print(const T& t);
};

template <> void Foo::print(const int& value) {
    std::cout << "It's an int: " << value << std::endl;
}

template <> void Foo::print(const float& value) {
    std::cout << "It's a float: " << value << std::endl;
}

int main() {
    Foo foo = Foo();
    int i = 2;
    float f = 3.5f;
    foo.print(i);
    foo.print(f);
}
