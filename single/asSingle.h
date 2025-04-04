/*************************************************************************************
 *
 * �� �� ��:   asSingle.h
 * ��    ��:		����ģ��
 * 
 * ��    ����  V1.0
 * �� �� �ߣ�  astronaut
 * ����ʱ�䣺  2024/3/29 13:01
 * ======================================
*************************************************************************************/

#ifndef AS_SINGLE_H
#define AS_SINGLE_H

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
// ģ����Ҫ����h�ļ���ʼ��
template<class T>
std::shared_ptr<T> asSingle<T>::single = nullptr;
template<class T>
std::mutex asSingle<T>::m_mutex;

#endif
