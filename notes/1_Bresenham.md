# Lesson1

* 转置x与y的原因

  因为取样的是x，且增量固定，所以线段两端在x上的距离较短时（比在y上的距离短），需要将x，y转置（距离越长采样的点越多），画的时候再转回来就可以了

## 优化

* 偏移量（error）

  换了种划线的方法，不在用插值的方法，在性能上优化了许多（减少了计算）

  开始是这样的：

  <img src="https://i.loli.net/2020/12/27/yPbwtoQBaHg6KW8.png" width="50%" height="50%">

  优化后：

  <img src="https://i.loli.net/2020/12/27/1ydZLvYbkGSnFUw.png" width="50%" height="50%">

  就是偏移量满1了之后，往对应的方向加一（图中是y）

  最开始是0.5，因为一般认为代表像素的点是它正中心那个点，后面减1后，就是满一格再偏移了。

  * 图片来自[这里](https://zhuanlan.zhihu.com/p/156892718) 

* 抛弃浮点数（进一步优化）

  attempt 4里偏移量是这样算的

  ```c++
  float derror = std::abs(dy/float(dx)); 
  ...
  error += derror; 
  if (error>.5) { 
  	y += (y1>y0?1:-1); 
      error -= 1.; 
  }
  ```

  如果能转换成整数的计算，性能肯定会更好，浮点除法毕竟有点麻烦。

  改进方法就是：

  ```c++
  int derror2 = std::abs(dy)*2; 
  ...
  error2 += derror2; 
  if (error2 > dx) { 
  	y += (y1>y0?1:-1); 
      error2 -= dx*2; 
  } 
  ```

  相当于把每次的偏移量放大了2\*dx倍，这样所有有偏移量的地方包括累计偏移量也放大2\*dx倍就可以了，与原来的代码是等效的。

* 还有一个优化角度，循环内部的分支（if...else...）

  [this issue](https://github.com/ssloy/tinyrenderer/issues/28) 

## 代码

* ```c++
  image.set(y, x, TGAColor(255, 1));
  ```

  这里调用了另一个构造函数

  ```c++
  TGAColor(int v, int bpp) : val(v), bytespp(bpp) {
  }
  ```

  在tgaimage.h里可以看到，这个与rgba的存储方式有关

  ```c++
  union {
  	struct {
  		unsigned char b, g, r, a;
  	};
  	unsigned char raw[4];
  	unsigned int val;
  };
  int bytespp;
  ```

  可以看到用了不同的数据类型来存储颜色值，unsigned int有4个字节分别代表rgba，一个字节8位，刚好最大255

  由此可以推出rgba转一个unsigned int的方法

  假设rgba都是255

  对应二进制为00000000 00000000 00000000 11111111

  将rgba做位运算，(r<<24)+(g<<16)+(b<<8)+a，可以得到

  11111111 11111111 11111111 11111111

  实际算的时候*256也行，等价于<<8

  这个例子里画出来的线是蓝色的，估计rgba分量的存储顺序是argb

  bytespp是字节数

* ```c++
  for(int i = 0; i < 1000000; i++) {
  	line(13, 20, 80, 40, image, white);
  	line(20, 13, 40, 80, image, red);
  	line(80, 40, 13, 20, image, red);//需要改上面的set这里颜色才能生效
  }
  ```

  这里的循环不清楚是为什么,我去掉循环生成的图片是一样的,可能是让你体会时间上的差异的XD

* ```c++
  int x0 = (v0.x+1.)*width/2.;
  int y0 = (v0.y+1.)*height/2.;
  int x1 = (v1.x+1.)*width/2.;
  int y1 = (v1.y+1.)*height/2.;
  ```

  这里应该是根据生成的大小做了调整,但是不清楚原因

* 教程里生成模型网格的时候,没有用优化的划线方法,我试了一下,两个结果差不多

  <img src="https://i.loli.net/2021/01/05/BelSyj9pLHtEV7u.png" width="50%" height="50%">

  放大到一个角落对比了一下,蓝色是优化方法画出的,白色就是简单的插值

  但是在生成时间上肯定是有区别的

