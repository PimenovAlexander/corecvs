#pragma once

//#include "utils/global.h"

template<typename T>
class CountedPtr
{
public:
	CountedPtr() : t(new T()), t_ref(new int(1)) {}
	CountedPtr(T* _t) : t(_t), t_ref(new int(1)) {}
	CountedPtr(const CountedPtr& p) : t(p.t), t_ref(p.t_ref) {
		++(*t_ref);
	}

	~CountedPtr() {
		if (!--(*t_ref)) {
			delete t_ref;  // delete_safe couldn't be used here (BufferFactory's friend doesn't work) :(
			delete t;
		}
	}

	T* get() { return t; }

private:

	/** @brief Assignment operation */
	CountedPtr& operator=(const CountedPtr&);

	/** @brief Pointer to object */
	T*   t;
	/** @brief Reference counter onto object */
	int* t_ref;
};
