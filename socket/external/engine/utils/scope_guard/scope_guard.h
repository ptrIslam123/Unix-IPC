#pragma once

#include <iostream>

namespace util {

namespace details {

enum class DeleterType {
    Function,
    Functor
};

template<typename T, typename D, DeleterType type>
struct DeleterHelper;

template<typename T, typename D>
struct DeleterHelper<T, D, DeleterType::Function> {
    typedef T ValueType;
    typedef ValueType *PointerType;
    typedef D DeleterType;

    DeleterHelper():deleter_() {};
    ~DeleterHelper() = default;

    void operator()(PointerType ptr) {
        deleter_(ptr);
    }

private:
    DeleterType deleter_;
};

template<typename T, typename D>
struct DeleterHelper<T, D, DeleterType::Functor> : public D {
    typedef T ValueType;
    typedef ValueType *PointerType;
    typedef D DeleterType;

    DeleterHelper() {};
    ~DeleterHelper() = default;

    void operator()(PointerType ptr) {
        D::operator()(ptr);
    }
};

} // namespace details

template<typename T, typename D>
struct ScopeGuard : public details::DeleterHelper<T, D, details::DeleterType::Functor> {
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
};

template<typename T, typename D>
ScopeGuard<T, D>::ScopeGuard(ValueType &&value):
details::DeleterHelper<T, D, details::DeleterType::Functor>(),
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
    details::DeleterHelper<T, D, details::DeleterType::Functor>::operator()(&value_);
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
