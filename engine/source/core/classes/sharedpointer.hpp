#ifndef GAMEENGINE_CORE_CLASSES_SHARED_POINTER_HPP_INCLUDE
#define GAMEENGINE_CORE_CLASSES_SHARED_POINTER_HPP_INCLUDE

#include "../TypeDefinitions.hpp"
#include "UniquePointer.hpp"

template <typename T>
class WeakPointer;

template <typename T>
class SharedPointer {
    template <typename U>
    friend class WeakPointer;

private:
    template <typename T>
    class SharedPointerStorage {
    public:
        uint64 ptrCount;
        uint64 weakCount;

        SharedPointerStorage() : ptrCount(1), weakCount(0) {}
        virtual ~SharedPointerStorage(){};

        virtual T* get() = 0;
        virtual void destroy() = 0;

        void increment() {
            ++ptrCount;
        }

        void incrementWeak() {
            ++weakCount;
        }

        void decrement() {
            --ptrCount;
        }

        void decrementWeak() {
            --weakCount;
        }
    };

    template <typename T, typename Deleter>
    class SharedPointerStorageImpl : public SharedPointerStorage<T> {
    private:
        Deleter deleter;
        T* ptr;

    public:
        SharedPointerStorageImpl(T* p, Deleter d) : ptr(p), deleter(d) {}

        virtual T* get() override {
            return ptr;
        }

        virtual void destroy() override {
            deleter(ptr);
        }
    };

    SharedPointerStorage<T>* ptrStorage;

    void decrementStorage() {
        ptrStorage->decrement();
        if (ptrStorage->ptrCount == 0) {
            ptrStorage->destroy();
            if (ptrStorage->weakCount == 0) {
                delete ptrStorage;
            }
        }
    }

public:
    SharedPointer() noexcept : ptrStorage(new SharedPointerStorageImpl<T, DefaultDeleter<T>>(nullptr, DefaultDeleter<T>())) {}
    SharedPointer(SharedPointer const& sharedPtr) noexcept : ptrStorage(sharedPtr.ptrStorage) {
        ptrStorage->increment();
    }
    SharedPointer(SharedPointer&& sharedPtr) noexcept : ptrStorage(sharedPtr.ptrStorage) {
        ptrStorage = nullptr;
    }
    SharedPointer& operator=(SharedPointer const& sharedPtr) noexcept {
        decrementStorage();
        ptrStorage = sharedPtr.ptrStorage;
        ptrStorage.increment();
    }
    SharedPointer& operator=(SharedPointer&& sharedPtr) noexcept {
        decrementStorage();
        ptrStorage = sharedPtr.ptrStorage;
        sharedPtr.ptrStorage = nullptr;
    }
    ~SharedPointer() {
        decrementStorage();
    }

    template <typename U, typename = EnableIf<IsConvertible<U*, T*>>>
    SharedPointer(SharedPointer<U> const& shared) noexcept : ptrStorage(shared.ptrStorage) {
        ptrStorage->increment();
    }
    template <typename U, typename = EnableIf<IsConvertible<U*, T*>>>
    SharedPointer(SharedPointer<U>&& shared) noexcept : ptrStorage(shared.ptrStorage) {}
    template <typename U, typename = EnableIf<IsConvertible<U*, T*>>>
    SharedPointer& operator=(U* ptr) {
        ptrStorage = new SharedPointerStorageImpl<T, DefaultDeleter<T>>(ptr, DefaultDeleter<T>());
    }
    template <typename U, typename = EnableIf<IsConvertible<U*, T*>>>
    explicit SharedPointer(U* ptr) : ptrStorage(new SharedPointerStorageImpl<T, DefaultDeleter<T>>(ptr, DefaultDeleter<T>())) {}
    template <typename U, typename Deleter, typename = EnableIf<IsConvertible<U*, T*>>>
    SharedPointer(U* ptr, Deleter deleter) : ptrStorage(new SharedPointerStorageImpl<T, Deleter>(ptr, deleter)) {}
    template <typename U>
    SharedPointer(WeakPointer<U> const& weak) noexcept : ptrStorage(weak.ptrStorage) {}
    /*template<typename U, typename Deleter>
    SharedPointer(UniquePointer&& unique) noexcept {}*/

    T* operator->() {
        return ptrStorage->get();
    }
    T& operator*() {
        return *ptrStorage->get();
    }
    T* get() {
        return ptrStorage->get();
    }
};

template <typename T>
class WeakPointer {
    template <typename U>
    friend class SharedPointer;

private:
    typename SharedPointer<T>::template SharedPointerStorage<T>* ptrStorage;

public:
    WeakPointer() : ptrStorage(nullptr) {}

    bool expired() {
        return ptrStorage->weakCount == 0;
    }
};

#endif // !GAMEENGINE_CORE_CLASSES_SHARED_POINTER_HPP_INCLUDE