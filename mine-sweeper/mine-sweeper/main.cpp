#include <iostream>
#include <ctime>
#include <cstdlib>
#include <termios.h>
#include <stdio.h>


using namespace std;

int uncovered[52][52] = {0};  // 已翻开的部分，需要在多个函数中使用，存为全局变量
bool firstuncover = true; // 递归时用于记录这是不是第一次翻开

// 用于翻开棋盘
void uncover(int (*uncovered)[52], int (*numdistb)[52], int (*mine)[52], int col, int row, bool first)
{
    bool near = false;
    // 如果这是第一次翻开，那么一定翻开
    if ((uncovered[row][col] == 0) && (numdistb[row][col] != 0) && (mine[row][col] == 0) && first)
    {
        uncovered[row][col] = 1;
    }
    first = false;
    if ((uncovered[row][col] == 1) || (numdistb[row][col] != 0) || (mine[row][col] == 1))
    {
        // 下面这段判断这是不是边界：如果是边界那还可以继续输出。如果周边有翻开的空格，且不是带数字的，那就可以翻。
        near = (((uncovered[row][col - 1] == 1) && (numdistb[row][col - 1] == 0)) || ((uncovered[row - 1][col] == 1) && (numdistb[row - 1][col] == 0)) || ((uncovered[row + 1][col] == 1) && (numdistb[row + 1][col] == 0)) || ((uncovered[row][col + 1] == 1) && (numdistb[row][col + 1] == 0)) || ((uncovered[row - 1][col - 1] == 1) && (numdistb[row - 1][col - 1] == 0)) || ((uncovered[row + 1][col - 1] == 1) && (numdistb[row + 1][col - 1] == 0)) || ((uncovered[row - 1][col + 1] == 1) && (numdistb[row - 1][col + 1] == 0)) || ((uncovered[row + 1][col + 1] == 1) && (numdistb[row + 1][col + 1] == 0)));
        if (near && (numdistb[row][col] != 0) && (mine[row][col] == 0))
        {
            uncovered[row][col] = 1;
        }
        return;
    }
    else
    {
        uncovered[row][col] = 1;
    }
    uncover(uncovered, numdistb, mine, col, row - 1, first);
    uncover(uncovered, numdistb, mine, col - 1, row, first);
    uncover(uncovered, numdistb, mine, col + 1, row, first);
    uncover(uncovered, numdistb, mine, col, row + 1, first);
    uncover(uncovered, numdistb, mine, col + 1, row - 1, first);
    uncover(uncovered, numdistb, mine, col - 1, row - 1, first);
    uncover(uncovered, numdistb, mine, col + 1, row + 1, first);
    uncover(uncovered, numdistb, mine, col - 1, row + 1, first);
}

/*
 Status
 number 1-8 : 附近的地雷数
 number 0   : 附近没有地雷
 number -1  : 未翻开
 number 9   : 插上了棋子
 number 10  : 插上了问号
 number 11  : 现在的光标位置
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
        case 11:
        {
            cout << "@";
        }
    }
}

// Debugging用
void dbpaint(int (*status)[52], int col, int row)
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
            cout << status[i][j];
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
        cout << status[row][j];
        cout << "│";
    }
    cout << endl << "└";
    for (int j = 2; j <= col; j++)
    {
        cout << "─" << "┴";
    }
    cout << "─" << "┘" << endl;
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
        paintStatus(status[row][j]);
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
    int flagged[52][52] = {0};    // 已标记为地雷的部分
    int marked[52][52] = {0};     // 已标记为问号的部分
    int numdistb[52][52] = {0};   // 每个格子周边的地雷数计算
    
    
    int row, col, num;
    cin >> row >> col >> num;
    int nummine = num;            // 总地雷数
    
    // 初始化uncovered数组，把边框框起来
    for (int i = 0; i <= row + 1; i++)
    {
        for (int j = 0; j <= col + 1; j++)
        {
            uncovered[i][j] = 1;
        }
    }
    for (int i = 1; i <= row; i++)
    {
        for (int j = 1; j <= col; j++)
        {
            uncovered[i][j] = 0;
        }
    }
    
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
    
    // 下面的内容仍然在调试中
    
    int status[52][52] = {0}; // status是棋盘的现状
    int cursor_a = 1, cursor_b = 1; // cursor的位置, a是行数，b是列数
    
    // 用于控制输出不需要回车
    struct termios stored_settings;
    struct termios new_settings;
    tcgetattr (0, &stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;
    tcsetattr (0, TCSANOW, &new_settings);
    
    bool lose = false; // 失败标记
    int currentLeft = 1; // 未标记的地雷数
    int usedflag = 0;
    
    while (currentLeft > 0)
    {
        // 这部分先生成棋盘的现状
        system("clear");
        for (int i = 1; i <= row; i++)
        {
            for (int j = 1; j <= col; j++)
            {
                status[i][j] = numdistb[i][j];
            }
        }
        for (int i = 1; i <= row; i++)
        {
            for (int j = 1; j <= col; j++)
            {
                if (!uncovered[i][j])
                {
                    status[i][j] = -1;
                }
            }
        }
        for (int i = 1; i <= row; i++)
        {
            for (int j = 1; j <= col; j++)
            {
                if (flagged[i][j])
                {
                    status[i][j] = 9;
                }
                else if (marked[i][j])
                {
                    status[i][j] = 10;
                }
            }
        }
        status[cursor_a][cursor_b] = 11;
        // 棋盘状态生成完毕

        paint(status, row, col);
        // 输出地雷数
        cout << "Total: " << nummine << ", Left: " << nummine - usedflag << endl;
        // 棋盘状态输出完毕
        
        // 下面是操纵光标
        // ASDW控制方向
        // F控制插旗
        // V表示问号
        int comm;

        comm = getchar();
        
        switch (comm)
        {
            case 91:             // Cursor
            {
                comm = getchar();
                switch (comm)
                {
                    case 65: // UP
                    {
                        if (cursor_a > 1)
                        {
                            cursor_a--;
                        }
                        break;
                    }
                    case 68: // LEFT
                    {
                        if (cursor_b > 1)
                        {
                            cursor_b--;
                        }
                        break;
                    }
                    case 67: // RIGHT
                    {
                        if (cursor_b < col)
                        {
                            cursor_b++;
                        }
                        break;
                    }
                    case 66: // DOWN
                    {
                        if (cursor_a < row)
                        {
                            cursor_a++;
                        }
                        break;
                    }
                }
                break;
            }
            case 102:                // Flagging
            {
                if (flagged[cursor_a][cursor_b])
                {
                    usedflag--;
                    flagged[cursor_a][cursor_b] = !flagged[cursor_a][cursor_b];
                }
                else
                {
                    if (usedflag < nummine) // 标记的地雷数不能超过总的地雷数
                    {
                        usedflag++;
                        flagged[cursor_a][cursor_b] = !flagged[cursor_a][cursor_b];
                    }
                }
                break;
            }
            case 118:                // Marking
            {
                marked[cursor_a][cursor_b] = !marked[cursor_a][cursor_b];
                break;
            }
        }
        if (comm == 99)                 // Uncovering
        {
            if (mine[cursor_a][cursor_b]) // 踩到地雷
            {
                lose = true;
                break;
            }
            else
            {
                if (uncovered[cursor_a][cursor_b] == 0)
                {
                    firstuncover = true;
                    uncover(uncovered, numdistb, mine, cursor_b, cursor_a, firstuncover);
                }
            }
        }
        
        // 下面计算剩下的未标记的地雷数
        int correctcount = 0;
        for (int i = 1; i <= row; i++)
        {
            for (int j = 1; j <= col; j++)
            {
                if (flagged[i][j] == 1 && mine[i][j] == 1)
                {
                    correctcount++;
                }
            }
        }
        currentLeft = nummine - correctcount;
    }
    
    // 结束控制输出不需要回车
    tcsetattr (0, TCSANOW, &stored_settings);
    
    // 输出游戏结果
    system("clear");
    if (lose)
    {
        for (int i = 1; i <= row; i++)
        {
            for (int j = 1; j <= col; j++)
            {
                if (mine[i][j])
                {
                    status[i][j] = 9;
                }
            }
        }
        paint(status, col, row);
        cout << endl << "GAME OVER!" << endl;
    }
    else
    {
        cout << endl << " YOU WIN!" << endl;
    }
    
    return 0;
}