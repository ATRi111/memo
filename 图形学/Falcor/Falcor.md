# API

## Object

- 各种渲染对象的父类
- 有一个引用计数器（线程安全的）

## ref

- 类似`std::shared_ptr`，但只能用在Object子类实例上

## BreakableRenference

- 类似`std::weak_ptr`，用于避免循环引用问题

## VAO

- 包含VBO，VBO即若干个Buffer构成的数组
- 没有EBO（相当于总是使用[1,2,3,4,...]这样的EBO）；通过Topology规定顶点构成图元的方式