

# 运行

## 初始化

- 过程：
  1. main之前的过程（创建进程→...→执行main函数）
  2. 调用`main`→`GuardedMain`→`FEngineLoop::PreInit`
  3. 引擎核心初始化（内存管理，文件系统，配置系统等）
  4. 加载核心模块
  5. 加载其余业务模块

# 内存管理

## GC

- 使用**标记-清除**算法
  - GC执行时，要求进程**完全暂停**
  - **不分代**；需要遍历所有`UObject`，而不是所有堆对象
  - 回收后**不压缩**（不将不连续分布的对象移动到一起），导致内存碎片增多，进而导致更频繁的申请更大内存
  - 开发者无法自行实现压缩，只能尽量减少GC，减少内存分配（如对象池）
- **`UObject`的GC必须由引擎管理**
  - 任何情况下都不应该定义`UObject`类实例（即不能分配到栈上）
  - 对于本地变量、函数参数，可以定义`UObject`指针或引用（如果确定存在）
    - 要确保作用域内`UObject`存活，可使用`TStrongObjectPtr`

  - `UObject`类中，所有作为成员变量的`TObjectPtr<UObject>`应该由`UPROPERTY`修饰，这样才受引擎管理
  - 原生C++类中可以`Uobject`指针成员不能直接delete
    - 可以令原生类继承`FGCObject`，然后显式将成员注册到`Collector`
    - 也可以用`TStrongObjectPtr`，确保生命周期内`UObject`存活

- **原生C++对象必然不参与标记-清除，可以自行GC，或使用智能指针**
- UE会自动检查引用关系并创建**GC集群（Cluster）**
  - 如果有若干个`UObject`实例仅被某一`UObject`实例引用，那么它们构成集群，该`UObject`是集群的Root
  - 检查集群是否存活时，仅检查Root是否存活即可

- UE使用一个`GUObjectArray`维护所有受引擎GC管理的对象
  - 是一个连续数组，其中每个元素是一个结构体，包含对象指针、GC状态等信息
  - 所有`UObject`有`InternalIndex`成员，表示它在`GUObjectArray`中的索引
  - 编译阶段每个类会生成`ReferenceTokenStream`，确定受引擎GC管理成员以及它们在类内的偏移量，以便GC时快速检索
  - `GUObjectArray`只能添加元素，不会删除已经不存活的元素（这也确保`InternalIndex`的有效性）

- Roots：垃圾回收器完全确定当前不会被回收的对象
  - 静态变量，栈上变量等
  - `AddRoots`显式添加的对象
  - `UWorld`，`UGameInstance`，CDO等引擎自动添加的永久对象
- 流程：
  1. 遍历`GUObjectArray`，将其中所有元素标记为“不可达”
  2. 确定Roots，从Roots出发，根据`ReferenceTokenStream`，找到所有存活的`UObject`对象，将其标记为“可达”
  3. 遍历`GUObjectArray`，将“不可达”对象占用的内存释放
- 使用**增量式GC**，即把一轮内GC内的工作**分配到多帧进行**
  - 性能要求极高的场景，可以通过**预分配内存+临时禁用GC**，进一步分摊GC成本

# C++

## Unreal Header Tool

- UE中包含的一个**可执行程序**，在编译之前就被调用，生成额外的头文件、源文件，参与后续编译（生成在`Intermediate`目录下）
- 对于每个含有至少一个`UCLASS`宏的头文件，生成一个`[头文件名].generated.h`和一个`[头文件名].gen.cpp`
  - 该头文件必须引用`[头文件名].generated.h`，`[头文件名].gen.cpp`会引用该头文件
  - `[头文件名].generated.h`包含`GENERATED_BODY(...)`等宏定义和函数声明
  - `[头文件名].gen.cpp`中包含`[头文件名].generated.h`中函数的具体实现，并定义类及其成员的元数据（静态全局变量）
  - `GENERATED_BODY(...)`以外的宏，C++源代码中会被定义为空文本；UHT处理完后，预处理阶段中，去除这些宏，以免影响编译

**MyThing.h:**

```C++
#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyThing.generated.h"

UCLASS(Blueprintable)
class MYPROJECT_API UMyThing : public UObject
{
    GENERATED_BODY()             

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HP;                    

    UFUNCTION(BlueprintCallable)
    void Heal(int32 Amount);
};
```

**MyThing.generated.h:**

```C++
#pragma once
#include "UObject/ObjectMacros.h"

template<> MYPROJECT_API UClass* StaticClass<UMyThing>();

// 关键：GENERATED_BODY() 展开后往类里注入的成员
#define GENERATED_BODY(...) \
public: \
    PRAGMA_DISABLE_DEPRECATION_WARNINGS \
    /* 允许反射系统访问私有成员、构造元数据 */ \
    friend struct Z_Construct_UClass_UMyThing_Statics; \
    static void StaticRegisterNativesUMyThing(); \
    /* 静态类访问器,为每个类提供全局可访问的静态实例*/ \
    static class UClass* StaticClass(); \
    /* 在每个类内部可使用的别名,指代类自身及其父类 */ \
    typedef UMyThing ThisClass; \
    typedef UObject Super; \
    PRAGMA_ENABLE_DEPRECATION_WARNINGS

```

**MyThing.gen.cpp:**

```c++
#include "MyThing.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "UObject/Script.h"

namespace
{
    struct Z_Construct_UClass_UMyThing_Statics
    {
        static const UECodeGen_Private::FClassParams ClassParams;
    };

    const UECodeGen_Private::FClassParams
        Z_Construct_UClass_UMyThing_Statics::ClassParams =
    {
        &UObject::StaticClass,    // 父类
        nullptr,
        "UMyThing",               // 类名（字符串）
        sizeof(UMyThing),
        ... ,
        PropParms_UMyThing,       // 属性元数据数组
        FuncParms_UMyThing,       // 函数元数据数组
    };
}

// HP的元数据
static const UECodeGen_Private::FIntPropertyParams NewProp_HP = {
    "HP",                         // 属性名（字符串）
    nullptr,                      // getter/setter（无则用直接偏移）
    (EPropertyFlags)0x0010000000000000, // EditAnywhere|BlueprintReadWrite 等功能的标记位
    sizeof(int32),                // 类型大小
    STRUCT_OFFSET(UMyThing, HP),  // HP 在类里的字节偏移（编译期算好）
    ...
};

// Heal的参数amount的元数据
static const UECodeGen_Private::FIntPropertyParams NewProp_Amount = {
    "Amount", (EPropertyFlags)0, sizeof(int32), STRUCT_OFFSET(FHeal_Params, Amount),
};

// Heal的元数据
static const UECodeGen_Private::FFunctionParams FuncParams_Heal = {
    "Heal",                              // 函数名
    nullptr,
    RF_Public | RF_Transient | RF_MarkAsNative,
    (EFunctionFlags)FUNC_BlueprintCallable | FUNC_Native, // 标记位
    sizeof(FHeal_Params),                // 参数结构大小
    &NewProp_Amount,                     // 参数列表
    1,  // 参数个数
    ...
};

//UClass表示类的元数据
UClass* Z_Construct_UClass_UMyThing()
{
    static UClass* Singleton = nullptr;
    if (!Singleton)
    {
        Singleton = UECodeGen_Private::ConstructUClass(
            Z_Construct_UClass_UMyThing_Statics::ClassParams);
    }
    return Singleton;
}
UClass* UMyThing::GetPrivateStaticClass()
{
    static UClass* Singleton = Z_Construct_UClass_UMyThing();
    return Singleton;
}
UClass* UMyThing::StaticClass()
{
    return GetPrivateStaticClass();
}

// 此函数修改类的元数据,为利用反射(按名称)调用成员函数提供支持
// 由于各个成员函数的参数列表各不相同，需要再进行一层封装，即中转函数
// 没有直接将文本关联到成员函数，而是关联到中转函数"execHeal"
void UMyThing::StaticRegisterNativesUMyThing()
{
    UMyThing::StaticClass()->AddNativeFunction(TEXT("Heal"), &UMyThing::execHeal);
}

// 中转函数
DEFINE_FUNCTION(UMyThing::execHeal)
{
    //此中转函数从统一格式的参数中提取出amount参数，传给"Heal"
    P_GET_PROPERTY(FIntProperty, Z_Param_Amount);
    P_FINISH;
    P_NATIVE_BEGIN;
    this->Heal(Z_Param_Amount);                   
    P_NATIVE_END;
}
```

## UObject

### UObject

- 所有引擎对象的父类

- `UObject`的所有子类总是应当使用`UCLASS(...)`和`GENERATED_BODY()`宏，通过UHT生成模板代码，进而获得以下功能：
  - GC：完全由引擎托管
  - 反射：生成元数据
  - 编辑器集成：需要进一步使用`UPROPERTY()`宏
  - 网络同步
  - 工厂函数：`NewObject<T>()`和`CreateDefaultObject<T>()`

### UProperty

- `UProperty`是用于实现反射的类，`UPROPERTY`宏用于修饰`UObject`指针成员
- `UPROPERTY()`的部分“参数”：
  - 编辑器访问权限和外观
  - 蓝图访问权限
  - 配置和存储
  - 元数据

### Class Default Object

- 所有`UObject`子类都分别持有一个的，作为模板存在的类实例
- 其中包含了类中各变量的默认值，但通常不应该直接修改，而应该在`PostInitProperties`或后续函数中覆盖
- 构造新`UObject`实例时，将其CDO的占用的整块内存直接复制一份，而不必执行完整的构造逻辑，以实现**高效创建**
- 构造全局/静态变量时，将所有`UObject`子类的`UClass* Z_Construct_UClass_[类名]()`函数指针添加到列表中（每个模块一个列表）；加载模块时，调用列表中的所有函数，调用到`Z_Construct_UClass_[类名]`时进一步调用了`UClass::CreateDefaultObject()`，从而创建出CDO

## Actor

### Actor

- 指`AActor`及其子类实例，或派生出的蓝图类及其实例
- 只有Actor能直接放到场景中，自身也有一些逻辑（一般只有整个Actor共享的逻辑/数据），类似`GameObject`
- Actor可以挂载到其他Actor上，子Actor跟随父Actor移动（但**不会随父Actor一同被销毁**）
- 每个Actor被视为一个整体，在Outliner中仅显示Actor及其层级关系，不会显示Component

生命周期（按顺序）：

| 函数                       | 含义/调用时机      | 说明                                             |
| -------------------------- | ------------------ | ------------------------------------------------ |
| `SpawnActor<T>`            | 动态创建Actor      | 还有其他创建方式                                 |
| `PostInitProperties`       | 构造后             | 构造时直接复制CDO                                |
| `PostSpawnInitialize`      |                    |                                                  |
|                            |                    |                                                  |
| `PostActorCreated`         | 动态创建后         | 与`PostActorCreated`，`PostDuplicate`互斥        |
| `PostLoad`                 | 从硬盘加载后       | 适用于人为放到场景中的物体                       |
| `PostDuplicate`            | 复制后             | 仅在编辑器模式下，完全取代`PostActorCreated`     |
|                            |                    |                                                  |
| `UWorld::AddActor()`       |                    | 将`Tick`注册到`FTickTaskManager`                 |
| `PreInitializeComponents`  | 组件初始化之前     |                                                  |
| `InitializeComponents`     | 通知所有组件初始化 |                                                  |
| `PostInitializeComponents` | 所有组件初始化后   |                                                  |
|                            |                    |                                                  |
| `BeginPlay`                | 第一次`Tick`之前   |                                                  |
| `Tick`                     | 每帧               | 用TickGroup控制调用时机，一个Actor只有一个`Tick` |
|                            |                    |                                                  |
| `EndPlay`                  | 销毁前             |                                                  |
| `Destroyed`                | 销毁后             |                                                  |

- 以上所有生命周期函数，都**严格分阶段串行调用**；同时创建/运行/销毁的一系列物体，
- Actor没有禁用状态，但可以直接`SetActorTickEnabled`

### Component

- 指`UActorComponent`及其子类对象，以及派生出的蓝图类
- 一个Actor持有若干Component，类似`MonoBehaviour`
  - 与Unity不同的是，Component之间也有层级关系，每个Component也有各自的Transform
  - Component间的层级关系仅在Actor的Details/Component窗口中显示
  - Actor被销毁时，其持有的Component也被销毁；销毁Component时，默认情况下不会递归地销毁其子Component


生命周期（按顺序）：

| 函数                  | 含义/调用时机                 | 说明                                                 |
| --------------------- | ----------------------------- | ---------------------------------------------------- |
| `PostInitProperties`  | 构造后                        |                                                      |
| `InitializeComponent` | 初始化                        |                                                      |
|                       |                               |                                                      |
| `OnRegister`          | 所属的`Actor`注册到`UWorld`时 |                                                      |
| `BeginPlay`           | 第一次`Tick`之前              |                                                      |
| `TickComponent`       | 每帧                          | 用TickGroup控制调用时机，一个Component只有一个`Tick` |
|                       |                               |                                                      |
| `EndPlay`             | 销毁前                        |                                                      |
| `Destroyed`           | 销毁后                        |                                                      |

### FTickTaskManager

- 统一调度所有Actor和Component的`Tick`，构建了基于依赖关系的Task Group
  - `TickGroup`控制基本的`Tick`时机
  - 可以人为设置依赖关系（`AddTickPrerequisiteActor`），控制同一`TickGroup`内的顺序
  - 默认情况下子Actor自动依赖父Actor，组件自动依赖所属的Actor

| Tick Group          | 枚举值 | 用途                                                    |
| ------------------- | ------ | ------------------------------------------------------- |
| `TG_PrePhysics`     | 0      | 大部分 Actor 的 Tick。输入处理、AI 决策、动画参数设置   |
| `TG_StartPhysics`   | 1      | 物理模拟启动前的特殊处理                                |
| `TG_DuringPhysics`  | 2      | 物理模拟异步执行，TG_DuringPhysics 中的 Tick 与物理并行 |
| `TG_EndPhysics`     | 3      | 物理结果处理（接收碰撞回调时）                          |
| `TG_PostPhysics`    | 4      | 基于物理结果更新状态。移动摄像机、更新 IK               |
| `TG_PostUpdateWork` | 5      | 动画后处理、Finalize 阶段的更新                         |
| `TG_LastDemotable`  | 6      | 低优先级 Tick（可被降频）                               |
| `TG_NewlySpawned`   | 7      | 本帧新生成的 Actor 的首次 Tick                          |

## Inupt

- 输入流程
  1. 物理设备输入，操作系统封装为事件（平台有关）
  2. `FSlateApplication`将操作系统事件封装为`FInputEvent`
  3. 优先将事件分发给获得焦点的Widget，如果未被消费则继续传递
  4. `UGameViewportClient`找到操作对应的`ULocalPlayer`
  5. 该Player的`UEnhancedInputLocalPlayerSubsystem`遍历持有的IMC堆栈
  6. 对于每个IMC，查找当前`FKey`对应的`UInputAction`
  7. 对于找到的`UInputAction`，如果有`UInputModifier`链，逐个应用，将`RawValue`修改为`ModifiedValue`
  8. 进一步根据`UInputTrigger`和输入值，判断操作是否触发，一旦触发，则消费此输入
  9. 进一步调用`APlayerController::ProcessInputStack`，遍历栈中所有`UInputComponent`，调用`ProcessInput`；一旦被消费，则中止后续调用
  10. 对于成功触发的`UInputAction`，调用其绑定的回调函数（如Actor行为）

### FInputActionValue

- 将所有类型的输入数据统一起来的类
  - Boolean：单个按键
  - Axis1D：扳机
  - Axis2D：WASD组合键，摇杆
  - Axis3D：VR手柄，3D鼠标

### FKey

- 单个物理按键/摇杆/扳机
- 每个具体的Key都预设好了对应的输入数据类型

### UInputAction

- 定义玩家接收到输入后执行的一个行为
- 将按键和玩家行为解耦；多个按键可以触发同一行为，为按键绑定提供支持

### InputMappingContext

- 

### UInputModifier

- 

## World

### Level

- 指`ULevel`实例，不应该继承此类
- `ULevel`用一个数组记录其中的所有Actor，其中的0号元素是`AWorldSetting`（当某个关卡被用作主关卡时，其`AWorldSetting`对World生效）
- 额外持有一个特殊的Actor，`ALevelScriptActor`，是关卡蓝图的父类

### World

- 指`UWorld`实例，不应该继承此类
- World中可以包含若干个Level，运行时动态变化

### GameMode

- 通常用于规定规则（游戏开始结束，获胜条件，关卡切换，玩家加入、重生等）
- 仅存在于服务器端

### GameState

- 指`GameStateBase`及其子类（`AGameState`适用于竞技类比赛）
- 通常包含`GameMode`相关的游戏数据（默认包含玩家列表，当前`GameMode`，当前游戏状态等数据；可人为添加其他数据）
- 由服务器发给客户端

# 蓝图

- 几类资源文件的统称，类似于支持多种资源类型的Prefab，与C++协同使用（非必须）

- 通过继承`FBlueprintEditor`，各种不同的蓝图有不同的编译器

- 各种蓝图资产中，与程序有关的部分会**增量编译**成**字节码**，由**蓝图虚拟机**运行
  - 和C++程序相比，编译更快，执行更慢；因此更适合实现表层的、经常修改的逻辑
- **蓝图虚拟机**：引擎中内置的C++程序，能执行一些底层的原子操作（读写变量，调用C++函数，跳转等）
  - 蓝图中的各种逻辑需要编译成由这些底层指令和数据组成的**字节码**

## 蓝图类

- 蓝图资产的一种，**概念上单继承**自一个C++类，但本身**不是C++类**
  - 编译而得的机器码在类的上下文中运行（可访问父类成员），以体现继承、多态
  - 能够通过**蓝图接口**绕开单继承的限制
  - 可以有多个蓝图类继承同一C++类
- 除了程序，还有设置参数默认值等功能，且不同蓝图类的编辑窗口不同（如Actor蓝图中可设置Component）
- 要创建蓝图类，需要用`UCLASS(Blueprintable,...)`修饰要继承的C++类
  - 静态函数和成员函数需要用`UFUNCTION`修饰，设为对蓝图可见
    - 调用成员函数需要指定实例(Target)，如果是父类的成员函数，Target默认为self

  - 成员变量需要用`UProperty`修饰，设为对蓝图可见

| UPROPERTY参数        | 含义           |
| -------------------- | -------------- |
| `EditAnyWhere`       | 编辑器中可修改 |
| `VisibleAnyWhere`    | 编辑器中只读   |
| `EditDefaultsOnly`   | 只能保持默认值 |
| `BlueprintReadWrite` | 蓝图中可读写   |

| UFUNCTION参数                 | 含义                      |
| ----------------------------- | ------------------------- |
| `BlueprintCallable`           | 蓝图中可调用              |
| `BlueprintPure`               | 蓝图中呈现为纯函数[1]     |
| `BlueprintImplementableEvent` | 留给蓝图实现的纯虚函数[2] |

[1]：纯函数只有输入输出，没有调用和被调用引脚，蓝图虚拟机只会在必要时自动调用纯函数

[2]：蓝图中无法调用，只能由C++代码调用（如果额外用`BlueprintCallable`修饰，会多出一个蓝图函数，和事件是两种节点）

### Actor蓝图

- 指继承自某个`AActor`子类的蓝图，其中包含一个Actor和若干个子Component（及其参数等）
  - 继承自Component的蓝图类不像Actor蓝图那样有充分的编辑器支持

- 可以在C++中为Actor挂载组件；在**构造函数**中挂载的组件，创建出的Actor蓝图中能直接看到（只读）
- 也可以在Actor蓝图中挂载组件，然后C++中可以定义/获取子组件
- Actor蓝图中可以添加**Child Actor**，这是一种特殊的**Component**
  - 该Component运行时生成一个子Actor（挂载到父Actor下），并且在父Actor销毁时同时销毁子Actor
  - 该Component的`Child Actor Class`参数规定了要生成哪个Actor蓝图的实例


### 关卡蓝图

- 一个特殊的蓝图类（继承自`ALevelScriptActor`)，仅存在于关卡文件中
- 用于实现关卡特有的逻辑，通常应该让蓝图类来获取关卡蓝图，并在关卡蓝图中定义一些事件

## 蓝图接口

- 类似抽象类，蓝图类实现蓝图接口，以实现多继承
- 其中定义纯虚函数，蓝图实现接口时，显示为事件（？）

## 动画蓝图

## 控件蓝图















































