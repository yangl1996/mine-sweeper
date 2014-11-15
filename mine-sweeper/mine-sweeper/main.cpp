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

// paintStatus
// 用于打印某位置的棋盘情况。
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

// 该函数用于打印棋盘。数据来自status数组，里面记录了周边的雷数、是否标记了棋子等等。
// 棋盘的最大大小是50。
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

// 主函数
int main()
{
    srand((unsigned)(time(NULL))); // 随机数seeding
    
    // initialization
    int mine[52][52] = {0};       // 地雷位置
    int uncovered[52][52] = {0};  // 已翻开的部分
    int flagged[52][52] = {0};    // 已标记为地雷的部分
    int marked[52][52] = {0};     // 已标记为问号的部分
    int numdistb[52][52] = {0};   // 每个格子周边的地雷数计算
    
    int row, col, num;
    cin >> row >> col >> num;
    int nummine = num;            // 未翻开的地雷数
    
    // 输入的地雷数不应该大于棋盘大小的一半
    while (num > row * col / 2)
    {
        cout << "Too many mines! Try smaller number." << endl;
        cout << "Retype number of mines:" << endl;
        cin >> num;
    }
    cin.get();
    // 下面随机生成棋盘
    int a = 1, b = 1; // 遍历棋盘。a、b用于记录现在遍历到的位置。
    while (num > 0)
    {
        // 如果随机数满足条件，那么就可以标上地雷
        if ((rand() % (row * col)) <= num)
        {
            if (mine[a][b] != 1) // 如果已经标上过了，那么就不用再标了
            {
                mine[a][b] = 1;
                num--;
            }
        }
        
        // 下面用来处理遍历位置的移动。从左向右挨个遍历，并且处理换行、换列、回到第一个等问题。
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
    
    // 下面的部分遍历某个格子周边的八个格子，来计算这个格子周围的地雷数
    // 这里计算之后，numdistb就不用再动了，可以直接将来调用
    for (int i = 1; i <= row; i++)
    {
        for (int j = 1; j <= col; j++)
        {
            if (mine[i - 1][j - 1])
            {
                numdistb[i][j]++;
            }
            if (mine[i - 1][j])
            {
                numdistb[i][j]++;
            }
            if (mine[i - 1][j + 1])
            {
                numdistb[i][j]++;
            }
            if (mine[i][j - 1])
            {
                numdistb[i][j]++;
            }
            if (mine[i][j + 1])
            {
                numdistb[i][j]++;
            }
            if (mine[i + 1][j - 1])
            {
                numdistb[i][j]++;
            }
            if (mine[i + 1][j])
            {
                numdistb[i][j]++;
            }
            if (mine[i + 1][j + 1])
            {
                numdistb[i][j]++;
            }
        }
    }
    
    // 下面的内容仍在处理调试中。
    int status[52][52] = {0}; // Status是经过计算的结果。也就是说，需要根据布雷计算出附近的地雷数，存在status中
    for (int i = 1; i <= row; i++)
    {
        for (int j = 1; j <= col; j++)
        {
            status[i][j] = numdistb[i][j];
        }
    }
    paint(status, row, col);
    return 0;
}
