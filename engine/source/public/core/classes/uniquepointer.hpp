#ifndef GAMEENGINE_CORE_CLASSES_UNIQUE_POINTER_HPP_INCLUDE
#define GAMEENGINE_CORE_CLASSES_UNIQUE_POINTER_HPP_INCLUDE

template<typename T>
struct DefaultDeleter {
	void operator()(T* p) {
		delete p;
	}
};

template<typename T, typename Deleter = DefaultDeleter<T>>
class UniquePointer {
	T* ptr;
	Deleter d;

public:
	template<typename = EnableIf<IsNothrowDefaultConstructible<Deleter>>>
	UniquePointer(): ptr(nullptr), d(Deleter()) {}
	UniquePointer(UniquePointer const&) = delete;
	UniquePointer(UniquePointer&& unique): ptr(unique.ptr), d(unique.d) {
		unique.ptr = nullptr;
	}
};

#endif // !GAMEENGINE_CORE_CLASSES_UNIQUE_POINTER_HPP_INCLUDE