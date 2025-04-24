#ifndef AS_NETDEFINE_H
#define AS_NETDEFINE_H

#if __cplusplus >= 201103L
#define std11
#endif

#ifdef std11
#include <memory>
#define AS_SHARED_ARRAY std::shared_ptr
#define AS_SHARED_PTR std::shared_ptr
#define AS_MAKE_SHARED	std::make_shared
#else
#include "boost/smart_ptr.hpp"
#define AS_SHARED_ARRAY boost::shared_array
#define AS_SHARED_PTR boost::shared_ptr
#define AS_MAKE_SHARED boost::make_shared
#endif
template<typename T>
inline AS_SHARED_ARRAY<T> make_shared_array(unsigned int size)
{
#ifdef std11
	return std::shared_ptr<T>(new T[size], std::default_delete<T[]>());
#else
	return boost::shared_array<T>(new T[size]);
#endif
}
template<typename T>
inline AS_SHARED_ARRAY<T> make_shared_array(T* ptr)
{
#ifdef std11
	return std::shared_ptr<T>(ptr, std::default_delete<T[]>());
#else
	return boost::shared_array<T>(ptr);
#endif
}

#endif