#pragma once

#include "lib/cpputils/utils/function_traits.hpp"

#define PICOJSON_USE_INT64

#define _GEN_ADD_CONPAIR(TYPE)\
template<typename U>\
void _addPair(IdentifierType id, std::TYPE<U> T::*member) {\
    parsers[id] = [member](T& t, picojson::value v) {\
        t.*member = internal::toContainer<std::TYPE<U>>(v);\
    };\
}

#define ENABLE_IF(...)\
typename std::enable_if<VA_ARG>::type* = nullptr

#define ENABLE_IF_MEMBER_FUNCTION(TYPE)\
typename std::enable_if<std::is_member_function_pointer<TYPE>::value>::type* = nullptr

#define ENABLE_IF_MEMBER_OBJECT(TYPE)\
typename std::enable_if<std::is_member_object_pointer<TYPE>::value>::type* = nullptr

#include <iostream>

#include <deque>
#include <set>
#include <unordered_set>
#include <map>
#include <type_traits>
#include <memory>
#include <fstream>
#include <list>
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

template<class T>
class Mapper {
public:
    typedef T mapper_type;
    typedef std::string IdentifierType;
    typedef std::function<void(T&, picojson::value)> MapperType;

    static Mapper<T> global() {
        return globalMapper();
    }

    template<typename... Args>
    static Mapper<T> createGlobal(Args... args) {
        globalMapper().clear();
        globalMapper() = _create(globalMapper(), std::forward<Args>(args)...);
        return globalMapper();
    }

    template<typename... Args>
    static Mapper<T> create(Args... args) {
        Mapper<T> p;
        return _create(p, std::forward<Args>(args)...);
    }

    template<typename A, typename B>
    static MapperType containerMapper(A member, Mapper<B> pt) {
        return _containerMapper(member, pt);
    }

    template<typename A, typename B>
    static MapperType mapper(A member, B pt) {
        return _mapper(member, pt);
    }

    void addPair(IdentifierType id, MapperType p) {
        parsers[id] = p;
    }

    template<typename Member,
             ENABLE_IF_MEMBER_FUNCTION(Member)>
    void addPair(IdentifierType id, Member p) {
        _addSetter(id, p);
    }

    template<typename Member,
             ENABLE_IF_MEMBER_OBJECT(Member)>
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
        return t;
    }

    void clear() {
        parsers.clear();
    }

private:
    std::unordered_map<IdentifierType, MapperType> parsers;

    static Mapper<T>& globalMapper() {
        static Mapper<T> mapper;
        return mapper;
    }

    static Mapper<T> _create(Mapper<T> p) { return p; }

    template<typename Second, typename... Args>
    static Mapper<T> _create(Mapper<T> p, IdentifierType f, Second s, Args... args) {
        p.addPair(f, s);
        return _create(p, std::forward<Args>(args)...);
    }

    template<typename U, typename Mapper>
    static MapperType _containerMapper(U T::*member, Mapper pt) {
        return [=](T& t, picojson::value v) {
            auto va = v.get<picojson::array>();
            for (auto& x : va) {
                (t.*member).insert((t.*member).end(), pt.init(x));
            }
        };
    }

    template<typename U, typename Mapper>
    static MapperType _mapper(U T::*member, Mapper pt) {
        return [=](T& t, picojson::value v) {
            t.*member = std::move(pt.init(v));
        };
    }

    template<typename U>
    void _addSetter(IdentifierType id, U T::*member) {
        parsers[id] = [member](T& t, picojson::value v) {
            typedef typename cu::tmp::function_traits<U>::template argument<0>::type argtype;
            (t.*member)(internal::to<argtype>(v));
        };
    }

    _GEN_ADD_CONPAIR(list)
    _GEN_ADD_CONPAIR(vector)
    _GEN_ADD_CONPAIR(deque)
    _GEN_ADD_CONPAIR(set)
    _GEN_ADD_CONPAIR(multiset)
    _GEN_ADD_CONPAIR(unordered_set)
    _GEN_ADD_CONPAIR(unordered_multiset)

    template<typename M, typename K = typename M::key_type, typename V = typename M::mapped_type>
    static void _mapHelper(M& map, picojson::value v) {
            auto vo = v.get<picojson::object>();
            for (auto& x : vo) {
                auto key = internal::toStr<K>(x.first);
                auto value = internal::to<V>(x.second);
                map.insert(std::make_pair(key, value));
            }
    }

    template<typename K, typename V>
    void _addPair(IdentifierType id, std::unordered_map<K, V> T::*member) {
        parsers[id] = [member](T& t, picojson::value v) { _mapHelper(t.*member, v); };
    }

    template<typename K, typename V>
    void _addPair(IdentifierType id, std::map<K, V> T::*member) {
        parsers[id] = [member](T& t, picojson::value v) { _mapHelper(t.*member, v); };
    }

    template<typename U>
    void _addPair(IdentifierType id, U T::*member) {
        parsers[id] = [member](T& t, picojson::value v) {
            t.*member = internal::to<U>(v);
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
