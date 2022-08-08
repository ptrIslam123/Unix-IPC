#pragma once

#include <type_traits>

namespace util {

template<typename T, typename D>
struct ScopeGuard {
public:
    typedef T ValueType;
    typedef D DeleterType;

    ScopeGuard(ValueType &&value);
    ScopeGuard(ScopeGuard &&other) noexcept ;
    ScopeGuard &operator=(ScopeGuard &&other);
    ScopeGuard(const ScopeGuard &other) = delete;
    ScopeGuard &operator=(const ScopeGuard &other) = delete;
    ~ScopeGuard();

    const ValueType &operator*() const;

    ValueType *operator->();
    const ValueType *operator->() const;

private:
    ValueType value_;
    DeleterType deleter_;
};

template<typename T, typename D>
ScopeGuard<T, D>::ScopeGuard(ValueType &&value):
value_(std::move(value)) {
}

template<typename T, typename D>
ScopeGuard<T, D>::ScopeGuard(ScopeGuard<T, D> &&other) noexcept :
value_(std::move(other.value_)){
}

template<typename T, typename D>
ScopeGuard<T, D> &ScopeGuard<T, D>::operator=(ScopeGuard &&other) {
    value_ = std::move(other.value_);
    return *this;
}

template<typename T, typename D>
const typename ScopeGuard<T, D>::ValueType &ScopeGuard<T, D>::operator*() const {
    return value_;
}

template<typename T, typename D>
ScopeGuard<T, D>::~ScopeGuard() {
    deleter_(&value_);
}

template<typename T, typename D>
typename ScopeGuard<T, D>::ValueType *ScopeGuard<T, D>::operator->() {
    return &value_;
}

template<typename T, typename D>
const typename ScopeGuard<T, D>::ValueType *ScopeGuard<T, D>::operator->() const {
    return &value_;
}

} // namespace util
