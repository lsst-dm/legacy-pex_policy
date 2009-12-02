struct Foo {
    void thing() {}
};

struct Bar : public Foo {
    void thing(bool b) {}
};

struct Baz : public Foo {};

int main(int argc, char** argv) {
    // Bar b;    // Why does this not compile ...
    Baz b;       // ... and this compile?
    b.thing();
}
