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

#include "Board.h"
#include <fstream>
#include <thread>

using namespace std;

void InitMask(Board &board, string maskstr, string bootstr)
{
  for (int i = 0; i < maskstr.length(); i++)
  {
    board.mask[i] = maskstr[i] == '0' ? true : false;
  }

  for (int i = 0; i < bootstr.length(); i++)
  {
    board.board[i] = bootstr[i] == '0' ? 0 : bootstr[i] == '1' ? 1
                                         : bootstr[i] == '2'   ? 2
                                         : bootstr[i] == '3'   ? 3
                                                               : 0;
  }
}
bool find_answer(string maskstr, string bootstr, int start, int end, const int index, bool is_write_to_file = false)
{
  Board board = Board();
  InitMask(board, maskstr, bootstr);
  string filename = to_string(H) + "x" + to_string(W) + "_" + to_string(MOVE) + "_" + to_string(start) + "-" + to_string(end - 1) + ".txt";
  if (end == 0)
    end = H * W;
  board.search(index, board, start, end, 0, MOVE);

  cout << endl
       << "Finished!" << endl;
  cout
      << "最大步数:" << board.BestValue << " (" << board.BestValue * 90 << " 度)" << endl
      << " 路径（xy表示第x行第y列）:";
  for (int i = 0; i < MOVE; i++)
    cout << 1 + board.BestMove[i] / W << 1 + board.BestMove[i] % W << " ";
  cout << endl;
  if (is_write_to_file)
  {
    ofstream logfile(filename);
    logfile << "BestValue:" << board.BestValue << endl
            << " PV:";
    for (int i = 0; i < MOVE; i++)
    {
      logfile << 1 + board.BestMove[i] / W << 1 + board.BestMove[i] % W << " ";
      if (i + 1 == MOVE)
      {
        logfile << endl;
      }
    }
  }
  return true;
}
//启动4个find_answer线程，传入参数分别为(0,1,true);(1,2,true);(2,4,true);(4,30,true)
int main()
{
  //init mask
  string bootstr = ""
                   "00000"
                   "00000"
                   "00000"
                   "00000"
                   "00000"
                   "00000";

  string maskstr = ""
                   "00000"
                   "000x0"
                   "00000"
                   "0x000"
                   "000x0"
                   "00000";
  //第一步，先创建4个线程对象
  thread t1(find_answer, maskstr, bootstr, 0, 1, 0, true);
  // thread t2(find_answer, maskstr, bootstr, 1, 2, 1, true);
  // thread t3(find_answer, maskstr, bootstr, 2, 4, 2, true);
  // thread t4(find_answer, maskstr, bootstr, 4, 30, 3, true);

  //第二步，等待线程结束
  t1.join();
  // t2.join();
  // t3.join();
  // t4.join();
  return 0;
}

//十步万度游戏