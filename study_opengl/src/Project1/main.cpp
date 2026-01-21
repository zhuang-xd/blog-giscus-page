#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const int Width = 1920;
const int Height = 1080;

#if 1
// draw point / line / triangle
void drawPoint(double x, double y) {
	/* Draw a point */
	x = x / Width;
	y = y / Height;
	//glPointSize(2.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0, 0.0, 0.0);    // Red
	glVertex2f(x, y);
	glEnd();
}

void drawLint(double x1, double y1, double x2, double y2) {

	//glLineWidth(2);//设置线段宽度

	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(x1, y1); //定点坐标范围
	glVertex2f(x2, y2);
	glEnd();
}

void drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3) {
	glBegin(GL_TRIANGLES);

	glColor3f(1.0, 0.0, 0.0);    // Red
	glVertex3f(x1, y1, 0.0);

	glColor3f(0.0, 1.0, 0.0);    // Green
	glVertex3f(x2, y2, 0.0);

	glColor3f(0.0, 0.0, 1.0);    // Blue
	glVertex3f(x3, y3, 0.0);
	glEnd();
}

void MidPointLine(int x1, int y1, int x2, int y2) {
	bool change = false;
	bool x_change = false;

	if (1.0 * abs(y2 - y1) / abs(x2 - x1) > 1) {
		change = true;
		std::swap(x1, y1);
		std::swap(x2, y2);
	}
	if (x2 - x1 < 0) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	if (1.0 * (y1 - y2) / (x1 - x2) < 0 && 1.0 * (y1 - y2) / (x1 - x2) > -1) {
		y1 = -1 * y1; y2 = -1 * y2;
		x_change = true;
	}

	int a, b, c, x, y, d0, d1, d2, d;
	a = y1 - y2;
	b = x2 - x1;
	c = x1 * y2 - x2 * y1;
	d0 = 2 * a + b;
	d1 = 2 * a;
	d2 = 2 * (a + b);

	d = d0;
	x = x1; y = y1;
	drawPoint(x, y);
	while (x <= x2) {
		if (d < 0) {
			d += d2;
			x++; y++;
		}
		else {
			d += d1;
			x++;
		}

		if (!change)
			if (x_change) drawPoint(x, -1 * y);
			else drawPoint(x, y);
		else
			if (x_change) drawPoint(y, -1 * x);
			else drawPoint(y, x);
	}
}

void BresenHamline(int x1, int y1, int x2, int y2) {

	bool change = false;
	bool x_change = false;

	if (1.0 * abs(y2 - y1) / abs(x2 - x1) > 1) {
		change = true;
		std::swap(x1, y1);
		std::swap(x2, y2);
	}
	if (x2 - x1 < 0) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	if (1.0 * (y1 - y2) / (x1 - x2) < 0 && 1.0 * (y1 - y2) / (x1 - x2) > -1) {
		y1 = -1 * y1; y2 = -1 * y2;
		x_change = true;
	}


	int x, y, dx, dy;
	float k, e;
	dx = x2 - x1;
	dy = y2 - y1;
	k = 1.0 * dy / dx;
	e = -0.5; x = x1; y = y1;

	for (int i = 0; i <= dx; i++) {

		if (!change)
			if (x_change) drawPoint(x, -1 * y);
			else drawPoint(x, y);
		else
			if (x_change) drawPoint(y, -1 * x);
			else drawPoint(y, x);

		x = x + 1;
		e = e + k;

		if (e >= 0) {
			y++;
			e -= 1;
		}
	}
}

void IntegerBresenhamline(int x1, int y1, int x2, int y2) {

	bool change = false;
	bool x_change = false;

	if (1.0 * abs(y2 - y1) / abs(x2 - x1) > 1) {
		change = true;
		std::swap(x1, y1);
		std::swap(x2, y2);
	}
	if (x2 - x1 < 0) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	if (1.0 * (y1 - y2) / (x1 - x2) < 0 && 1.0 * (y1 - y2) / (x1 - x2) > -1) {
		y1 = -1 * y1; y2 = -1 * y2;
		x_change = true;
	}

	int x, y, dx, dy, e;
	dx = x2 - x1;
	dy = y2 - y1;
	e = -1 * dx;
	x = x1; y = y1;

	for (int i = 0; i <= dx; i++) {
		if (!change)
			if (x_change) drawPoint(x, -1 * y);
			else drawPoint(x, y);
		else
			if (x_change) drawPoint(y, -1 * x);
			else drawPoint(y, x);

		x++;
		e = e + 2 * dy;
		if (e >= 0) {
			y++;
			e -= 2 * dx;
		}
	}
}

void Drawlines(int x1, int y1, int x2, int y2) {

	int x = x1, y = y1, dx = abs(x2 - x1), dy = abs(y2 - y1), s1 = 1, s2 = 1, e, flag = 0;
	if (x1 >= x2) s1 = -1;
	if (y1 >= y2) s2 = -1;
	if (dy > dx) { int change = dx; dx = dy; dy = change;  flag = 1; }
	e = -dx;
	int DX = 2 * dx, DY = 2 * dy;
	for (int i = 1; i <= dx; i++) {
		drawPoint(x, y);
		if (e >= 0) {
			if (!flag) y += s2;
			else x += s1;
			e = e - DX;
		}
		if (!flag) x += s1;
		else y += s2;
		e = e + DY;
	}
}

void Bresenham_Circle(int xc, int yc, int r) {
	int x, y, d;
	x = 0;
	y = r;
	d = r;
	drawPoint(x + xc, y + yc);
	while (x < y)
	{
		if (d < 0) 
			d = d + 8 * x + 12;
		else {
			d = d + 8 * (x - y) + 20;
			y--;
		}
		x++;
		drawPoint(x + xc, y + yc); drawPoint(y + xc, x + yc);
		drawPoint(y + xc, -x + yc); drawPoint(x + xc, -y + yc);
		drawPoint(-x + xc, -y + yc); drawPoint(-y + xc, -x + yc);
		drawPoint(-x + xc, y + yc); drawPoint(-y + xc, x + yc);
	}
}



// Listen Mouse
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double X0, Y0;
		glfwGetCursorPos(window, &X0, &Y0);  //获得鼠标位置	
		printf("in %f %f ", X0, Y0);
		Bresenham_Circle(X0 - Width / 2, Height / 2 - Y0, 50);
	}

}

void curse_poscallback(GLFWwindow* window, double x, double y) {
	std::cout << "(pos:" << x << "," << y << ")" << std::endl;
}



void initWindow(GLFWwindow* window)
{
	glfwMakeContextCurrent(window);

	////设置按键回调
	//glfwSetKeyCallback(window, mouse_button_callback);
	//设置鼠标按键点下松开的回调
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	//设置鼠标移动的回调
	glfwSetCursorPosCallback(window, curse_poscallback);

	////设置窗口大小改变的回调，让绘画区域在窗口中间
	//glfwSetWindowSizeCallback(window, windowResize_callback);
}

void initParam()
{
	//显示规则：窗口左下角坐标为0，0；所以下行代码表示在窗口左下角向右向上的400个像素单位作为画布
	glViewport(0, 0, Width, Height);//设置显示区域400*400，但是可以拖动改变窗口大小
	glLineWidth(3.0);//设置线条宽度，3个像素

	glEnable(GL_BLEND);

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);//设置点圆滑

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);//设置线光滑

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}





int main(void) {

	if (!glfwInit()) return -1;

	// a windowed mode window and its OpenGL context 
	GLFWwindow* window = glfwCreateWindow(Width, Height, "Hello World", NULL, NULL);

	if (window == NULL) {
		glfwTerminate();
		return -1;
	}

	// Make the window's context current 
	glfwMakeContextCurrent(window);
	// Set Mouse

	initWindow(window);
	initParam();


	std::pair<int, int> test[8] = { {100,200},{200,100},{200,-100},{100,-200},{-100,-200},{-200,-100},{-200,100},{-100,200} };
	// Loop until the user closes the window 
	while (!glfwWindowShouldClose(window)) {

		//for (int i = 0; i < 8; i++) {
		//	IntegerBresenhamline(0, 0, test[i].first, test[i].second);
		//	/*	MidPointLine(test[i].first, test[i].second, 0, 0);*/
		//}

		Bresenham_Circle(0, 0, 100);


		// Swap front and back buffers 
		glfwSwapBuffers(window);
		// Poll for and process events 
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

#endif