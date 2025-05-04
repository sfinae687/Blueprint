# Blueprint

> 模仿ImHex数据处理器的可编程可扩展节点编辑器。着重于图像处理。
> 
> ——發自我的多媒體技術大作業。

## 模块

- `blueprint.GUI` : GUI 上下文管理器
- `blueprint` : 驱动程序：程序入口点，加载配置信息和扩展，绘制节点编辑器。
- `blueprint.constraint` : 约束求解器：接收图描述的信息，处理节点的数据更新。
- `blueprint.flow` : 数据流描述：定义和描述节点实例的链接管理，
- `blueprint.dyn_node` 节点描述：描述节点编辑器中的节点（操作）和边（数据类型）的行为
  - 节点操作描述
  - 数据类型描述
- `blueprint.draw_node` : 节点绘制描述
- `blueprint.plugin_manage` 插件管理器
- `blueprint.scheduler` 事件和调度器

## 编译

虽然理论上说，所有支持ISO C++23的，并受到CMake模块兼容的Linux平台编译器都可编译该程序。

但因为C++模块的实现的滞后，目前仅有 `Clang 20.1.4` 及更高版本可以成功编译该程序。

## 依赖库

- [Boost 1.88.0](https://www.boost.org/)
- [Dear ImGUI](https://github.com/ocornut/imgui)
- [Proxy](https://github.com/microsoft/proxy)
- [GoogleTest](https://github.com/google/googletest)
- [OpenGL](https://opengl.org/)
- [GLFW](https://github.com/glfw/glfw)
- [Eigen](https://eigen.tuxfamily.org/)

配置说明：

- Boost请在自行安装，并将安装目录配置到 `CMAKE_PREFIX` 变量中。
- OpenGL可以选择任意一种实现。
- GLFW依赖于一下库，需自行安装：
  - `libxkbcommon`
  - `libxinerama`
  - `Xcoursor`
  - `libxi`

其他库作为子模块或在线资源（需要从网络中下载）由项目的构建系统自动配置。