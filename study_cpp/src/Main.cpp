#include <iostream>
#include "Log.h"
#include "Entity.h"
#include "Singleton.h"
#include "Inheritance.h"

void egLog()
{
	Log log;
	log.setLogLevel(Log::LEVEL_INFO);
	log.debug("hello");
	log.info("hello");
	log.warning("hello");
	log.error("hello");
}


void egEntity()
{
	Entity e(10, 20); // 构造函数
	e.Print(e.Sum());
}

void egSingleton()
{
	Singleton::Get();
	std::cout << "num = " << Singleton::Get().m_num << std::endl;
	Singleton::Get().Increment();
	std::cout << "num = " << Singleton::Get().m_num << std::endl;
}

void egInheritance()
{
	Person person("Alan");
	Student student("BiBi", 10, 1111);

	// 继承
	person.Print();
	student.Print();
	student.Show();

	std::cout << "=======================" << std::endl;

	// 多态
	Person* p = &student; // 父类指针指向子类对象
	p->SayHello();
	student.SayHello();

	std::cout << "=======================" << std::endl;

	//接口
	std::cout << "p className = " << p->GetClassName() << std::endl;
	std::cout << "student className = " << student.GetClassName() << std::endl; // 这里子类也要重写基类的这个虚函数，不然会去调用基类的
}

/// <summary>
/// 
/// </summary>

#include <string>
std::string concatArrary(const std::string& str) {
	std::string str2 = str;
	return str2 += " good";
}

void egArray()
{
	const char* name = "Shelton";
	char name2[] = "Shelton";

	std::cout << "new name = " << concatArrary(name) << std::endl;
}

/// <summary>
/// 
/// </summary>

void Print(const std::string& str)
{
	std::cout << str << std::endl;
}

void egString()
{
	const char* str = R"(hello
world
ok
)";
	Print(str);

	std::string s = std::string("ok") + "kk";
	Print(s);
}

/// <summary>
/// 
/// </summary>

void egConst()
{
	// 常量
	const int a = 10;

	// 指针
	int b = 20;
	const int* p = &b;
	//(*p)++;
	p = &b; // 指针的内容不能改，但是指向的地址可以改

	int c = 20;
	int* const p2 = &b; // 相反
	(*p2)++;
	//p2 = &c;
}

/// <summary>
/// 
/// </summary>

void egMutable()
{
	int a = 20;
	auto function = [=]() mutable
		{
			a++; // 捕获的变量是只读的，不能修改，但用了mutbale
		};
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>

class Dog {
public:
	std::string m_Name;
	int m_Age;
public:
	Dog(const std::string& name, int age)
		:m_Name(name), m_Age(age)
	{
	};
};

/// <summary>
/// 
/// </summary>

class Vector2 {
public:
	float m_X;
	float m_Y;
public:
	Vector2(float x, float y)
		: m_X(x), m_Y(y)
	{
	}

	// api

	Vector2 Add(const Vector2& other) const
	{
		return Vector2(m_X + other.m_X, m_Y + other.m_Y);
	}

	Vector2 Multiply(const Vector2& other) const
	{
		return Vector2(m_X * other.m_X, m_Y * other.m_Y);
	}

	bool Equal(const Vector2& other) const
	{
		return (m_X == other.m_X) && (m_Y == other.m_Y);
	}

	// 重载运算符

	const Vector2 operator+(const Vector2& other) const
	{
		return Add(other);
	}

	const Vector2 operator*(const Vector2& other) const
	{
		return Multiply(other);
	}

	const bool operator==(const Vector2& other) const
	{
		return Equal(other);
	}
};

// 重载 cout的 <<
std::ostream& operator<<(std::ostream& os, const Vector2& v)
{
	return os << "[" << v.m_X << ", " << v.m_Y << "]" << std::endl;
};

void egOperator()
{
	Vector2 v1(5.5f, 2.5f);
	Vector2 v2(3.5f, 4.5f);
	//Vector2 v3 = v1.Add(v2);
	Vector2 v3 = v1 + v2 * v2;
	//std::cout << "v3 = [" << v3.m_X << "," << v3.m_Y << "]" << std::endl;
	std::cout << "v3 = " << v3 << std::endl;

	std::string isEqual = (v2 == v3) ? "yes" : "no";
	std::cout << "v2 equals v3 ? " << isEqual << std::endl;
}


/// <summary>
/// 
/// </summary>
/// <returns></returns>


class Sheep
{
public:
	Sheep()
	{
		std::cout << "Sheep constructor called" << std::endl;
	};
	~Sheep()
	{
		std::cout << "Sheep destructor called" << std::endl;
	};

	void Print()
	{
		std::cout << "Sheep Print called" << std::endl;
	};
};


// class Sheep; 的前向声明，可能在实际使用时没有正确包含 Sheep 的完整定义。
// 需要确保 Sheep 的定义在 ScopePointer 的实现之前是可见的。
class ScopePointer
{
public:
	Sheep* m_Ptr;
public:
	ScopePointer(Sheep* s)
		:m_Ptr(s)
	{
		std::cout << "ScopePointer constructor called" << std::endl;
	}

	~ScopePointer()
	{
		delete m_Ptr;
		std::cout << "ScopePointer destructor called" << std::endl;
	}
};


void egScopePointer()
{
	//Sheep(); // 栈上
	//new Sheep(); // 堆上
	ScopePointer sheep(new Sheep());
}

/// <summary>
/// 
/// </summary>
#include <memory>
void egSmartPointer()
{
	// 作用域指针
	//std::unique_ptr<Sheep> sheep = std::make_unique<Sheep>();

	// 引用计数指针
	//std::shared_ptr<Sheep> sheep;
	//{
	//	std::shared_ptr<Sheep> sheep2 = std::make_shared<Sheep>();
	//	sheep = sheep2;
	//}
	//这里sheep不会被销毁，因为只有sheep的引用计数归0了


	// 基于shared_ptr
	std::weak_ptr<Sheep> sheep;
	{
		std::shared_ptr<Sheep> sheep2 = std::make_shared<Sheep>();
		sheep = sheep2;
	}
	// 这里sheep和sheep2都被销毁了，weak_ptr不会增加引用计数
}

/// <summary>
/// 
/// </summary>

class String
{
private:
	int m_Size;
	char* m_Data;
public:
	String(const char* str)						// 拷贝
		:m_Size(strlen(str))
	{
		//std::cout << "拷贝构造" << std::endl;
		m_Data = new char[m_Size + 1];			// 多申请 \0 的位置
		memcpy(m_Data, str, m_Size + 1);		// 把 \0 也拷贝过来了
	}

	String(const String& other)					// 拷贝构造
	:m_Size(other.m_Size)
	{
		//std::cout << "拷贝构造"<< std::endl;
		m_Data = new char[other.m_Size + 1];
		memcpy(m_Data, other.m_Data, m_Size + 1);
	}

	// 重载[]
	char& operator[](int index)					// 这里必须是引用
	{
		return m_Data[index];
	}

	~String()									// 析构
	{
		//std::cout << "析构" << std::endl;
		delete[] m_Data;
	}

	friend std::ostream& operator<<(std::ostream& os, String str1);  // 友元
};

std::ostream& operator<<(std::ostream& os, String str1)
{
	return os << str1.m_Data;
};

void egCopying()
{
	String str1 = "hello";	// 拷贝构造，创建一个hello的String变量，拷贝给str1
	String str2 = str1;		// 拷贝构造

	std::cout << str1 << std::endl;
	std::cout << str2 << std::endl;
	
	str2[2] = 'k';
	std::cout << str1 << std::endl;
	std::cout << str2 << std::endl;
}


/// <summary>
/// 
/// </summary>
/// <returns></returns>

#include <vector>

struct Vertex
{
	float x, y, z;
};

std::ostream& operator<<(std::ostream& os, Vertex v)
{
	return os << v.x << "," << v.y << "," << v.z;
}

void egVector()
{
	std::vector<Vertex> verties;
	verties.push_back({1,2,3});
	verties.push_back({4,5,6});

	std::cout << verties[0] << std::endl;
	std::cout << verties[1] << std::endl;

	std::cout << "==========================" << std::endl;

	for (int i = 0; i < verties.size(); i++)
	{
		std::cout << verties[i] << std::endl;
	}

	std::cout << "==========================" << std::endl;

	verties.erase(verties.begin()); // 删除第0个元素（按元素删除）
	for (Vertex v: verties)
	{
		std::cout << v << std::endl;
	}
	
}

int main()
{
	//egLog();
	//egEntity();
	egSingleton();
	//egInheritance();
	//egArray();
	//egString();
	//egConst();
	//egMutable();
	//egOperator();
	//egScopePointer();
	//egSmartPointer();
	//egCopying();
	//egVector();

	std::cin.get();
}