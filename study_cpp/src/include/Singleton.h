#pragma once

// 单例模式
class Singleton {
public:
	int m_num = 1;
public:
	// 懒汉模式
	static Singleton& Get(); // 引用版本
	void Increment();
};