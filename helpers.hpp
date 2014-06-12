#pragma once

#include <string>
#include <functional>
#include "lib/picojson/picojson.h"

namespace initor {

template<typename T, typename U, typename F>
inline std::function<void(T&, std::string)> to(U T::*member, F manipulator) {
    return [member, manipulator](T& t, std::string v) {
        t.*member = manipulator(v);
    };
}

template<typename T, typename U>
inline std::function<void(T&, std::string)> toString(U T::*member) {
    return to(member, [](std::string v) { return v; });
}

template<typename T, typename U>
inline std::function<void(T&, std::string)> toFloat(U T::*member) {
    return to(member, [](std::string v) { return std::stof(v); });
}

template<typename T, typename U>
inline std::function<void(T&, std::string)> toDouble(U T::*member) {
    return to(member, [](std::string v) { return std::stod(v); });
}

template<typename T, typename U>
inline std::function<void(T&, std::string)> toBool(U T::*member) {
    return to(member, [](std::string v) { return v == "true" || v == "0"; });
}

template<typename T, typename U>
inline std::function<void(T&, std::string)> toInteger(U T::*member) {
    return to(member, [](std::string v) { return std::stoi(v); });
}

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

} // namespace internal

} // namespace initor
