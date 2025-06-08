#ifndef __SINGLE_H__
#define __SINGLE_H__
#include <mutex>
#include <memory>


template<class T>
class asSingle
{
protected:
	asSingle() = default;
	asSingle(const asSingle<T>& s) = delete;
	asSingle& operator=(const asSingle<T>& s) = delete;
	~asSingle() {}

public:
	static std::shared_ptr<T> instance()
	{
		if (single != nullptr)
		{
			return single;
		}
		m_mutex.lock();
		if (single != nullptr)
		{
			m_mutex.unlock();
			return single;
		}
		single = std::shared_ptr<T>(new T);
		m_mutex.unlock();
		return single;
	}
private:
	static std::shared_ptr<T> single;
	static std::mutex m_mutex;
};
// 模板类要求在h文件初始化
template<class T>
std::shared_ptr<T> asSingle<T>::single = nullptr;
template<class T>
std::mutex asSingle<T>::m_mutex;

template<class T>
class asSingleton
{
protected:
	asSingleton() = default;
	asSingleton(const asSingleton<T>& s) = delete;
	asSingleton& operator=(asSingleton<T>& s) = delete;
	~asSingleton(){}

public:
	static T* instance()
	{
		if (s_single != nullptr)
		{
			return s_single;
		}
		s_mutex.lock();
		if (s_single != nullptr)
		{
			s_mutex.unlock();
			return s_single;
		}
		s_single = new T;
		s_mutex.unlock();
		return s_single;
	}
	static void delete_instance()
	{
		s_mutex.lock();
		if (s_single)
		{
			delete s_single;
			s_single = nullptr;
		}
		s_mutex.unlock();
	}
private:
	static T* s_single;
	static std::mutex s_mutex;
}; 
template<class T>
T* asSingleton<T>::s_single = nullptr;
template<class T>
std::mutex asSingleton<T>::s_mutex;

#endif
