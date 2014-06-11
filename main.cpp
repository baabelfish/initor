#include "mapper.hpp"

struct Test {
    int x;
    int y;
    std::string z;

    struct Wat {
        float a;
    } nested;
};

int main() {
    auto wat_parser = initor::Mapper<Test::Wat>::make_parser(
        "a", initor::toFloat(&Test::Wat::a)
    );

    auto test_parser = initor::Mapper<Test>::make_parser(
        "x", initor::toInteger(&Test::x),
        "y", initor::toInteger(&Test::y),
        "z", initor::toString(&Test::z),
        "nested", initor::Mapper<Test>::useMapper(&Test::nested, wat_parser)
    );

    auto n = test_parser.init(initor::parseJsonFile("test.json"));

    std::cout << "x:      " << n.x << std::endl;
    std::cout << "y:      " << n.y << std::endl;
    std::cout << "z:      " << n.z << std::endl;
    std::cout << "nested: " << n.nested.a << std::endl;

    return 0;
}
