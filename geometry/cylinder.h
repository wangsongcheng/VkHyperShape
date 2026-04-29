#ifndef CYLINDER_H
#define CYLINDER_H
#include "geometry.h"
struct CylinderParameter{
    float radius = .015;
    float samples = 20;
    uint32_t segments = 16;
    //这样写能直接传给imgui
    std::array<float, 4>point[4] = { {1, 0, 0, 0}, { 0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} };
};
class Cylinder:public Geometry{
    vulkan::Primitive mGeometry;
    vulkan::Primitive mWireframe;
public:
    Cylinder(/* args */);
    ~Cylinder();
    virtual void Cleanup();

    virtual void Draw(vk::CommandBuffer command, vk::PipelineLayout layout);
    virtual void DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout);

    virtual void Update(const void *useData = nullptr);
};
#endif