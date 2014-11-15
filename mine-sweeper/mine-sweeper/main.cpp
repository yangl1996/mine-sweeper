#include <iostream>
#include <ctime>
#include <cstdlib>

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
void paint(int (*status)[52], int col, int row)
{
    cout << "┌";
    for (int i = 2; i <= col; i++)
    {
        cout << "─" << "┬";
    }
    cout << "─";
    cout << "┐";
    cout << endl;
    for (int i = 1; i <= row - 1; i++)
    {
        cout << "│";
        for (int j = 1; j <= col; j++)
        {
            paintStatus(status[i][j]);
            cout << "│";
        }
        cout << endl << "├";
        for (int j = 2; j <= col; j++)
        {
            cout << "─" << "┼";
        }
        cout << "─";
        cout << "┤" << endl;
    }
    cout << "│";
    for (int j = 1; j <= col; j++)
    {
        paintStatus(status[row - 1][j]);
        cout << "│";
    }
    cout << endl << "└";
    for (int j = 2; j <= col; j++)
    {
        cout << "─" << "┴";
    }
    cout << "─" << "┘" << endl;
}

int main()
{
    srand((unsigned)(time(NULL)));
    int mine[52][52] = {0};
    int row, col, num;
    cin >> row >> col >> num;
    while (num > row * col / 2)
    {
        cout << "Too many mines! Try smaller number." << endl;
        cout << "Retype number of mines:" << endl;
        cin >> num;
    }
    int a = 1, b = 1;
    while (num > 0)
    {
        if (random() % (row * col) <= num)
        {
            if (mine[a][b] != 1)
            {
                mine[a][b] = 1;
                num--;
            }
        }
        if (a != row)
        {
            if (b != col)
            {
                b++;
            }
            else
            {
                b = 1;
                a++;
            }
        }
        else
        {
            if (b != col)
            {
                b++;
            }
            else
            {
                a = 1;
                b = 1;
            }
        }
    }
    int status[52][52] = {0}; //Status是经过计算的结果。也就是说，需要根据布雷计算出附近的地雷数，存在status中
    for (int i = 1; i <= row; i++)
    {
        for (int j = 1; j <= col; j++)
        {
            status[i][j] = mine[i][j];
        }
    }
    paint(status, row, col);
    return 0;
}
