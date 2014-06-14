#pragma once

#include <iostream>

#include <type_traits>
#include <memory>
#include <fstream>
#include <stdexcept>
#include <functional>
#include <unordered_map>
#include "helpers.hpp"
#include "lib/picojson/picojson.h"

namespace initor {

inline picojson::value parseJson(std::string str) {
    picojson::value v;
    std::string err;
    picojson::parse(v, str.c_str(), str.c_str() + strlen(str.c_str()), &err);
    if (err.empty()) { return v; }
    else { throw std::runtime_error(err); }
}

inline picojson::value parseJsonFile(std::string file) {
    std::ifstream is(file);
    if (is.is_open()) {
        std::string buffer;
        std::string temp;
        while (std::getline(is, temp)) { buffer += temp; }
        return parseJson(buffer);
    }
    is.close();
    throw std::runtime_error("Could not open file: " + file);
}

class BaseMapper {
public:
    BaseMapper() {}
    virtual ~BaseMapper() {}
};

template<class T>
class Mapper : public BaseMapper {
public:
    typedef std::string IdentifierType;
    typedef std::function<void(T&, std::string)> SetterType;
    typedef std::function<void(T&, picojson::value)> MapperType;

    template<typename... Args>
    static Mapper<T> make_parser(Args... args) {
        Mapper<T> p;
        return _make_parser(p, std::forward<Args>(args)...);
    }

    template<typename U, typename Mapper>
    static MapperType useMapper(U T::*member, Mapper pt) {
        return [=](T& t, picojson::value v) {
            t.*member = std::move(pt.init(v));
        };
    }

    Mapper(): BaseMapper() {}

    virtual ~Mapper() {}

    void addPair(IdentifierType id, SetterType st) {
        setters[id] = st;
    }

    void addPair(IdentifierType id, MapperType p) {
        parsers[id] = p;
    }

    template<typename Member,
             typename std::enable_if<std::is_member_pointer<Member>::value>::type* = nullptr>
    void addPair(IdentifierType id, Member p) {
        _addPair(id, p);
    }

    T init(picojson::value doc) const {
        T t;
        for (auto& x : parsers) {
            auto pit = parsers.find(x.first);
            if (pit != parsers.end()) {
                pit->second(t, access(doc, x.first));
            }
        }
        for (auto& x : setters) {
            try { x.second(t, access(doc, x.first).to_str()); }
            catch (std::runtime_error) {}
        }
        return t;
    }

private:
    std::unordered_map<IdentifierType, SetterType> setters;
    std::unordered_map<IdentifierType, MapperType> parsers;

    static Mapper<T> _make_parser(Mapper<T> p) { return p; }

    template<typename Second, typename... Args>
    static Mapper<T> _make_parser(Mapper<T> p, IdentifierType f, Second s, Args... args) {
        p.addPair(f, s);
        return _make_parser(p, std::forward<Args>(args)...);
    }

    template<typename U>
    void _addPair(IdentifierType id, U T::*member) {
        parsers[id] = [member](T& t, picojson::value v) {
            if (v.is<picojson::array>()) {
                t.*member = internal::to<U>(v);
            }
            else if (v.is<picojson::object>()) {
                t.*member = internal::to<U>(v);
            }
            else {
                t.*member = internal::to<U>(v);
            }
        };
    }

    picojson::value access(picojson::object obj, std::string key) const {
        auto it = obj.find(key);
        if (it == obj.end()) { throw std::runtime_error("Key was not found: " + key); }
        return it->second;
    }

    picojson::value access(picojson::value doc, std::string key) const {
        auto& obj = doc.get<picojson::object>();
        auto it = obj.find(key);
        if (it == obj.end()) { throw std::runtime_error("Key was not found: " + key); }
        return it->second;
    }
};

} // namespace initor
