# 工作流

### 创建Pass

1. 打开tools\make_new_render_pass.py
2. 修改create_project的参数
3. 调试程序
4. 运行setup_vs2022.bat

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

## FBO

- FBO::Desc类用于记录FBO上绑定的若干个颜色缓冲附件、和一个深度/模板缓冲附件
  - 每个附件用一个FBO::Desc::TargetDesc类示例表示

## RenderPass

- RenderGraph中的一个节点，接收上一个Pass的输入，计算后产生输出
- 在`reflect`函数中规定此Pass的输入和输出