#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

const int H = 6, W = 5, MOVE = 15 ;//高，宽，步数
//初始盘面，0上，1右，2下，3左
const string boardstr = ""
"00000"
"00000"
"00000"
"00000"
"00000"
"00000"
;

//是否有表盘，0有，x没有
const string maskstr = ""
"00000"
"00000"
"00000"
"00000"
"00000"
"00000"
;

bool fullsearch = false;//=true则跳过最开始的输入
bool useinit = false;//=true则强制使用指定开局，跳过最开始的输入

int main_strictSolve();//严格求解
int main_fuzzySolve();//模糊求解，同一个点重复很多次，然后再点别的点。因为我观察到很多最优解都是某个点被点击非常多次。
int main()
{
  return main_fuzzySolve();
}


class Board
{
public:
  static bool mask[H * W];//不规则形状，false的位置代表那个地方没有表盘
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
      //检查一下是不是到了不存在表盘的位置
      if (!mask[x + y * W])break;

    }
    return movenum;
  }

  //达到最后一步，检查是否发现更好的解
  static void searchLeaf(const Board& baseboard, int minloc, int basescore)
  {
    Board b;
    int minx = minloc % W, miny = minloc / W;
    for (int y = miny; y < H; y++)
      for (int x = 0; x < W; x++)
      {
        if (y == miny && x < minx)continue;//ensure x+y*W<=minloc
        if (!mask[x + y * W])continue;//无表盘的位置
        b.copy(baseboard);
        int score = basescore + b.play(x, y);
        if (score > BestValue) {
          BestValue = score;
          MoveBuf[MOVE - 1] = x + y * W;
          for (int i = 0; i < MOVE; i++)BestMove[i] = MoveBuf[i];
          cout << "New Record:" << BestValue << " PV:";
          for (int i = 0; i < MOVE; i++)cout << 1 + BestMove[i] / W << 1 + BestMove[i] % W << " ";
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
      cout << "进度条:" << MoveBuf[0] << "_" << MoveBuf[1] << endl;
    }
    if (depth == 1)
    {
      searchLeaf(baseboard, minloc, basescore);
      return;
    }
    Board b;
    for (int loc = minloc; loc < maxloc; loc++)
    {
      if (!mask[loc])continue;//无表盘的位置
      b.copy(baseboard);
      int score = basescore + b.play(loc % W, loc / W);
      MoveBuf[MOVE - depth] = loc;
      search(b, loc, H * W, score, depth - 1);
    }
  }

};

int Board::BestValue = 0;
int Board::BestMove[MOVE];
int Board::MoveBuf[MOVE];
bool Board::mask[H * W];
int main_strictSolve()
{
  Board board;
  int start = 0, end = 0;
  cout << "HZY 2021.11.12" << endl;
  cout << "长宽和步数都在代码里直接设置，设置好之后编译" << endl;
  if (!fullsearch)
  {
    cout << "如果进行完整搜索，以下两步直接输入0。如果任务量很大需要开多个窗口并行，才需要考虑下面两步。具体说明在代码注释中" << endl << endl;
    cout << "第一步从第几个点开始搜（输入后按回车）:";
    cin >> start;
    cout << "搜到第几个点（不包含，左闭右开）（0表示全搜）:";
    cin >> end;
  }
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



  bool hasInitialBoard = true;
  if (!useinit)
  {
    cout << "是否指定了初始盘面或者表盘空缺处(yes/no)";
    while (1)
    {
      string initialBoard_str;
      cin >> initialBoard_str;
      if (initialBoard_str == "yes")
      {
        hasInitialBoard = true;
        break;
      }
      else if (initialBoard_str == "no")
      {
        hasInitialBoard = false;
        break;
      }
      else cout << "请输入yes或者no: ";
    }
  }

  if (hasInitialBoard)
  {
    if (boardstr.length() != H * W || maskstr.length() != H * W)
    {
      cout << boardstr.length() << "给定的初始盘面或者表盘空缺处和设置的棋盘大小不同";
      return 0;
    }
    for (int i = 0; i < H * W; i++)
    {
      board.board[i] = boardstr[i] - '0';
      Board::mask[i] = (maskstr[i] == '0');
    }
  }
  else
  {
    for (int i = 0; i < H * W; i++)
    {
      board.board[i] = 0;
      Board::mask[i] = true;
    }
  }







  Board::search(board, start, end, 0, MOVE);


  cout << endl << endl << endl << "Finished!" << endl << endl;
  cout << "以下路径中\"xy\"指的是第x行第y列的位置" << endl;
  cout << "最大步数:" << Board::BestValue << " (" << Board::BestValue * 90 << " 度)  路径: ";
  for (int i = 0; i < MOVE; i++)cout << 1 + Board::BestMove[i] / W << 1 + Board::BestMove[i] % W << " ";
  cout << endl;

  string filename = to_string(H) + "x" + to_string(W) + "_" + to_string(MOVE) + "_" + to_string(start) + "-" + to_string(end - 1) + ".txt";
  if (hasInitialBoard)filename = boardstr + "__" + maskstr + "__" + filename;
  ofstream logfile(filename);
  logfile << "BestValue:" << Board::BestValue << endl << " PV:";
  for (int i = 0; i < MOVE; i++)logfile << 1 + Board::BestMove[i] / W << 1 + Board::BestMove[i] % W << " ";


  return 0;
}

int main_fuzzySolve()
{
  //当MOVE过大时，没法穷举
  //假设只有一个点出现频率高，则让前十几步只走这一个点，最后几步穷举

  int finalMoves =9;//穷举几步
  if (finalMoves > H * W - 1)finalMoves = H * W - 1;
  int fixedMoves = MOVE - finalMoves;

  Board initialboard;
  int start = 0, end = 0;
  cout << "HZY 2021.11.12" << endl;
  cout << "长宽和步数都在代码里直接设置，设置好之后编译" << endl;
  if (!fullsearch)
  {
    cout << "如果进行完整搜索，以下两步直接输入0。如果任务量很大需要开多个窗口并行，才需要考虑下面两步。具体说明在代码注释中" << endl << endl;
    cout << "第一步从第几个点开始搜（输入后按回车）:";
    cin >> start;
    cout << "搜到第几个点（不包含，左闭右开）（0表示全搜）:";
    cin >> end;
  }
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



  bool hasInitialBoard = true;
  if (!useinit)
  {
    cout << "是否指定了初始盘面或者表盘空缺处(yes/no)";
    while (1)
    {
      string initialBoard_str;
      cin >> initialBoard_str;
      if (initialBoard_str == "yes")
      {
        hasInitialBoard = true;
        break;
      }
      else if (initialBoard_str == "no")
      {
        hasInitialBoard = false;
        break;
      }
      else cout << "请输入yes或者no: ";
    }
  }

  if (hasInitialBoard)
  {
    if (boardstr.length() != H * W || maskstr.length() != H * W)
    {
      cout << boardstr.length() << "给定的初始盘面或者表盘空缺处和设置的棋盘大小不同";
      return 0;
    }
    for (int i = 0; i < H * W; i++)
    {
      initialboard.board[i] = boardstr[i] - '0';
      Board::mask[i] = (maskstr[i] == '0');
    }
  }
  else
  {
    for (int i = 0; i < H * W; i++)
    {
      initialboard.board[i] = 0;
      Board::mask[i] = true;
    }
  }







  for (int loc0 = 0; loc0 <H*W; loc0++)
  {
    int score = 0;
    Board board(initialboard);
    for (int i = 0; i < fixedMoves; i++)
    {
      score+=board.play(loc0 % W, loc0 / W);
      Board::MoveBuf[i] = loc0;
    }
    Board::search(board, start, end, score, finalMoves);

  }

  cout << endl << endl << endl << "Finished!" << endl << endl;
  cout << "以下路径中\"xy\"指的是第x行第y列的位置" << endl;
  cout << "最大步数:" << Board::BestValue << " (" << Board::BestValue * 90 << " 度)  路径: ";
  for (int i = 0; i < MOVE; i++)cout << 1 + Board::BestMove[i] / W << 1 + Board::BestMove[i] % W << " ";
  cout << endl;

  string filename ="Fuzzy_" + to_string(H) + "x" + to_string(W) + "_" + to_string(MOVE) + "_" + to_string(start) + "-" + to_string(end - 1) + ".txt";
  if (hasInitialBoard)filename = boardstr + "__" + maskstr + "__" + filename;
  ofstream logfile(filename);
  logfile << "BestValue:" << Board::BestValue << endl << " PV:";
  for (int i = 0; i < MOVE; i++)logfile << 1 + Board::BestMove[i] / W << 1 + Board::BestMove[i] % W << " ";



  return 0;
}