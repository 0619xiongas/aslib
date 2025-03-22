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

#endif
