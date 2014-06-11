#include "mapper.hpp"

struct Something {
    int a;
    int b;
};

struct Test {
    int x;
    int y;
    std::string z;
    Something s;

    struct Wat {
        float a;
    } nested;
};

int main() {
    using namespace initor;

    auto wat_parser = initor::Mapper<Test::Wat>::make_parser(
        "a", toFloat(&Test::Wat::a)
    );

    auto test_parser = initor::Mapper<Test>::make_parser(
        "x", toInteger(&Test::x),
        "y", toInteger(&Test::y),
        "z", toString(&Test::z),
        "s", [](Test& t, std::string v) { t.s = { std::stoi(v) * 2, std::stoi(v) / 2 }; },
        "nested", Mapper<Test>::useMapper(&Test::nested, wat_parser)
    );

    auto n = test_parser.init(parseJsonFile("test.json"));

    std::cout << "x:      " << n.x << std::endl;
    std::cout << "y:      " << n.y << std::endl;
    std::cout << "z:      " << n.z << std::endl;
    std::cout << "s.a:    " << n.s.a << std::endl;
    std::cout << "s.b:    " << n.s.b << std::endl;
    std::cout << "nested: " << n.nested.a << std::endl;

    return 0;
}
