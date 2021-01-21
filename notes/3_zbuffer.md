# Z-buffer & Back face culling

## z-buffer

zbuffer是个很好理解的东西，看一下[画家算法](https://en.wikipedia.org/wiki/Painter%27s_algorithm)就知道是怎么在操作了

* 用一个缓冲区（一般用数组）存储每个像素的深度，越靠近相机（眼睛）的优先级越高

* 可以去掉看不见的像素，减少计算量

## back face culling

back face culling和zbuffer的作用差不多，都是用来裁掉看不见的那一部分，来减少计算量

但是back face culling裁掉的是三角形，它给一个面定义了正面和反面（通过顺时针和逆时针的顶点连接顺序），从相机看过去，如果是正面就着色，如果是反面就丢弃（cull）掉。具体可以看learnopengl的[教程](https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/04%20Face%20culling/)。

* 有点疑惑，zbuffer和back face culling的作用是不是重合了，而且zbuffer应该比back face culling更好用，是不是和shading方式有关系。

## code


* ```c++
  Vec3f world2screen(Vec3f v) {
    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
  }
  ```
  
  这里对x，y做了一个四舍五入的取整，不太清楚为什么，不做的话最后渲染出来的图会有很奇怪的问题。
  
  <img src="https://i.loli.net/2021/01/19/jYbg5LKtknyuCDe.png" width="50%" height="50%">

