# fill triangle

## line sweeping

这个没什么好说的，就是把同一行的pixel全部填上颜色，也不用担心采样步长的问题（最后中间的部分都要被填满的，边界上离散的点看不出来的），就是在sweep的时候，要注意是分为两部分的。

<img src="https://i.loli.net/2021/01/13/7tidFKWRJpaYhkH.png" width="30%" height="30%">

## boundingbox

pseudo-code

``` c++
triangle(vec2 points[3]) { 
    vec2 bbox[2] = find_bounding_box(points); 
    for (each pixel in the bounding box) { 
        if (inside(points, pixel)) { 
            put_pixel(pixel); 
        } 
    } 
}
```

这种做法相对来说高明一点，是先找一个（尽量小的）包围盒把三角形包围起来，然后遍历包围盒里面的pixel，把在三角形里面的pixel着色就行了。至于怎么判断pixel在三角形里面，可以用求重心坐标的方法来做。

* 其实判断一个点是否在三角形里面还有一种相对简单一点的方法：

  可以用三次叉积来判断。假设有像素Q（用像素中心的点来代表像素），三角形p0p1p2

  首先算出**p0p1** × **p1pQ**，可以根据得到的向量的正负判断Q点在**p0p1**的左边还是右边，同理算出**p1p2** × **p1pQ**，**p2p0** × **p2pQ**，如何得到的三个向量同正或同负，则代表像素在三角形内。画图可以很清晰的看到，一个点在三角形内部时，将三角形的三条边看成顺序的三个向量，这个点一定是在这三个向量的同一侧的。

  详细可以看闫令琪老师的[GAMES101](https://www.bilibili.com/video/BV1X7411F744) Lecture05

  ![ft.png](https://i.loli.net/2021/01/13/xKZrCo6pUljtQ1A.png)

  右边是重心坐标，左边是三次叉积

* 教程里求重心坐标的部分

  ``` c++
  Vec3f barycentric(Vec2i *pts, Vec2i P) { 
      Vec3f u = cross(Vec3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0]), Vec3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]));
      /* `pts` and `P` has integer value as coordinates
         so `abs(u[2])` < 1 means `u[2]` is 0, that means
         triangle is degenerate, in this case return something with negative coordinates */
      //degenerate大概指的是三角形三个顶点在一条直线上
      if (std::abs(u[2])<1) return Vec3f(-1,1,1);
      return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); 
  } 
  ```

  

[u, v, 1]是[**ABx**, **ACx**, **PAx**], [**AB<sub>y</sub>**, **AC<sub>y</sub>**, **PA<sub>y</sub>**]这两个向量构成平面的法向量，将这两个向量求叉积可以得到一个向量，假设这个向量是**ans**，也就是代码中的```Vec3f u```，**ans**和[u, v, 1]必然在同一条直线上，那么将**ans**的z分量化为1后，其x和y分量就是我们要求的u和v。(同一条直线上的两个向量通过一个常数是可以转化的)感觉线代没学好，这里还纠结了好一会(X。

  不过讲道理，这个```Vec3f u```命名也蛮蛋疼的，容易和重心坐标的u，v搞混。

* 求boundingbox那里加了生成图片边界的范围检测，看起来后有点绕，其实就是求三个顶点的最大和最小的x，y的值，作为boundingbox的边界。

# fillobj

* 重心坐标与三次叉积的区别

  ![tw.png](https://i.loli.net/2021/01/14/FeoRYpmx5y1BTgS.png)

  上面有两个三角形的对比图，差别不大，但是到了复杂的几何体上就有差别了，左边是三次叉积，右边是重心坐标，三次叉积的结果在有些地方可以看出明显的边界。

  不过我想了想这个差别主要来自我将边界上的点都没算在三角形里（三次叉积的方法），如果都算的话差别应该不大。

* ```c++
  Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
  //这玩意就是叉乘
  //geometry.h里面重载了^,效果和cross是一样的，这两个用一个就行了
  //真是蛋疼，搞不懂为什么要这样
  ```

* <img src="https://i.loli.net/2021/01/15/8csxg3F7AhXG6IJ.png" alt="tw.png" style="zoom:30%;" />

  光照方向搞反了会生成一个很惊悚的东西

  那么问题来了，为什么光线方向正确生成一个假面骑士一样的嘴巴，而反了会出现个这么吓人的玩意？原因就出在没考虑三角形的覆盖顺序，z-buffer可以用来解决这个问题。