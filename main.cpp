#include "mapper.hpp"

#include <list>

struct Something {
    int a;
    int b;
};

struct Test {
    int x;
    int y;
    std::vector<int> vec;
    std::list<float> lis;
    std::set<float> set;
    std::string z;
    Something s;

    struct Wat {
        float a;
    } nested;
};

int main() {
    using namespace initor;

    auto wat_parser = initor::Mapper<Test::Wat>::make_parser(
        "a", &Test::Wat::a
    );

    auto test_parser = initor::Mapper<Test>::make_parser(
        "x", &Test::x,
        "y", &Test::y,
        "z", &Test::z,
        "vec", &Test::vec,
        "list", &Test::lis,
        "set", &Test::set,
        "s", [](Test& t, picojson::value v) { t.s = { std::stoi(v.to_str()), 0 }; },
        "nested", Mapper<Test>::useMapper(&Test::nested, wat_parser)
    );

    auto n = test_parser.init(parseJsonFile("test.json"));

    std::cout << "x:      " << n.x << std::endl;
    std::cout << "y:      " << n.y << std::endl;
    std::cout << "z:      " << n.z << std::endl;
    std::cout << "s.a:    " << n.s.a << std::endl;
    std::cout << "s.b:    " << n.s.b << std::endl;
    std::cout << "nested: " << n.nested.a << std::endl;

    std::cout << "vec:    ";
    for (auto& x : n.vec) { std::cout << x << " "; }
    std::cout << std::endl;

    std::cout << "list:   ";
    for (auto& x : n.lis) { std::cout << x << " "; }
    std::cout << std::endl;

    std::cout << "set:    ";
    for (auto& x : n.set) { std::cout << x << " "; }
    std::cout << std::endl;

    return 0;
}
