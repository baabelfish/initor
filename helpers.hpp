#pragma once

#include "lib/picojson/picojson.h"
#include <vector>
#include <type_traits>

namespace initor {

namespace internal {

template<typename T>
T toStr(std::string in) {
    return in;
}

template<> long double toStr<long double>(std::string in) { return std::stold(in); }
template<> unsigned long long toStr<unsigned long long>(std::string in) { return std::stoull(in); }
template<> unsigned long toStr<unsigned long>(std::string in) { return std::stoul(in); }
template<> long long toStr<long long>(std::string in) { return std::stoll(in); }
template<> long toStr<long>(std::string in) { return std::stol(in); }
template<> int toStr<int>(std::string in) { return std::stoi(in); }
template<> unsigned toStr<unsigned>(std::string in) { return std::stol(in); }
template<> float toStr<float>(std::string in) { return static_cast<float>(std::stof(in)); }
template<> std::string toStr<std::string>(std::string in) { return in; }


// TODO: Remove -v-
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
// TODO: Remove -^-

template<typename C>
C toContainer(picojson::value in) {
    auto inc = in.get<picojson::array>();
    C c;
    for (auto& x : inc) {
        c.insert(c.end(), to<typename C::value_type>(x));
    }
    return c;
}

} // namespace internal
} // namespace initor
