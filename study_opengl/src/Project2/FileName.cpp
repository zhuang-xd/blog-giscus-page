#include <iostream>

using namespace std;

int main()
{
    int a[10] = { 2, 8, 3, 4, 6, 9, 7, 1, 5, 1 };

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9 - i; j++)
        {
            if (a[i] > a[i + 1]) {
                int tmp = a[i];
                a[i] = a[i + 1];
                a[i + 1] = tmp;
            }
        }
    }

    for (int i = 0; i < 10; i++)
    {
        cout << a[i] << " ";
    }
}