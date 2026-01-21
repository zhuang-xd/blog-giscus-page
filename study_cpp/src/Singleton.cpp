#include "Singleton.h"

// 这里不能加static，因为在.h文件中已经声明过是static了，这里再加编译器会认为是一个新的静态函数
Singleton& Singleton::Get() 
{
	static Singleton s;
	return s;
}

//static Singleton* Singleton::Get()
//{
//	static Singleton s;
//	return &s;
//}

void Singleton::Increment() {
	m_num++;
}