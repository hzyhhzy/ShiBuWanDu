#include <cstring>
#include <cstdint>
#include <iostream>
#define MOVE 15
#define H 6
#define W 5
class Board
{
public:
    static int BestMove[MOVE]; //记录最佳走法
    static int MoveBuf[MOVE];  //记录正在搜的分支
    static int BestValue;      //记录最佳分值

    uint8_t board[H * W]; //上右下左分别是0123
    Board() { clear(); }
    Board(const Board &other) { memcpy(board, other.board, H * W); }
    void copy(const Board &other) { memcpy(board, other.board, H * W); }
    void clear()
    {
        for (int i = 0; i < H * W; i++)
            board[i] = 0;
    }
    int play(int x, int y); //return走了多少步

    //达到最后一步，检查是否发现更好的解
    static void searchLeaf(const Board &baseboard, int minloc, int basescore);

    //递归搜索
    //baseboard 从哪个盘面开始搜
    //basescore 已经获得多少分
    //depth 还剩下多少步
    //minloc 因为交换次序不影响结果，所以可以强制要求后一步一定在前一步的后面。
    //       让minloc=前一步的位置，这样搜索下一步时直接跳过minloc前面的部分
    //maxloc 没什么用途，只是用来把完整任务分成多个子任务，便于开好几个窗口并行计算。[minloc, maxloc)左闭右开，也就是不包括maxloc
    //       除了根节点 maxloc都是H*W
    static void search(const Board &baseboard, int minloc, int maxloc /*not include*/, int basescore, int depth);
};