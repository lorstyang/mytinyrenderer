# Transformation

* 向量/几何体的变换
* 坐标（系）变换
* 左手右手坐标系的概念
* 正交/透视投影
* 齐次坐标等等

还是GAMES101[闫令琪老师的课](https://www.bilibili.com/video/BV1X7411F744) P2-P4，只需要一点线性代数知识即可

我做的[笔记](https://lorstyang.github.io/2020/10/19/Games101-Transformation/)

## moving the camera

可能习惯性的会想到之前相机是放哪的，其实之前没有东西来确定放在哪，但是之前是确定了由世界坐标系到屏幕坐标系的转换规则的，这也变相的确定了相机位置。

![movingcamera.png](https://i.loli.net/2021/01/20/yb4muwcqZV1JIfH.png)

## code

* 引入了新的计算方式，geometry.h需要进行对应的修改，以满足矩阵的运算。

* ```c++
  void viewport(int x, int y, int w, int h)
  ```

  这个是把[-1, 1]^3的立方体映射到[x, x + w]\*[y, y + h]\*[0, d]上，本来屏幕是[0, width]*[0, height]，这样做估计是为了做一定的调整，至于z轴，作者用的d是255（好像是可以这样做zbuffer的灰度图还是怎么来着），其实不用太管它，设为1就行了。

  (w == width h == height)

  viewport矩阵

  如果按照上面的说法，推导出来是这样的
  $$
  M_{viewport} = \left[ \begin{matrix} w/2 & 0 & 0 & x+w/2 \\ 0 & h/2 & 0 & y+h/2 \\ 0 & 0 & d/2 & d/2 \\ 0 & 0 & 0 & 1 \end{matrix}\right]
  $$
  如果只是[-1, 1]^2 --> [0, w]*[0, h]
  $$
  M_{viewport} = \left[ \begin{matrix} w/2 & 0 & 0 & x+w/2 \\ 0 & h/2 & 0 & y+h/2 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{matrix}\right]
  $$

* ```c++
  void projection(float coeff) {
  	Projection = Matrix::identity();
  	Projection[3][2] = coeff;
  }
  ```

  这个投影只用了一种非常简单的透视投影，就是将所有的点投影到z为0的xy平面上，根据相似三角形可以得出两个点的关系，进而得到投影矩阵
  
  camera放在C点(0, 0, c)
  $$
M_{projection} = \left[ \begin{matrix} 1 & 0 & 0 & 0 \\ 0 & 1 & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & -1/c & 1 \end{matrix}\right]
  $$
  放在其他位置可能求出来不是-1/c，我在代码里随便设了个值，结果上来看影响不大
  
* 因为模型的顶点数据是本来就在[-1, 1]之间，所以教程里的projection matrix没有把视锥体转换成[-1, 1]^3的立方体，但是GAMES101里是没有默认这点的，所以推导出来的projection matrix大不相同。

