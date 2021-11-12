#include "Board.h"

int Board::play(int x, int y) //return走了多少步
{
    int movenum = 0;
    while (1)
    {
        //TODO 这地方可以加上不规则形状的判断
        movenum++;
        int16_t loc = y * W + x;
        uint8_t c = (board[loc] + 1) & 3; // x+1 mod 4
        board[loc] = c;
        if (c == 0)
        {
            if (y == 0)
                break;
            y--;
        }
        else if (c == 1)
        {
            if (x == W - 1)
                break;
            x++;
        }
        else if (c == 2)
        {
            if (y == H - 1)
                break;
            y++;
        }
        else //if (c == 3)
        {
            if (x == 0)
                break;
            x--;
        }
    }
    return movenum;
}

void Board::searchLeaf(const Board &baseboard, int minloc, int basescore)
{
    Board b;
    int minx = minloc % W, miny = minloc / W;
    for (int y = miny; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            if (y == miny && x < minx)
                continue; //ensure x+y*W<=minloc
            b.copy(baseboard);
            int score = basescore + b.play(x, y);
            if (score > BestValue)
            {
                BestValue = score;
                MoveBuf[MOVE - 1] = x + y * W;
                for (int i = 0; i < MOVE; i++)
                    BestMove[i] = MoveBuf[i];
                std::cout << "New Record:" << BestValue << " PV:";
                for (int i = 0; i < MOVE; i++)
                    std::cout << 1 + BestMove[i] / W << 1 + BestMove[i] % W << " ";
                std::cout << std::endl;
            }
        }
    }
}

void Board::search(const Board &baseboard, int minloc, int maxloc /*not include*/, int basescore, int depth)
{
    if (depth == MOVE - 2)
    {
        //显示正在搜的第一步和第二步的位置
        //假如H*W=30，进度条的顺序就是0_0 0_1 .... 0_29 1_1 1_2 ... 1_29 2_2 .....28_28 28_29 29_29
        //越往后越快，在这个例子中4_0之后只有1/7的时间
        std::cout << "进度条:" << MoveBuf[0] << "_" << MoveBuf[1] << std::endl;
    }
    if (depth == 1)
    {
        searchLeaf(baseboard, minloc, basescore);
        return;
    }
    Board b;
    for (int loc = minloc; loc < maxloc; loc++)
    {
        b.copy(baseboard);
        int score = basescore + b.play(loc % W, loc / W);
        MoveBuf[MOVE - depth] = loc;
        search(b, loc, H * W, score, depth - 1);
    }
}