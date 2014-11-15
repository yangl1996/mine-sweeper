#include <iostream>

using namespace std;

/*
 Status
 number 1-8 : 附近的地雷数
 number 0   : 附近没有地雷
 number -1  : 未翻开
 number 9   : 插上了棋子
 number 10  : 插上了问号
 */
void paintStatus(int status)
{
    switch (status)
    {
        case 0:
        {
            cout << " ";
            break;
        }
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        {
            cout << status;
            break;
        }
        case -1:
        {
            cout << "#";
            break;
        }
        case 9:
        {
            cout << "X";
            break;
        }
        case 10:
        {
            cout << "?";
            break;
        }
    }
}

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
            paintStatus(status[i][j]);
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
        paintStatus(status[row - 1][j]);
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
    int status[50][50] = {0}; //Status是经过计算的结果。也就是说，需要根据布雷计算出附近的地雷数，存在status中
    status[0][0] = -1;
    status[0][1] = 2;
    status[0][3] = 10;
    status[0][4] = 9;
    paint(status, 15, 15);
    return 0;
}
