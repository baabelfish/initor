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

    std::vector<Something> smt;

    void verySecret(int v) {
        very_secret = v;
    }

    int verySecret() {
        return very_secret;
    }

    int getVerySecret() {
        return very_secret;
    }

    void setVerySecret(int v) {
        very_secret = v;
    }

private:
    int very_secret;
};

int main() {
    using namespace initor;

    auto wat_parser = initor::Mapper<Test::Wat>::make_parser(
        "a", &Test::Wat::a
    );

    auto smt_parser = initor::Mapper<Something>::make_parser(
        "a", &Something::a,
        "b", &Something::b
    );

    auto test_parser = initor::Mapper<Test>::make_parser(
        "x", &Test::x,
        "y", &Test::y,
        "z", &Test::z,
        "very_secret", &Test::setVerySecret,
        "vec", &Test::vec,
        "list", &Test::lis,
        "set", &Test::set,
        "s", [](Test& t, picojson::value v) { t.s = { std::stoi(v.to_str()), 0 }; },
        "nested", Mapper<Test>::mapper(&Test::nested, wat_parser),
        "nestedList", Mapper<Test>::containerMapper(&Test::smt, smt_parser)
    );

    auto n = test_parser.init(parseJsonFile("test.json"));

    std::cout << "x:       " << n.x << std::endl;
    std::cout << "y:       " << n.y << std::endl;
    std::cout << "z:       " << n.z << std::endl;
    std::cout << "s.a:     " << n.s.a << std::endl;
    std::cout << "s.b:     " << n.s.b << std::endl;
    std::cout << "nested:  " << n.nested.a << std::endl;
    std::cout << "vsecret: " << n.getVerySecret() << std::endl;

    std::cout << "vec:     ";
    for (auto& x : n.vec) { std::cout << x << " "; }
    std::cout << std::endl;

    std::cout << "list:    ";
    for (auto& x : n.lis) { std::cout << x << " "; }
    std::cout << std::endl;

    std::cout << "set:     ";
    for (auto& x : n.set) { std::cout << x << " "; }
    std::cout << std::endl;

    std::cout << "smt:     ";
    for (auto& x : n.smt) { std::cout << "{" << x.a << ", " << x.b << "}, "; }
    std::cout << std::endl;

    return 0;
}
