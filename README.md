# Blueprint

> 模仿ImHex数据处理器的可编程可扩展节点编辑器。着重于图像处理。
> 
> ——發自我的多媒體技術大作業。

## 模块设计

- GUI 上下文管理器
- 驱动程序：程序入口点，加载配置信息和扩展，绘制节点编辑器。
- 约束求解器：接收图描述的信息，处理节点的数据更新。
- 数据流描述：定义和描述节点实例的链接管理，
- 节点描述：描述节点编辑器中的节点（操作）和边（数据类型）的行为
  - 节点操作描述
  - 数据类型描述
  - 节点的附加 Widget
- 插件管理器
- 事件和调度器


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