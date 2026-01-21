#pragma once

class Entity {
public:
	int m_x;
	int m_y;
public:
	Entity();
	Entity(int x, int y);
	int Sum();
	void Print(int num);
};