# Blueprint

> 模仿ImHex数据处理器的可编程可扩展节点编辑器。着重于图像处理。
> 
> ——發自我的多媒體技術大作業。

适用于图形处理的节点编辑器工具。项目初衷在2D游戏中为图形资源提供一个可灵活调整的资源预处理程序（比如提供一个法线贴图生成）。目前的设计目标为：
成为一个插件驱动的图形处理节点编辑器，并作为一些C++新特性的实验项目。

## CONTRIBUTE

### 模块

Blueprint 使用 [C++ Module](https://zh.cppreference.com/w/cpp/language/modules) 管理组件。各模块名和相应职责如下：

- `blueprint` : 驱动程序：程序入口点，加载配置信息和扩展，绘制节点编辑器。
- `blueprint.GUI` : GUI 上下文管理器
- `blueprint.plugin_manage` 插件管理器
- `blueprint.constraint` : 约束求解器：接收图描述的信息，处理节点的数据更新。
- `blueprint.flow` : 数据流描述：定义和描述节点实例的链接管理，
- `blueprint.draw_node` : 节点绘制描述。
- `bluerpint.stk_node` : 从动态信息中”找回“静态类型，或将静态类型封装成动态信息。
- `blueprint.dyn_node` 节点描述：描述节点编辑器中的节点（操作）和边（数据类型）的行为。用于跨越DLL边界。
- `blueprint.scheduler` 事件和调度器

诚然，该项目模块之间耦合十分紧密，只保持住了基本的层级关系，并非良好设计。

### 编译

虽然理论上说，所有支持ISO C++23的，并受到CMake C++模块特性兼容的Linux平台编译器都可编译该程序。

但由于CMake依赖于编译器提供的方法分析模块依赖关系以及C++模块实现的问题（Bug），目前仅有 `Clang 20.1.4` 及更高版本可以通过CMake成功编译该程序。

因为预计会使用Linux上特有的某些系统调用，所以应该不可在MSVC上编译。但是这些系统调用在Windows上有等价版本，所以
应该并不存在很大的移植障碍。

### 依赖库

- [Boost 1.88.0](https://www.boost.org/)
- [Dear ImGUI](https://github.com/ocornut/imgui)
- [Proxy](https://github.com/microsoft/proxy)
- [GoogleTest](https://github.com/google/googletest)
- [OpenGL](https://opengl.org/)
- [GLFW](https://github.com/glfw/glfw)
- [Eigen](https://eigen.tuxfamily.org/)
- [thread-pool](https://github.com/DeveloperPaul123/thread-pool)

配置说明：

- Boost请在自行安装，并将安装目录配置到 `CMAKE_PREFIX` 变量中。
- OpenGL可以选择任意一种实现。
- GLFW依赖于一下库，需自行安装：
  - `libxkbcommon`
  - `libxinerama`
  - `Xcoursor`
  - `libxi`

其他库作为子模块或在线资源（需要从网络中下载）由项目的构建系统自动配置。