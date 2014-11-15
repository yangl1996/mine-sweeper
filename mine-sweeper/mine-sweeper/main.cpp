#include <iostream>

using namespace std;

// the size is 50 * 50
void paint(int (*status)[50], int col, int row)
{
    cout << "┌";
    for (int i = 1; i < col; i++)
    {
        cout << "─" << "┬";
    }
    cout << "─";
    cout << "┐";
    cout << endl;
    for (int i = 0; i < row - 1; i++)
    {
        cout << "│";
        for (int j = 0; j < col; j++)
        {
            cout << status[i][j];
            cout << "│";
        }
        cout << endl << "├";
        for (int j = 1; j < col; j++)
        {
            cout << "─" << "┼";
        }
        cout << "─";
        cout << "┤" << endl;
    }
    cout << "│";
    for (int j = 0; j < col; j++)
    {
        cout << status[row - 1][j];
        cout << "│";
    }
    cout << endl << "└";
    for (int j = 1; j < col; j++)
    {
        cout << "─" << "┴";
    }
    cout << "─" << "┘" << endl;
}

int main()
{
    int status[50][50] = {0};
    paint(status, 10, 10);
    return 0;
}
