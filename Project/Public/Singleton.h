#ifndef __Singleton_h__
#define __Singleton_h__
template< typename T >
class Singleton
{
public:
	Singleton(){ _instance = static_cast< T *>(this); };
	virtual ~Singleton(){};
	static T * GetInstancePtr(){ return _instance; };
	static T & GetInstance(){ return *_instance; };
private:
	static T *_instance;
};
#endif