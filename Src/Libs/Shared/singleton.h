#pragma once


// Templated Singleton class, inspired by:
// http://www.codeproject.com/KB/cpp/singleton_template.aspx
//
// Must be explicitly initialized and destroyed using Init(), Free()
// and needs a 'friend class Singleton<XXXX>' declaration inside the 
// singleton'ed class.

template <class T>
class Singleton
{
private:
	static T* instance;

protected:
	// Hide constructors
	Singleton() {}
	Singleton(const Singleton<T>& other) {}
	Singleton<T>& operator=(const Singleton<T>& other) { return *this; }
	virtual ~Singleton() {}

public:
	static T* Instance()	
	{
		ASSERT_MSG(instance != NULL, "Singleton not initialized yet!");
		return instance;
	}

	static void Init()
	{
		if (instance == NULL)
		{
			instance = new T();
		}		
	}

	static void Free()
	{
		delete instance;
		instance = NULL;
	}
};


template<class T> T* Singleton<T>::instance = 0;