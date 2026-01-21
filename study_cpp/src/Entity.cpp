#include <Entity.h>
#include <iostream>

Entity::Entity()
	:m_x(0), m_y(0)
{
}

Entity::Entity(int x, int y) 
{
	m_x = x;
	m_y = y;
}

int Entity::Sum() 
{
	return m_x + m_y;
}

void Entity::Print(int num) 
{
	std::cout << "num = " << num;
}