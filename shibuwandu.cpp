

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
const int H = 5, W = 5, MOVE = 10;//高，宽，步数
using namespace std;
class Board
{
public:
  static int BestMove[MOVE];//记录最佳走法
  static int MoveBuf[MOVE];//记录正在搜的分支
  static int BestValue;//记录最佳分值


  uint8_t board[H * W];//上右下左分别是0123
  Board() { clear(); }
  Board(const Board& other) { memcpy(board, other.board, H * W); }
  void copy(const Board& other) { memcpy(board, other.board, H * W); }
  void clear()
  {
    for (int i = 0; i < H * W; i++)board[i] = 0;
  }
  int play(int x, int y)//return走了多少步
  {
    int movenum = 0;
    while (1)
    {
      //TODO 这地方可以加上不规则形状的判断
      movenum++;
      int16_t loc = y * W + x;
      uint8_t c = (board[loc] + 1) & 3;// x+1 mod 4
      board[loc] = c;
      if (c == 0) {
        if (y == 0)break;
        y--;
      }
      else if (c == 1) {
        if (x == W - 1)break;
        x++;
      }
      else if (c == 2) {
        if (y == H - 1)break;
        y++;
      }
      else //if (c == 3) 
      {
        if (x == 0)break;
        x--;
      }

    }
    return movenum;
  }
  
  //达到最后一步，检查是否发现更好的解
  static void searchLeaf(const Board& baseboard,int minloc,int basescore)  
  {
    Board b;
    int minx = minloc % W, miny = minloc / W;
    for (int y = miny; y < H; y++)
    for (int x = 0; x < W; x++)
      {
      if (y == miny && x < minx)continue;//ensure x+y*W<=minloc
        b.copy(baseboard);
        int score =basescore+ b.play(x, y);
        if (score > BestValue) {
          BestValue = score;
          MoveBuf[MOVE - 1] = x + y * W;
          for (int i = 0; i < MOVE; i++)BestMove[i] = MoveBuf[i];
          cout << "New Record:" << BestValue << " PV:";
          for (int i = 0; i < MOVE; i++)cout << 1 + BestMove[i] / W << 1 + BestMove[i] % W  << " ";
          cout << endl;
        }
      }
  }
  
  //递归搜索
  //baseboard 从哪个盘面开始搜
  //basescore 已经获得多少分
  //depth 还剩下多少步
  //minloc 因为交换次序不影响结果，所以可以强制要求后一步一定在前一步的后面。
  //       让minloc=前一步的位置，这样搜索下一步时直接跳过minloc前面的部分
  //maxloc 没什么用途，只是用来把完整任务分成多个子任务，便于开好几个窗口并行计算。[minloc, maxloc)左闭右开，也就是不包括maxloc
  //       除了根节点 maxloc都是H*W
  static void search(const Board& baseboard, int minloc, int maxloc/*not include*/, int basescore, int depth)
  {
    if (depth == MOVE - 2)
    {
      //显示正在搜的第一步和第二步的位置
      //假如H*W=30，进度条的顺序就是0_0 0_1 .... 0_29 1_1 1_2 ... 1_29 2_2 .....28_28 28_29 29_29
      //越往后越快，在这个例子中4_0之后只有1/7的时间
      cout <<"进度条:"<< MoveBuf[0] << "_" << MoveBuf[1] << endl;
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
      search(b, loc,H*W, score, depth - 1);
    }
  }

};

int Board::BestValue = 0;
int Board::BestMove[MOVE];
int Board::MoveBuf[MOVE];

int main()
{
  Board board;
  int start, end;
  cout << "HZY 2021.11.12" << endl;
  cout << "长宽和步数都在代码里直接设置，设置好之后编译" << endl;
  cout << "如果进行完整搜索，以下两步直接输入0。如果任务量很大需要开多个窗口并行，才需要考虑下面两步。具体说明在代码注释中" << endl;
  cout << "第一步从第几个点开始搜:";
  cin >> start;
  cout << "搜到第几个点（不包含，左闭右开）（0表示全搜）:";
  cin >> end;
  //start和end表示第一步的搜索范围
  //如果原问题规模较小，比如H=5 W=5 MOVE=10，直接输入两个0
  //如果原问题耗时较长，建议手动多线程，方法如下。
  //比如，H=6 W=5 MOVE=15，可以开4个窗口，分别输入"0 1"、"1 2"、"2 4"、"4 30"(或者"4 0",0会被自动转化为30)
  //这样四个窗口的任务量比较接近，四个窗口两个小时内都能解决。最后会保存文本文件，看一下哪个最多。
  // TODO 可以写一个智能一点的多线程，现在这个太笨蛋了
  // 
  //程序耗时计算：每个线程每秒可以穷举15M(1500万)个路径。
  //总路径数：C(MOVE, H*W+MOVE-1),C表示组合数,C(x,y)=y!/(x!*(y-x)!)
  //例如，高6宽5 15步，总路径数为C(15,44)=229911617056=230G，一个线程大概4个小时可以穷举完。

  if (end == 0)end = H * W;
  Board::search(board, start,end, 0, MOVE);


  cout << endl << endl << endl << "Finished!" << endl << endl;
  cout << "最大步数:" << Board::BestValue << " (" << Board::BestValue*90 << " 度)" <<endl<< " 路径（xy表示第x行第y列）:";
  for (int i = 0; i < MOVE; i++)cout << 1 + Board::BestMove[i] / W << 1 + Board::BestMove[i] % W << " ";
  cout << endl;

  string filename = to_string(H) + "x" + to_string(W) + "_" + to_string(MOVE) + "_" + to_string(start) + "-" + to_string(end - 1) + ".txt";
  ofstream logfile(filename);
  logfile<< "BestValue:" << Board::BestValue<< endl << " PV:";
  for (int i = 0; i < MOVE; i++)logfile  << 1 + Board::BestMove[i] / W << 1 + Board::BestMove[i] % W << " ";

}