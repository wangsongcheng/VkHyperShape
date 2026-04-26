# VkHyperShape

**基于 Vulkan 的实时四维几何可视化程序**

VkHyperShape 专注于解决**四维空间到三维空间的投影与渲染**问题。
它通过自定义的 4D 数学库与 Vulkan 高性能渲染管线，将抽象的高维几何结构直观地呈现在屏幕上。

![1](https://github.com/wangsongcheng/VkHyperShape/screenshot/1.png)
![2](https://github.com/wangsongcheng/VkHyperShape/screenshot/2.png)
![3](https://github.com/wangsongcheng/VkHyperShape/screenshot/3.png)

---

## ✨ 核心特性

- **真·四维变换**：在 CPU 端计算 4D 旋转与位移，而非简单的 3D 变形。
- **双重投影模式**：支持**透视投影**(近大远小，更有空间感)与**正交投影**。
- **GPU 驱动**：利用 Vulkan 实现高效的实时光栅化。
- **通用几何支持**：不仅限于超立方体，支持多种正多胞形。

---

## 🧠 技术实现

### 4D 投影管线
本项目采用经典的**齐次坐标**体系，将维度从 3D 扩展到4D：