#ifndef WIZ_UTILITY_OPTIONAL_H
#define WIZ_UTILITY_OPTIONAL_H

#include <cassert>
#include <utility>
#include <type_traits>

#include <wiz/utility/macros.h>

namespace wiz {

    template <typename T>
    class Optional {
        public:
            Optional()
            : hasValue_(false) {
                new(&data) T();
            }

            template<typename U>
            Optional(const U& value)
            : hasValue_(true) {
                new(&data) T(value);
            }

            template<typename U,
                std::enable_if_t<!std::is_same<std::decay_t<U>, Optional>::value>* = nullptr,
                std::enable_if_t<std::is_constructible<T, U>::value>* = nullptr
            >
            Optional(U&& value)
            : hasValue_(true) {
                new(&data) T(std::forward<U>(value));
            }

            Optional(const Optional& other)
            : hasValue_(other.hasValue_) {
                if (hasValue_) {
                    new(&data) T(*reinterpret_cast<std::add_pointer_t<const T>>(&other.data));
                }
            }

            Optional(Optional&& other)
            : hasValue_(other.hasValue_) {
                other.hasValue_ = false;
                if (hasValue_) {
                    new(&data) T(std::move(*reinterpret_cast<std::add_pointer_t<T>>(&other.data)));
                }
            }

            ~Optional() {
                if (hasValue_) {
                    reinterpret_cast<std::add_pointer_t<T>>(&data)->~T();
                }
            }

            Optional& operator =(const Optional& other) {
                if (this != &other) {
                    reinterpret_cast<std::add_pointer_t<T>>(&data)->~T();
                    hasValue_ = other.hasValue_;
                    if (hasValue_) {
                        new(&data) T(*reinterpret_cast<std::add_pointer_t<const T>>(&other.data));
                    }
                }
                return *this;
            }

            Optional& operator =(Optional&& other) {
                if (this != &other) {
                    reinterpret_cast<std::add_pointer_t<T>>(&data)->~T();
                    hasValue_ = other.hasValue_;
                    other.hasValue_ = false;
                    if (hasValue_) {
                        new(&data) T(std::move(*reinterpret_cast<std::add_pointer_t<T>>(&other.data)));
                    }
                }
                return *this;
            }

            WIZ_FORCE_INLINE explicit operator bool() const {
                return hasValue_;
            }

            WIZ_FORCE_INLINE T& operator *() & {
                return get();
            }

            WIZ_FORCE_INLINE const T& operator *() const & {
                return get();
            }

            WIZ_FORCE_INLINE T&& operator *() && {
                return get();
            }

            WIZ_FORCE_INLINE const T&& operator *() const && {
                return get();
            }

            WIZ_FORCE_INLINE T* operator ->() {
                return &get();
            }

            WIZ_FORCE_INLINE const T* operator ->() const {
                return &get();
            }

            WIZ_FORCE_INLINE bool hasValue() const {
                return hasValue_;
            }

            WIZ_FORCE_INLINE T& get() & {
                assert(hasValue_);
                return *reinterpret_cast<std::add_pointer_t<T>>(&data);
            }

            WIZ_FORCE_INLINE const T& get() const & {
                assert(hasValue_);
                return *reinterpret_cast<std::add_pointer_t<const T>>(&data);
            }

            WIZ_FORCE_INLINE T&& get() && {
                assert(hasValue_);
                return std::move(*reinterpret_cast<std::add_pointer_t<T>>(&data));
            }

            WIZ_FORCE_INLINE const T&& get() const && {
                assert(hasValue_);
                return std::move(*reinterpret_cast<std::add_pointer_t<const T>>(&data));
            }

            WIZ_FORCE_INLINE T getOrDefault(T defaultValue) const {
                if (hasValue()) {
                    return get();
                } else {
                    return defaultValue;
                }
            }

            WIZ_FORCE_INLINE std::add_pointer_t<T> tryGet() {
                if (hasValue()) {
                    return &get();
                }
                return nullptr;
            }

            WIZ_FORCE_INLINE std::add_pointer_t<const T> tryGet() const {
                if (hasValue()) {
                    return &get();
                }
                return nullptr;
            }
        private:
            using DataType = std::aligned_storage_t<sizeof(T), alignof(T)>;

            bool hasValue_;
            DataType data;
    };
}
#endif
