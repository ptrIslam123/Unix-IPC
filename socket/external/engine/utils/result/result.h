#pragma once

#include <variant>
#include <cassert>

/**
 * enum Result<T, E> {
 *     Ok() -> T
 *     Error() -> E
 * }
 */

namespace util {

template<typename T, typename E>
class Result {
public:
    typedef T ValueType;
    typedef E ErrorType;

    Result(const ValueType &value);
    Result(T &&value);
    Result(const ErrorType &error);
    Result(ErrorType &&error);

    bool isError() const;
    bool isOk() const;

    ValueType &value();
    const ValueType  &value() const;

    ErrorType &error();
    const ErrorType &error() const;

private:
    std::variant<ValueType , ErrorType> value_;
};

template<typename T, typename E>
Result<T, E>::Result(const ValueType &value):
value_(value) {}

template<typename T, typename E>
Result<T, E>::Result(ValueType &&value):
value_(std::move(value)) {}

template<typename T, typename E>
Result<T, E>::Result(const ErrorType &error):
value_(error) {}

template<typename T, typename E>
Result<T, E>::Result(ErrorType &&error):
value_(std::move(error)) {}

template<typename T, typename E>
bool Result<T, E>::isOk() const {
    return std::holds_alternative<ValueType>(value_);
}

template<typename T, typename E>
bool Result<T, E>::isError() const {
    return std::holds_alternative<ErrorType>(value_);
}

template<typename T, typename E>
typename Result<T, E>::ValueType &Result<T, E>::value() {
    assert(isOk());
    return std::get<ValueType>(value_);
}

template<typename T, typename E>
const typename Result<T, E>::ValueType &Result<T, E>::value() const {
    assert(isOk());
    return std::get<ValueType>(value_);
}


template<typename T, typename E>
typename Result<T, E>::ErrorType &Result<T, E>::error() {
    assert(isError());
    return std::get<ErrorType>(value_);
}

template<typename T, typename E>
const typename Result<T, E>::ErrorType &Result<T, E>::error() const {
    assert(isError());
    return std::get<ErrorType>(value_);
}

} // namespace util