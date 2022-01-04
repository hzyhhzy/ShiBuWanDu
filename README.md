# ShiBuWanDu
”十步万度“穷举求解器   
如果不知道什么是“十步万度”，随便搜一下就知道了。   

   
## 使用说明
长宽和步数还有初始盘面和表盘空缺处都在代码里直接设置，设置好之后编译   

有严格求解main_strictSolve()和模糊求解main_fuzzySolve()的功能
模糊求解是强制某个点点击很多次，剩下几步穷举。
int finalMoves =9是最后穷举几步，前面的步骤都是点击同一个点。
   
打开程序后会提示输入两个数字。如果问题规模较小，直接输入两个0。两个0表示完整搜索。   
如果任务量很大需要开多个窗口并行，才需要考虑那两个数字。具体说明如下   
   
输入的两个数字表示第一步的搜索范围，左闭右开。   
比如输入0 10，代表第一步从0搜到9。如果第二个数字是0，代表搜到最后一个。   
问题规模较大时，可以手动“多线程”，开几个窗口手动分配工作量。   
例：H=6 W=5 MOVE=15，可以开4个窗口，分别输入"0 1"、"1 2"、"2 4"、"4 30"(或者"4 0",0会被自动转化为30)。   
这样四个窗口的任务量比较接近，四个窗口两个小时内都能解决。最后会保存文本文件，看一下哪个最多。  
   
最后输入一个yes或者no，yes使用代码里的初始表盘位置，no假设所有位置都有表盘且都朝上。   
   
TODO 可以写一个智能一点的多线程，现在这个太笨蛋了   
   
## 原理
“十步万度”的步骤任意交换后结果不变，因此最终结果只取决于每个表盘点了多少次。
为什么不变呢？并不显然，而且很反直觉，严谨证明应该也不是一两句话的事。如果谁有特别巧妙的严谨证明可以跟我说一下，比如发个issue或者pull request或者直接加我qq 2658628026。   

写程序时，可以要求编号小的表盘一定先点，也就是后一个表盘的编号不得小于前一个表盘，这样一个很简单的递归就搞定了。   

这是个典型的高中经常讲的“有放回的抓取问题”。   
假如有30个表盘，点击15次，那么路线总数为：C(15,30+15-1) = C(15,44) = 44!/15!/29! = 229911617056 = 2.3e11 = 2299亿     
2299亿个情况，家用机很快就能解决。我自己的电脑每个线程每秒穷举15M(1500万)个情况，开了4个窗口，不超过2小时。   

## 讨论
感觉“可交换”是个非常神奇的性质，我怀疑这个东西背后有一些更本质的东西，但是我数学水平太差，也没学过群论之类的。期待某个数学大佬来研究一下这个问题。   
   
这个问题的一些变形也无法用这种方法，还有待研究。比如，如何找到一个初始局面，使得点一次转的步数最多？   
   
一些其他的发现：对于4x4的盘面，持续点击几乎任何一个点（我只是挑了几个试了一下，都是6600），都是6600次回到初态。对于任意大小的盘面都有这个性质吗？对于H乘W的盘面又是多少次？   
容易证明，点击后的局面，在走过的路径上一定没有“环”。有多少局面是无环的？任意一个无环的初始状态，点击一个点足够多次都可以回到初态吗？   
    
4x4有557568000个局面无环，这个数字也是6600的倍数。感觉应该是某种“群”


***注：点击多少次可以回到初始状态的问题已经解决***   
设邻接矩阵是A，设每个表盘转了x[i]次，设每个表盘点击了b[i]次。若回到初态，则满足x=b+Ax/4且b为整数且x的每一项均为4的倍数。因此可以求(4I-A)的逆，取每一项的分母，按行求最大公倍数。点击次数是最大公倍数。    
    
    
## 其他
以前我做过的一些同类型的问题：   
单人跳棋：最少27步，状态数26亿  https://github.com/hzyhhzy/tiaoqimeiju   
浅塘：最多51步，状态数（所有可能的“关卡”）270亿，如果把出口位置挪到第二行则存在一个64步的局面    https://github.com/hzyhhzy/QianTangSolver   


