# 工作流

### 创建Pass

1. 打开tools\make_new_render_pass.py
2. 修改create_project的参数
3. 调试py程序
4. 运行setup_vs2022.bat

### 运行Pass

1. 编写脚本（scripts文件夹）
2. 运行解决方案，通过Load Scipt加载脚本

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

## RenderGrpah

- 表示由若干个Pass组成的渲染管线

## RenderPass

- RenderGraph中的一个节点，接收上一个Pass的输入，计算后输出给下一个Pass
- 在`reflect`函数中规定此Pass的输入和输出

## ResourceBindFlags

- 规定缓冲区的类型，本质上是规定如何访问缓冲区
- 可以使用或运算，表示一个缓冲区兼有多种功能

| 枚举常量              | 说明                   |
| --------------------- | ---------------------- |
| None                  | 临时缓冲区             |
| Vertex                | 顶点缓冲区(VAO和VBO)   |
| Index                 | 顶点索引缓冲区(EBO)    |
| Constant              | 常量                   |
| StreamOutput          | 接收流输出的缓冲区     |
| ShaderResource        | 只读缓冲区             |
| UnorderedAccess       | 可同时读写缓冲区       |
| RenderTarget          | 只写缓冲区             |
| DepthStencil          | 可用作深度测试的缓冲区 |
| Shared                |                        |
| AccelerationStructure | 加速结构               |