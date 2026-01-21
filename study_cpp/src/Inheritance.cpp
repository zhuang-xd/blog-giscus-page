#include "Inheritance.h"
#include <iostream>

Person::Person(const char *name) :m_name(name) {};

void Person::Print() {
	std::cout << "Name: " << m_name << ", Age: " << m_age << std::endl;
}

void Person::SayHello() {
	std::cout << "person: Hello" << std::endl;
}

Student::Student(const char *name, int studentID, int grade) 
	:Person(name),
	 m_studentID(studentID),
	 m_grade(grade)
{}

void Student::Show() {
	std::cout << "Student ID: " << m_studentID << ", Grade: " << m_grade << std::endl;
}

void Student::SayHello() {
	std::cout << "student: Hello" << std::endl;
}