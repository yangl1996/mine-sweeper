#include <iostream>
#include <ctime>
#include <cstdlib>
#include <termios.h>
#include <stdio.h>
#include <fstream>
#include <cmath>


using namespace std;

// 全局变量
int uncovered[52][52] = {0};  // 已翻开的部分（需要在递归中进行操作）
bool firstuncover = true; // 递归时用于记录这是不是第一次翻开
int g_row = 10, g_col = 10, g_mine = 10;

// 用于显示帮助
void dispHelp()
{
    system("clear");
    cout << "             MINE SWEEPER" << endl;
    cout << "Welcome to Mine Sweeper! These are the" << endl;
    cout << "commands you will use durning the play." << endl << endl;
    cout << "Up key     - Move the cursor up" << endl;
    cout << "Down key   - Move the cursor down" << endl;
    cout << "Left key   - Move the cursor left" << endl;
    cout << "Right key  - Move the cursor right" << endl;
    cout << "F          - Flag the current cell" << endl;
    cout << "V          - Questionmark the current cell" << endl;
    cout << "C          - Uncover the cell" << endl;
    cout << "Q          - Quit the current game" << endl;
    cout << "S          - Save current game" << endl;
    cout << "L          - Load game" << endl << endl;
    cout << "    ...Press any key to return...";
    
    // 用于控制输出不需要回车
    struct termios stored_settings;
    struct termios new_settings;
    tcgetattr (0, &stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;
    tcsetattr (0, TCSANOW, &new_settings);
    
    getchar();
    
    // 结束控制输出不需要回车
    tcsetattr (0, TCSANOW, &stored_settings);
    
    return;
}

// 存盘功能
int saveGame(int (*uncovered)[52], int (*numdistb)[52], int (*mine)[52], int (*flagged)[52], int (*marked)[52], int nummine, int cur_row, int cur_col, int col, int row, long startTime, int currentleft, int usedflag, long currentTime)
{
    // 打开文件
    ofstream archivefile ("ms-archive.txt");
    int temp;
    if (archivefile.is_open()) // 判断是否成功打开文件
    {
        archivefile << row << endl << col << endl;
        
        for (int i = 0; i <= 51; i++)
        {
            for (int j = 0; j <= 51; j++)
            {
                temp = uncovered[i][j];
                archivefile << temp << endl;
            }
        }
        
        for (int i = 0; i <= 51; i++)
        {
            for (int j = 0; j <= 51; j++)
            {
                temp = numdistb[i][j];
                archivefile << temp << endl;
            }
        }
        
        for (int i = 0; i <= 51; i++)
        {
            for (int j = 0; j <= 51; j++)
            {
                temp = mine[i][j];
                archivefile << temp << endl;
            }
        }
        
        for (int i = 0; i <= 51; i++)
        {
            for (int j = 0; j <= 51; j++)
            {
                temp = flagged[i][j];
                archivefile << temp << endl;
            }
        }
        
        for (int i = 0; i <= 51; i++)
        {
            for (int j = 0; j <= 51; j++)
            {
                temp = marked[i][j];
                archivefile << temp << endl;
            }
        }
        
        archivefile << nummine << endl;
        archivefile << cur_row << endl << cur_col << endl;
        archivefile << startTime << endl;
        archivefile << currentleft << endl << usedflag << endl;
        archivefile << currentTime << endl;
        
        archivefile.close();
        return 1;
    }
    else
    {
        archivefile.close();
        return 0;
    }
}

// 用于设置难度
void setHard()
{
    system("clear");
    cout << "Set the size of the checkerboard" << endl;
    cout << "Row: ";
    cin >> g_row;
    while (g_row <= 3)
    {
        system("clear");
        cout << "Too small checkerboard!" << endl;
        cout << "Retype the row size: " << endl;
        cin >> g_row;
    }
    system("clear");
    cout << "Set the size of the checkerboard" << endl;
    cout << "Col: ";
    cin >> g_col;
    while (g_col <= 3)
    {
        system("clear");
        cout << "Too small checkerboard!" << endl;
        cout << "Retype the column size: " << endl;
        cin >> g_col;
    }
    system("clear");
    cout << "Set the quantity of mines" << endl;
    cout << "Quantity of mines: ";
    cin >> g_mine;
    
    // 输入的地雷数不应该大于棋盘大小的一半
    while ((g_mine > g_col * g_row / 2) || (g_mine <= 0))
    {
        if (g_mine <= 0)
        {
            system("clear");
            cout << "The quantity can't be zero!" << endl;
            cout << "Retype the quantity of mines: " << endl;
            cin >> g_mine;
        }
        else if (g_mine > g_col * g_row / 2)
        {
            system("clear");
            cout << "Too many mines! Try a smaller number." << endl;
            cout << "Retype the quantity of mines: " << endl;
            cin >> g_mine;
        }
    }
    
    system("clear");
    cout << "Hardness set!" << endl;
    cout << "Checkerboard size: " << g_row << "*" << g_col << endl;
    cout << "Quantity of mines: " << g_mine << endl << endl;
    cout << "...Press any key to return..." << endl;
    
    // 用于控制输出不需要回车
    struct termios stored_settings;
    struct termios new_settings;
    tcgetattr (0, &stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;
    tcsetattr (0, TCSANOW, &new_settings);
    
    getchar();
    getchar();
    
    // 结束控制输出不需要回车
    tcsetattr (0, TCSANOW, &stored_settings);
    
    return;
}



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

// 游戏主函数
int game()
{
    srand((unsigned)(time(NULL))); // 随机数seeding
    
    // initialization
    int mine[52][52] = {0};       // 地雷位置
    int flagged[52][52] = {0};    // 已标记为地雷的部分
    int marked[52][52] = {0};     // 已标记为问号的部分
    int numdistb[52][52] = {0};   // 每个格子周边的地雷数计算
    
    int row, col, num;
    
    // 把全局变量放入函数里
    row = g_row;
    col = g_col;
    num = g_mine;

    
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
    
    long startTime = time(NULL); // 记下开始时的时间
    
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
        
        // 用于确认是否需要退出游戏
        if (comm == 113)
        {
            system("clear");
            cout << "Are you sure to quit? (Y / N)" << endl;
            int choose = 0;
            while (choose != 121 && choose != 110)
            {
                choose = getchar();
                if (choose == 121)
                {
                    return 0;
                }
                else if (choose == 110)
                {
                    break;
                }
            }
        }
        
        switch (comm)
        {
            case 115:            // Save
            {
                cout << endl;
                if (saveGame(uncovered, numdistb, mine, flagged, marked, nummine, cursor_a, cursor_b, col, row, startTime, currentLeft, usedflag, time(NULL)))
                {
                    cout << "Successfully Saved";
                }
                else
                {
                    cout << "Error Saving!";
                }
                getchar();
                break;
            }
            case 108:            // Load
            {
                cout << endl;
                // 打开文件
                ifstream archivefile ("ms-archive.txt");
                if (archivefile.is_open()) // 判断是否成功打开文件
                {
                    archivefile >> row >> col;
                    
                    for (int i = 0; i <= 51; i++)
                    {
                        for (int j = 0; j <= 51; j++)
                        {
                            archivefile >> uncovered[i][j];
                        }
                    }
                    
                    for (int i = 0; i <= 51; i++)
                    {
                        for (int j = 0; j <= 51; j++)
                        {
                            archivefile >> numdistb[i][j];
                        }
                    }
                    
                    for (int i = 0; i <= 51; i++)
                    {
                        for (int j = 0; j <= 51; j++)
                        {
                            archivefile >> mine[i][j];
                        }
                    }
                    
                    for (int i = 0; i <= 51; i++)
                    {
                        for (int j = 0; j <= 51; j++)
                        {
                            archivefile >> flagged[i][j];
                        }
                    }
                    
                    for (int i = 0; i <= 51; i++)
                    {
                        for (int j = 0; j <= 51; j++)
                        {
                            archivefile >> marked[i][j];
                        }
                    }
                    
                    archivefile >> nummine;
                    archivefile >> cursor_a >> cursor_b;
                    archivefile >> startTime;
                    archivefile >> currentLeft >> usedflag;
                    long currentTime;
                    archivefile >> currentTime;
                    startTime = time(NULL) - abs(currentTime - startTime);
                    
                    archivefile.close();
                    
                    cout << "Loaded" << endl;
                }
                else
                {
                    archivefile.close();
                    cout << "Error loading" << endl;
                }
                getchar();
                break;
                
            }
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
                    if ((usedflag < nummine) && (!uncovered[cursor_a][cursor_b])) // 标记地雷数不能超过总
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
        cout << "GAME OVER!" << endl;
    }
    else
    {
        // 计算所用的时间
        long endTime = time(NULL);
        long usedTime = endTime - startTime;
        long usedHour = usedTime / 3600;
        long usedMin = (usedTime % 3600) / 60;
        long usedSec = usedTime - 3600 * usedHour - 60 * usedMin;
        
        // 输出游戏胜利
        cout << endl << "YOU WIN!" << endl;
        
        // 输出所用的时间
        cout << "Used time: ";
        if (usedHour == 0)
        {
            if (usedMin == 0)
            {
                cout << usedSec << " seconds" << endl << endl;
            }
            else
            {
                cout << usedMin << ":" << usedSec << endl << endl;
            }
        }
        else
        {
            cout << usedHour << ":" << usedMin << ":" << usedSec << endl << endl;
        }
    }
    cout << "...Press any key to return..." << endl;
    getchar();
    
    // 结束控制输出不需要回车
    tcsetattr (0, TCSANOW, &stored_settings);
    return 0;
}


// 主菜单
int main()
{
    while (1)
    {
        // 显示菜单
        system("clear");
        cout << "MINE SWEEPING" << endl << endl;
        cout << "    MENU" << endl;
        cout << "1 - PLAY GAME" << endl;
        cout << "2 - SET HARDNESS" << endl;
        cout << "3 - HELP" << endl;
        cout << "4 - EXIT" << endl;
    
        // 用于控制输出不需要回车
        struct termios stored_settings;
        struct termios new_settings;
        tcgetattr (0, &stored_settings);
        new_settings = stored_settings;
        new_settings.c_lflag &= (~ICANON);
        new_settings.c_cc[VTIME] = 0;
        new_settings.c_cc[VMIN] = 1;
        tcsetattr (0, TCSANOW, &new_settings);
    
        // 进行菜单操作
        int sel = 0;
        sel = getchar();
        switch (sel)
        {
            case 49:
            {
                system("clear");
                game();
                break;
            }
            case 50:
            {
                system("clear");
                tcsetattr (0, TCSANOW, &stored_settings);
                setHard();
                break;
            }
            case 51:
            {
                system("clear");
                dispHelp();
                break;
            }
            case 52:
            {
                system("clear");
                return 0;
            }
            
        }
    
        // 结束控制输出不需要回车
        tcsetattr (0, TCSANOW, &stored_settings);
    }
    return 0;
}