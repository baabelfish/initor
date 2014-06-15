#pragma once

#include "lib/picojson/picojson.h"
#include <vector>
#include <type_traits>

namespace initor {

namespace internal {

template<typename T>
T to(picojson::value in) {
    return in.get<T>();
}

template<> long double to<long double>(picojson::value in) { return std::stold(in.to_str()); }
template<> unsigned long long to<unsigned long long>(picojson::value in) { return std::stoull(in.to_str()); }
template<> unsigned long to<unsigned long>(picojson::value in) { return std::stoul(in.to_str()); }
template<> long long to<long long>(picojson::value in) { return std::stoll(in.to_str()); }
template<> long to<long>(picojson::value in) { return std::stol(in.to_str()); }
template<> int to<int>(picojson::value in) { return std::stoi(in.to_str()); }
template<> unsigned to<unsigned>(picojson::value in) { return std::stol(in.to_str()); }
template<> float to<float>(picojson::value in) { return static_cast<float>(in.get<double>()); }

template<typename C>
C toContainer(picojson::value in) {
    auto inc = in.get<picojson::array>();
    C c;
    for (auto& x : inc) {
        c.insert(c.end(), to<typename C::value_type>(x));
    }
    return c;
}

// template<typename T>
// std::vector<T> toArray(picojson::value in) {
//     auto inc = in.get<picojson::array>();
//     std::vector<T> vec;
//     auto inserter = std::inserter(inc, inc.end());
//     for (auto& x : inc) { vec.push_back(to<T>(x)); }
//     return vec;
// }

} // namespace internal
} // namespace initor
