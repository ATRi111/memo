# OpenGL

- **规定了一系列图形API的一套标准**（实际实现API的代码存放在显卡驱动中）

## 概念

### 上下文

- OpenGL程序是一种“状态机”，要实现一个功能，通常需要先设置好一系列的参数（Context），而不是单纯地调用一个API
- **上下文(Context)**即**当前**渲染所需的数据和渲染设置等
- 将某个对象**绑定**到上下文意味着接下来此对象将被用作渲染数据（直到**解除绑定**）

### VBO

![VAO与VBO](VAO与VBO.png)

- VBO：顶点缓冲区对象（在GPU中创建），一块连续的，数据格式不完全固定的内存空间
  - 可以视为一个数组，每个元素包含单个顶点的所有**属性**

- VAO：顶点数组对象（在GPU中创建），程序根据VAO中的内容来确定VBO中数据的布局

## API

```C++
glGenBuffers(int count,int* ret):生成若干个缓冲区
	count:数量
    ret:接收结果(ret会被视为数组首地址,其中存放生成的所有缓冲区的标识符)

glBindBuffer(int target,int buffer):将指定缓冲区绑定到上下文
    target:缓冲区类型
    buffer:缓冲区的标识符(0表示将绑定到上下文的指定类型的缓冲区解除)

glBufferData(int target,int size,void* data,int usage):把一块数据复制到当前绑定的缓冲区
    target:缓冲区类型
    size:缓冲区大小
    data:数据首地址
    usage:用途编码	//STATIC:仅修改一次 DYNAMIC:可能被修改多次 STREAM:每次绘制都会改变

        
glGenVertexArrays(int count,int* ret):生成若干个顶点数组
    count:数量
    ret:接收结果(ret会被视为数组首地址,其中存放生成的所有顶点数组的标识符)

glBindVertexArray(int array):将指定顶点数组绑定到当前上下文
    array:顶点数组的标识符(0表示将绑定到上下文的顶点数组解除)

glVertexAttribPointer(int index, int count, int type, int normalized, int stride, void* offset):
指明VBO中某种属性的分布,本质上是在修改VAO
	index:分配给该属性的编号
    count:每个顶点包含多少个该属性	(例如,位置、颜色通常包含三个浮点数)
    type:该属性的变量类型编码		//GL_FLOAT:浮点数
    normalized:是否需要标准化
    stride:两个相邻顶点在VBO中的间距(由每个节点包含的属性种类决定)
    offset:该属性相对于顶点初始地址的偏移量
	        
glEnableVertexAttribArray(int index):启用顶点属性
    index:启用顶点属性的编号

        
glShaderSource(int index,int count,char* string,int length):设定着色器的源代码
    index:着色器的标识符
    count:源代码的段数(字符串可以分多段)
    string:源代码字符串首地址
    length:字符串长度(如果设为NULL,自动根据字符串中的结束符确定长度)
```

# GLFW

- 主要用于绘制窗口、接受输入的第三方库（可用于OpenGL、Vulkan、OpenGL ES）

## 概念

- GLFWContext：绘制窗口所需的数据和设置，每个GLFWwindow拥有一个
  - calling thread：调用线程，指当前正在运行的线程，此线程中执行的GLFW API会作用于**当前活跃的GLFWContext**
- GLFWwindow使用**双缓冲**，渲染计算在后缓冲进行，计算完毕后交换前后缓冲，根据前缓冲中的数据进行渲染

## 示例

```c++
#include <GLFW/glfw3.h>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
```

## API

```c++
glfwMakeContextCurrent(GLFWwindow* handle):将当前活跃的上下文设为指定窗口的上下文
    handle:窗口指针(如果为nullptr,则使当前活跃的上下文不再活跃)

void glfwSwapBuffers (GLFWwindow *window):交换指定窗口的前后缓冲区
    handle:窗口指针
```

# GLAD

- 用于访问OpenGL API的第三方库
