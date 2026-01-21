#pragma once

#include <string>


class Dispalyable
{
public:
	virtual std::string GetClassName() = 0; // 纯虚函数，抽象类
};


class Person : Dispalyable // 实现继承 Dispalyable 接口
{
public:
	std::string m_name = "person";
	int m_serial = 0;
	int m_age = 10;
public:
	Person(const char* name);
	void Print();
	virtual void SayHello(); // 多态
	virtual std::string GetClassName() override { return "Person"; } // 实现 Dispalyable 接口
};

class Student : public Person // 继承
{
public:
	int m_serial = 1;
	int m_studentID;
	int m_grade;
public:
	void Show();
	Student(const char *name, int studentID, int m_grade);
	void SayHello() override;
	virtual std::string GetClassName() override { return "Student"; } // 重写父类的虚函数
};