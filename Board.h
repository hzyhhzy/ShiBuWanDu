#include <cstring>
#include <cstdint>
#include <iostream>
#include <tuple>
#define MOVE 15
#define H 6
#define W 5

template <typename Container>
auto its_and_idx(Container &&container)
{
    using std::begin, std::end;
    return std::tuple{begin(container), end(container), 0};
}
class Board
{
public:
    bool mask[H * W];   //不规则形状，false的位置代表那个地方没有表盘
    int BestMove[MOVE]; //记录最佳走法
    int MoveBuf[MOVE];  //记录正在搜的分支
    int BestValue;      //记录最佳分值

    uint8_t board[H * W]; //上右下左分别是0123

    Board() : BestValue(0)
    {
        clear();
    }
    Board(const Board &other)
    {
        memcpy(board, other.board, sizeof(board));
        memcpy(MoveBuf, other.MoveBuf, sizeof(MoveBuf));
        memcpy(BestMove, other.BestMove, sizeof(BestMove));
        memcpy(mask, other.mask, sizeof(mask));
        BestValue = other.BestValue;
    }

    void copy(const Board &other)
    {
        memcpy(board, other.board, sizeof(board));
        memcpy(MoveBuf, other.MoveBuf, sizeof(MoveBuf));
        memcpy(BestMove, other.BestMove, sizeof(BestMove));
        memcpy(mask, other.mask, sizeof(mask));
        BestValue = other.BestValue;
    }
    //将所有变量的数值都重置为0
    void clear()
    {
        memset(BestMove, 0, sizeof(BestMove));
        memset(MoveBuf, 0, sizeof(MoveBuf));
        memset(board, 0, sizeof(board));
        memset(mask, true, sizeof(mask));
    }
    int play(int x, int y); //return走了多少步

    //达到最后一步，检查是否发现更好的解
    void searchLeaf(const int index, const Board &baseboard, int minloc, int basescore);

    //递归搜索
    //baseboard 从哪个盘面开始搜
    //basescore 已经获得多少分
    //depth 还剩下多少步
    //minloc 因为交换次序不影响结果，所以可以强制要求后一步一定在前一步的后面。
    //       让minloc=前一步的位置，这样搜索下一步时直接跳过minloc前面的部分
    //maxloc 没什么用途，只是用来把完整任务分成多个子任务，便于开好几个窗口并行计算。[minloc, maxloc)左闭右开，也就是不包括maxloc
    //       除了根节点 maxloc都是H*W
    void search(const int index, const Board &baseboard, int minloc, int maxloc /*not include*/, int basescore, int depth);
};