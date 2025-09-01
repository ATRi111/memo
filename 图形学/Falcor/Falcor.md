# 工作流

### 创建Pass

1. 打开tools\make_new_render_pass.py
2. 修改create_project的参数
3. 运行py程序
4. 运行CMakeLists文件（或运行setup_vs2022.bat）

### 运行Pass

1. 编写脚本（scripts文件夹）
2. 运行解决方案，通过Load Scipt加载脚本

- 如果要添加新文件，应当在Source文件夹中添加，并修改CMakeLists文件，然后在VS中选择重建项目

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

- 规定资源（缓冲区或纹理）的类型，本质上是规定如何访问资源
- 可以使用或运算，表示一个资源兼有多种功能

| 枚举常量              | 说明                 |
| --------------------- | -------------------- |
| None                  | 临时缓冲区           |
| Vertex                | 顶点缓冲区(VAO和VBO) |
| Index                 | 顶点索引缓冲区(EBO)  |
| Constant              | 常量缓冲区           |
| StreamOutput          | 接收流输出的缓冲区   |
| ShaderResource        | 只读纹理             |
| UnorderedAccess       | 任意位置读写纹理     |
| RenderTarget          | 只写纹理             |
| DepthStencil          | 深度纹理             |
| Shared                |                      |
| AccelerationStructure | 加速结构             |

## Property

- 记录所有可以由外部脚本设置的渲染参数
- 通过类似字典的结构来存储（key是字符串，而value可以是各种类型的变量）

## ChannelDesc

- Channel指的是Pass渲染一个缓冲区的过程，一个Pass可以带有若干个Channel
- ChannelDesc包含了描述Channel的信息：
  - `name`：Channel名
  - `texname`：要渲染的纹理在着色器中的名称
  - `optional`：是否启用
  - `format`：纹理数据格式


### ChannelList

- `ChannelDesc`数组

## Program

- Shader Program

## ShaderVar

- 着色器变量

# Slang文件

