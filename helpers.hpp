#pragma once

#include "lib/picojson/picojson.h"
#include <vector>

namespace initor {
namespace internal {

template<typename T>
T to(picojson::value in) {
    return in.get<T>();
}

template<>
float to<float>(picojson::value in) {
    return static_cast<float>(in.get<double>());
}

template<>
int to<int>(picojson::value in) {
    return std::stoi(in.to_str());
}

template<typename T>
std::vector<T> toArray(picojson::array in) {
    std::vector<T> vec;
    for (auto& x : in) { vec.push_back(to<T>(x)); }
    return vec;
}

} // namespace internal
} // namespace initor
