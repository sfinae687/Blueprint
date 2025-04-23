# Blueprint
> 模仿ImHex数据处理器的可编程可扩展节点编辑器。着重于图像处理。
> 
> ——發自我的多媒體技術大作業。

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