#include <array>
#include <thread>
#include <chrono>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "Camera.hpp"
#include "imgui_impl/glfw.h"
#include "vulkan/renderer.hpp"
#include "imgui_impl/vulkan.hpp"
#ifdef DEBUG
#include "geometry/test.h"
#endif
#include "geometry/font.h"
#include "geometry/grid.h"
#include "geometry/clifford.h"
#include "geometry/tesseract.h"
#include "geometry/pentatope.h"
#include "geometry/cylindrical.h"
#include "geometry/sixteenCell.h"
#include "geometry/kleinBottle.h"
#include "geometry/hypersphere.h"
#include "geometry/twentyFourCell.h"
struct ImGuiPlaneInput{
    float angle;
    std::string splane;
    mglm::Plane plane;
    ImGuiPlaneInput(){
    }
    ImGuiPlaneInput(const std::string&splane){
        this->splane = splane;
    }
};
struct MVP_UBO{
    float view[5][5];
    float model[5][5];
    float projection[5][5];
};
struct ImGuiInput{
    bool grid4d;
    bool grid3d;
    bool fill = true;
    bool font = false;
    bool ortho = false;
    bool clifford = false;
    bool cylinder = false;
    bool tesseract = true;
    bool pentatope = false;
    bool cylindrical = false;
    bool sixteenCell = false;
    bool kleinBottle = false;
    bool hypersphere = false;
    bool twentyFourCell = false;
    bool UpdateGeometry = false;
    CylinderParameter parameter;
#ifdef DEBUG
    bool testGeometry = false;
#endif
    void UnSelect(){
        font = false;
        ortho = false;
        grid4d = false;
        grid3d = false;
        clifford = false;
        cylinder = false;
        tesseract = false;
        pentatope = false;
        cylindrical = false;
        sixteenCell = false;
        kleinBottle = false;
        hypersphere = false;
        twentyFourCell = false;
        // memset(this, 0, sizeof(ImGuiInput));
    }
#ifdef DEBUG
    void SeletctTestGeometry(){
        UnSelect();
        testGeometry = true;
        UpdateGeometry = true;
    }
#endif
    void SelectTesseract(){
        UnSelect();
        tesseract = true;
        UpdateGeometry = true;
    }
    void SelectPentatope(){
        UnSelect();
        pentatope = true;
        UpdateGeometry = true;
    }
    void SelectTwentyFourCell(){
        UnSelect();
        twentyFourCell = true;
        UpdateGeometry = true;
    }
    void SelectCylinder(){
        UnSelect();
        cylinder = true;
        UpdateGeometry = true;
    }
    void SelectClifford(){
        UnSelect();
        clifford = true;
        UpdateGeometry = true;
    }
    void SelectSixteenCell(){
        UnSelect();
        sixteenCell = true;
        UpdateGeometry = true;
    }
    void SelectKleinBottle(){
        UnSelect();
        kleinBottle = true;
        UpdateGeometry = true;
    }
    void SelectCylindrical(){
        UnSelect();
        cylindrical = true;
        UpdateGeometry = true;
    }
    void SelectGrid3D(){
        UnSelect();
        fill = false;
        grid3d = true;
        UpdateGeometry = true;
    }
    void SelectGrid4D(){
        UnSelect();
        fill = false;
        grid4d = true;
        UpdateGeometry = true;
    }
    void SelectHypersphere(){
        UnSelect();
        hypersphere = true;
        UpdateGeometry = true;        
    }
    void SelectFont(){
        UnSelect();
        font = true;
        UpdateGeometry = true;
    }
};
struct PushConstant{
    glm::mat4 model;
    glm::mat4 projection;
};
struct GraphicsPipeline{
    //整个程序只需要一个
    vk::PipelineCache cache;
    vk::PipelineLayout layout;
    vk::Pipeline fill, wireframe;
};
vulkan::Pool g_VulkanPool;
vulkan::Queue g_VulkanQueue;
vulkan::Device g_VulkanDevice;
vulkan::RenderEngine g_VulkanRenderer;

uint32_t g_WindowWidth = 1000, g_WindowHeight = g_WindowWidth;

Camera g_Camera(3);
vulkan::Buffer g_MvpUbo;
GraphicsPipeline g_Pipeline;

std::vector<vk::DescriptorSet>g_Set;
std::vector<vk::DescriptorSetLayout>g_CameraSetLayout(2);

Geometry *g_Geometry;

// PickingTexture g_PickingTexture;

vulkan::ImGui g_VulkanImGui;

std::array<ImGuiPlaneInput, 2>g_Plane;

ImGuiInput g_ImGuiInput;

mglm::vec4 g_CameraPos = mglm::vec4(0, 0, 0, 3);
const char *GetPlaneString(const mglm::Plane&plane){
    if(mglm::abs(plane) == mglm::planes::XY){
        return "XY";
    }
    else if(mglm::abs(plane) == mglm::planes::XZ){
        return "XZ";
    }
    else if(mglm::abs(plane) == mglm::planes::YZ){
        return "YZ";
    }
    else if(mglm::abs(plane) == mglm::planes::XW){
        return "XW";
    }
    else if(mglm::abs(plane) == mglm::planes::YW){
        return "YW";
    }
    else if(mglm::abs(plane) == mglm::planes::ZW){
        return "ZW";
    }
    return "UNKNOW";
}
void SetPlane(const std::string&splane){
    if(splane == "XY"){
        g_Plane[0].plane = mglm::planes::XY;
        g_Plane[1].plane = mglm::orthogonal(g_Plane[0].plane);
    }
    else if(splane == "XZ"){
        g_Plane[0].plane = mglm::planes::XZ;
        g_Plane[1].plane = mglm::orthogonal(g_Plane[0].plane);
    }
    else if(splane == "YZ"){
        g_Plane[0].plane = mglm::planes::YZ;
        g_Plane[1].plane = mglm::orthogonal(g_Plane[0].plane);
    }
    else if(splane == "XW"){
        g_Plane[0].plane = mglm::planes::XW;
        g_Plane[1].plane = mglm::orthogonal(g_Plane[0].plane);
    }
    else if(splane == "YW"){
        g_Plane[0].plane = mglm::planes::YW;
        g_Plane[1].plane = mglm::orthogonal(g_Plane[0].plane);
    }
    else if(splane == "ZW"){
        g_Plane[0].plane = mglm::planes::ZW;
        g_Plane[1].plane = mglm::orthogonal(g_Plane[0].plane);
    }
}
void UpdateUniform(const vulkan::Device&device){
    mglm::mat5 projection;
    if(g_ImGuiInput.ortho){
        projection = mglm::ortho(-1, 1, -1, 1, -1.0f, 1.0f, -1, 1.0f);
    }
    else{
        projection = mglm::perspective(glm::radians(45.0f), glm::radians(45.0f), glm::radians(45.0f), .1f, 100.0f);
    }
    const mglm::mat5 view = mglm::lookAt(g_CameraPos, mglm::vec4(0), mglm::vec4(0, 1, 0, 0));
    const mglm::mat5 model = mglm::rotate(mglm::mat5(1.0f), glm::radians(g_Plane[0].angle), g_Plane[0].plane, glm::radians(g_Plane[1].angle), g_Plane[1].plane);
    MVP_UBO ubo;
    for (size_t i = 0; i < 5; i++){
        for (size_t j = 0; j < 5; j++){
            ubo.view[i][j] = view[i][j];
            ubo.model[i][j] = model[i][j];
            ubo.projection[i][j] = projection[i][j];
        }
    }
    g_MvpUbo.UpdateData(device, &ubo);
}
void ShowGeometry(){
    ImGui::SeparatorText("几何");
    const std::array currentItems = { "超立方体", "超圆柱", "超球", "贝塞尔管道", "四维字", "正五胞体", "正十六胞体", "正二十四胞体", "Clifford环面", "克莱因瓶", "三维网格", "四维网格",
#ifdef DEBUG
        "图元测试"
#endif
    };
    static std::string geometry = currentItems[0];
    if(ImGui::BeginCombo("几何", geometry.c_str())){
        for (auto currentGeometry = currentItems.begin(); currentGeometry != currentItems.end(); ++currentGeometry){
            bool is_selected = geometry == *currentGeometry;
            if (ImGui::Selectable(*currentGeometry, is_selected)){
                geometry = *currentGeometry;
                if(geometry == "超立方体"){
                    g_ImGuiInput.SelectTesseract();
                }
                else if(geometry == "超球"){
                    g_ImGuiInput.SelectHypersphere();
                }
                else if(geometry == "正五胞体"){
                    g_ImGuiInput.SelectPentatope();
                }
                else if(geometry == "正十六胞体"){
                    g_ImGuiInput.SelectSixteenCell();
                }
                else if(geometry == "正二十四胞体"){
                    g_ImGuiInput.SelectTwentyFourCell();
                }
                else if(geometry == "Clifford环面"){
                    g_ImGuiInput.SelectClifford();
                }
                else if(geometry == "克莱因瓶"){
                    g_ImGuiInput.SelectKleinBottle();
                }
                else if(geometry == "超圆柱"){
                    g_ImGuiInput.SelectCylindrical();
                }
                else if(geometry == "四维字"){
                    g_ImGuiInput.SelectFont();
                }
                else if(geometry == "贝塞尔管道"){
                    g_ImGuiInput.SelectCylinder();
                }
                else if(geometry == "三维网格"){
                    g_ImGuiInput.SelectGrid3D();
                }
                else if(geometry == "四维网格"){
                    g_ImGuiInput.SelectGrid4D();
                }
#ifdef DEBUG
                else if(geometry == "图元测试"){
                    g_ImGuiInput.SeletctTestGeometry();
                }
#endif
                break;
            }
        }
        ImGui::EndCombo();
    }
}
void ShowPlaneCombo(const char *lable){
    const std::vector<std::string> currentItems = { "XY", "XZ", "YZ", "XW", "YW", "ZW" };
    static std::string plane = currentItems[0];
    if(ImGui::BeginCombo(lable, plane.c_str())){
        for (auto currentPlane = currentItems.begin(); currentPlane != currentItems.end(); ++currentPlane){
            bool is_selected = plane == *currentPlane;
            if (ImGui::Selectable(currentPlane->c_str(), is_selected)){
                plane = *currentPlane;
                break;
            }
        }
        ImGui::EndCombo();
    }
    SetPlane(plane);
}
void ShowRotate(){
    ImGui::SeparatorText("四维旋转");
    ShowPlaneCombo("平面一");
    ImGui::SameLine();
    ImGui::Text("平面二:%s", GetPlaneString(g_Plane[1].plane));
    static bool syncAngle, modifyPlane;
    ImGui::Checkbox("同步角度", &syncAngle);
    if(ImGui::SliderFloat("平面一角度", &g_Plane[0].angle, 0, 360)){
        if(syncAngle){
            g_Plane[1].angle = g_Plane[0].angle;
        }
        UpdateUniform(g_VulkanDevice);
    }
    if(ImGui::SliderFloat("平面二角度", &g_Plane[1].angle, 0, 360)){
        if(syncAngle){
            g_Plane[0].angle = g_Plane[1].angle;
        }
        UpdateUniform(g_VulkanDevice);
    }
}

void UpdateImGui(VkCommandBuffer command){
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if(ImGui::BeginMainMenuBar()){
        if(ImGui::BeginMenu("摄像机")){
            if(ImGui::MenuItem("重置")){
                g_Camera.UpdateUniform(g_VulkanDevice);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if(ImGui::Begin("四维几何")){
        if(ImGui::Checkbox("填充", &g_ImGuiInput.fill)){
            UpdateUniform(g_VulkanDevice);
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("正交投影", &g_ImGuiInput.ortho)){
            UpdateUniform(g_VulkanDevice);
        }
        static float cameraPos[] = { g_CameraPos.x, g_CameraPos.y, g_CameraPos.z, g_CameraPos.w };
        if(ImGui::InputFloat4("四维摄像机", cameraPos)){
            g_CameraPos = mglm::vec4(cameraPos[0], cameraPos[1], cameraPos[2], cameraPos[3]);
            UpdateUniform(g_VulkanDevice);
        }
        ShowRotate();
        ShowGeometry();
        if(g_ImGuiInput.cylinder || g_ImGuiInput.font){
            if(ImGui::InputFloat("半径", &g_ImGuiInput.parameter.radius)){
                g_Geometry->Update(&g_ImGuiInput.parameter);
            }
            if(ImGui::InputFloat("采样率", &g_ImGuiInput.parameter.samples)){
                g_Geometry->Update(&g_ImGuiInput.parameter);
            }
            for (size_t i = 0; i < 4; i++){
                char label[0xff];
                sprintf(label, "点%d", i);
                if(ImGui::InputFloat4(label, g_ImGuiInput.parameter.point[i].data())){
                    g_Geometry->Update(&g_ImGuiInput.parameter);
                }
            }
        }
    }
    ImGui::End();
    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool isMinimized = (draw_data->DisplaySize.x <=.0f || draw_data->DisplaySize.y <= .0f);
    if(!isMinimized)g_VulkanImGui.RenderDrawData(command, draw_data);
}
void Draw(vk::CommandBuffer command, const vk::Pipeline *pipeline, uint32_t count){
    PushConstant pc;
    pc.model = glm::mat4(1.0f);
    pc.projection = glm::perspective(glm::radians(45.0f), (float)g_WindowWidth / g_WindowHeight, 0.1f, 100.0f);
    pc.projection[1][1] *= -1;
    if(g_ImGuiInput.fill){
        vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline[0]);
    }
    else{
        vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline[1]);
    }
    command.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, g_Pipeline.layout, 0, g_Set, {});
    vkCmdPushConstants(command, g_Pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant), &pc);
    if(g_ImGuiInput.fill){
        g_Geometry->Draw(command, g_Pipeline.layout);
    }
    else{
        g_Geometry->DrawWireframe(command, g_Pipeline.layout);
    }
}
void RecordCommand(vk::CommandBuffer command, vulkan::Image&color, vulkan::Image&depth){
    vulkan::framework::BeginCommands(command, vk::CommandBufferUsageFlagBits::eSimultaneousUse);
    vulkan::framework::BeginRendering(command, {&color}, {depth}, g_WindowWidth, g_WindowHeight);
    std::array pipeline = { g_Pipeline.fill,  g_Pipeline.wireframe };
    Draw(command, pipeline.data(), pipeline.size());

    UpdateImGui(command);

    vulkan::framework::EndRendering(command, color);
    command.end();
}
void RecordCommand(vk::CommandBuffer command, vk::Framebuffer frame, vk::RenderPass renderPass){
    vulkan::framework::BeginCommands(command, vk::CommandBufferUsageFlagBits::eSimultaneousUse);
    vulkan::framework::BeginRenderPass(command, frame, renderPass, g_WindowWidth, g_WindowHeight);

    std::array pipeline = { g_Pipeline.fill,  g_Pipeline.wireframe };
    Draw(command, pipeline.data(), pipeline.size());

    UpdateImGui(command);

    vkCmdEndRenderPass(command);
    vkEndCommandBuffer(command);
}

void keybutton(GLFWwindow *window, int key, int scancode, int action, int mods){

}
void mousescroll(GLFWwindow *window, double xoffset, double yoffset){;
    g_Camera.ProcessMouseScroll(yoffset);
    g_Camera.UpdateUniform(g_VulkanDevice);
}
glm::vec2 g_MousePos;
void mousecursor(GLFWwindow *window, double xpos, double ypos){
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glm::vec2 current(xpos, ypos);
        glm::vec2 delta = current - g_MousePos;
        g_MousePos = current;
        g_Camera.ProcessMouseMovement(delta.x, delta.y);
        g_Camera.UpdateUniform(g_VulkanDevice);
    }
}
void mousebutton(GLFWwindow *window,int button,int action,int mods){
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if(action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT){
        g_MousePos = glm::vec2(xpos, ypos);
    }
}

void DestroyGraphicsPipeline(vk::Device device){
    vkDestroyPipeline(device, g_Pipeline.fill, VK_NULL_HANDLE);
    vkDestroyPipeline(device, g_Pipeline.wireframe, VK_NULL_HANDLE);
    vkDestroyPipelineLayout(device, g_Pipeline.layout, VK_NULL_HANDLE);
}
void CreatePipelineLayout(vk::Device device){
    auto info = vulkan::pipeline::initializers::pipelineLayoutCreateInfo(g_CameraSetLayout.size(), 1);
    auto pushConstantRange = vulkan::pipeline::initializers::pushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(PushConstant), 0);
    info.pSetLayouts = g_CameraSetLayout.data();
    info.pPushConstantRanges = &pushConstantRange;
    g_Pipeline.layout = device.createPipelineLayout(info);
}
void CreateGraphicsPipeline(vk::Device device, VkPipelineLayout layout){
    vk::GraphicsPipelineCreateInfo info;
    vk::PipelineVertexInputStateCreateInfo inputState{};
    std::array<vk::PipelineShaderStageCreateInfo,2> shaderStages;
    auto bindingDescriptions = Vertex::inputBindingDescription(0);
    auto attributeDescriptions = Vertex::inputAttributeDescriptions(0);
    vk::PipelineViewportStateCreateInfo viewportState = vulkan::pipeline::initializers::pipelineViewportStateCreateInfo(1, 1);
    if(g_VulkanDevice.IsEnableDynamicRendering()){
        info = vulkan::pipeline::initializers::pipelineCreateInfo(layout, VK_NULL_HANDLE);
    }
    else{
        info = vulkan::pipeline::initializers::pipelineCreateInfo(layout, g_VulkanRenderer.GetRenderPass());
    }
    const vk::ColorComponentFlags colorWriteMask = vk::ColorComponentFlagBits::eR| vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    vk::PipelineColorBlendAttachmentState blendAttachmentState = vulkan::pipeline::initializers::pipelineColorBlendAttachmentState(colorWriteMask, vk::False);
    vk::PipelineColorBlendStateCreateInfo colorBlendState = vulkan::pipeline::initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
    vk::PipelineMultisampleStateCreateInfo multisampleState = vulkan::pipeline::initializers::pipelineMultisampleStateCreateInfo(vk::SampleCountFlagBits::e1);
    vk::PipelineDepthStencilStateCreateInfo depthStencilState = vulkan::pipeline::initializers::pipelineDepthStencilStateCreateInfo(vk::True, vk::True, vk::CompareOp::eLessOrEqual);
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState = vulkan::pipeline::initializers::pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList, vk::False);
    vk::PipelineRasterizationStateCreateInfo rasterizationState = vulkan::pipeline::initializers::pipelineRasterizationStateCreateInfo(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise);
    // std::array<vk::DynamicState, 2> dynamicStateEnables = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    // vk::PipelineDynamicStateCreateInfo dynamicState = vulkan::pipeline::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()));
    vk::Rect2D scissors = vulkan::pipeline::initializers::scissor(g_WindowWidth, g_WindowHeight);
    vk::Viewport viewport = vulkan::pipeline::initializers::viewport(g_WindowWidth, g_WindowHeight);
    viewportState.pScissors = &scissors;
    viewportState.pViewports = &viewport;

    inputState.sType =  vk::StructureType::ePipelineVertexInputStateCreateInfo;
    inputState.vertexBindingDescriptionCount = 1;
    inputState.pVertexBindingDescriptions = &bindingDescriptions;
    inputState.pVertexAttributeDescriptions = attributeDescriptions.data();
    inputState.vertexAttributeDescriptionCount = attributeDescriptions.size();
    shaderStages[0] = vulkan::pipeline::tools::loadShader(device, "shaders/base.vert.spv", vk::ShaderStageFlagBits::eVertex);
    shaderStages[1] = vulkan::pipeline::tools::loadShader(device, "shaders/base.frag.spv", vk::ShaderStageFlagBits::eFragment);
    // pipelineCreateInfo.pTessellationState = ;
    vk::PipelineRenderingCreateInfo renderingInfo;
    if(g_VulkanDevice.IsEnableDynamicRendering()){
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachmentFormats = &g_VulkanRenderer.GetSurfaceFormat();
        renderingInfo.depthAttachmentFormat = g_VulkanRenderer.GetDepthImage().GetFormat();
        info.pNext = &renderingInfo;    
    }
    info.pStages = shaderStages.data();
    info.stageCount = shaderStages.size();

    info.pVertexInputState = &inputState;
    info.pViewportState = &viewportState;
    // info.pDynamicState = &dynamicState;
    info.pColorBlendState = &colorBlendState;
    info.pMultisampleState = &multisampleState;
    info.pDepthStencilState = &depthStencilState;
    info.pInputAssemblyState = &inputAssemblyState;
    info.pRasterizationState = &rasterizationState;
    auto result = device.createGraphicsPipeline(g_Pipeline.cache, info);
    if(result.result == vk::Result::eSuccess){        
        g_Pipeline.fill = result.value;
    }
    inputAssemblyState.topology = vk::PrimitiveTopology::eLineList;
    result = device.createGraphicsPipeline(g_Pipeline.cache, info);
    if(result.result == vk::Result::eSuccess){
        g_Pipeline.wireframe = result.value;
    }
    device.destroyShaderModule(shaderStages[0].module);
    device.destroyShaderModule(shaderStages[1].module);
}
void CreateMVPUBO(const vulkan::Device&device){
    g_MvpUbo.Create(device, sizeof(MVP_UBO), vk::BufferUsageFlagBits::eStorageBuffer, vma::MemoryUsage::eCpuToGpu, true);
    UpdateUniform(device);
    std::vector bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex)
    };
    vulkan::framework::UpdateDescriptorSets(device, bindings, {g_MvpUbo}, {}, g_Set[1]);
}
void DestoryMVPUBO(const vulkan::Device&device){
    g_MvpUbo.Destroy(device);
}
void SetupDescriptorSetLayout(vk::Device device){
    std::array bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex)
    };
    g_CameraSetLayout[0] = device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, bindings));
    std::array mvp_bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex)
    };
    g_CameraSetLayout[1] = device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, mvp_bindings));
}
void Setup(GLFWwindow *window){
    glfwSetKeyCallback(window, keybutton);
    glfwSetScrollCallback(window, mousescroll);
    glfwSetCursorPosCallback(window, mousecursor);
    glfwSetMouseButtonCallback(window, mousebutton);
    // glfwSetFramebufferSizeCallback( window, FramebufferResizeCallback);

    SetupDescriptorSetLayout(g_VulkanDevice);
    
    g_Pipeline.cache = vulkan::pipeline::tools::createPipelineCache(g_VulkanDevice, "GraphicsPipelineCache");

    g_Camera.CreateUniform(g_VulkanDevice);
    g_Camera.UpdateUniform(g_VulkanDevice);

    g_Set = g_VulkanPool.AllocateDescriptorSets(g_VulkanDevice, g_CameraSetLayout);

    CreateMVPUBO(g_VulkanDevice);

    std::vector bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex)
    };
    vulkan::framework::UpdateDescriptorSets(g_VulkanDevice, bindings, { g_Camera.GetUniform() }, {}, g_Set[0]);

    g_Geometry = new Tesseract;
    g_Geometry->Setup(g_VulkanDevice, g_VulkanQueue.graphics, g_VulkanPool);
    g_Geometry->Update();

    CreatePipelineLayout(g_VulkanDevice);
    CreateGraphicsPipeline(g_VulkanDevice, g_Pipeline.layout);

    //imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO&io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;//启用手柄
    io.ConfigNavCursorVisibleAlways = true;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);
    g_VulkanImGui.Setup(g_VulkanDevice, g_VulkanPool);
    g_VulkanImGui.CreateFont("fonts/SourceHanSerifCN-Bold.otf", g_VulkanQueue.graphics, g_VulkanPool);
    if(g_VulkanDevice.IsEnableDynamicRendering()){
        g_VulkanImGui.CreatePipeline(g_VulkanRenderer.GetSurfaceFormat(), g_VulkanRenderer.GetDepthImage().GetFormat(), g_Pipeline.cache);
    }
    else{
        g_VulkanImGui.CreatePipeline(g_VulkanRenderer.GetRenderPass(), g_Pipeline.cache);
    }
}

void Cleanup(const vulkan::Device&device){
    ImGui_ImplGlfw_Shutdown();
    g_VulkanImGui.Cleanup();
    DestoryMVPUBO(device);
    for (size_t i = 0; i < g_CameraSetLayout.size(); i++){
        device.GetDevice().destroyDescriptorSetLayout(g_CameraSetLayout[i]);
    }
    vulkan::pipeline::tools::destroyPipelineCache(device, "GraphicsPipelineCache", g_Pipeline.cache);
    g_Geometry->Cleanup();
    DestroyGraphicsPipeline(device);
    g_Camera.DestroyUniform(device);
    // g_PickingTexture.Cleanup(device);
}
void RecreateSwapchain(void *userData){
    GLFWwindow* window = (GLFWwindow*)userData;
    int32_t width, height;
    do{
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }while(width <= 0 || height <= 0);
    g_VulkanDevice.waitIdle();
    g_WindowWidth = width;
    g_WindowHeight = height;
    //想不调用vkDeviceWaitIdle重建交换链也行, 在重新创建交换链时, VkSwapchainCreateInfoKHR的结构体中oldSwapChain设置为原来的交换链
    g_VulkanRenderer.CleanupSwapchain();
    g_VulkanRenderer.CreateSwapchain(g_WindowWidth, g_WindowHeight);
    g_VulkanRenderer.CreateDepthImage();
    if(!g_VulkanDevice.IsEnableDynamicRendering()){
        g_VulkanRenderer.CreateFrameBuffer();
    }
}
void display(GLFWwindow* window){
    g_VulkanDevice.waitIdle();
    if(g_ImGuiInput.UpdateGeometry){
        g_ImGuiInput.UpdateGeometry = false;
        g_Geometry->Cleanup();
        delete g_Geometry;
        if(g_ImGuiInput.tesseract){
            g_Geometry = new Tesseract;
        }
        else if(g_ImGuiInput.pentatope){
            g_Geometry = new Pentatope;
        }
        else if(g_ImGuiInput.sixteenCell){
            g_Geometry = new SixteenCell;
        }
        else if(g_ImGuiInput.twentyFourCell){
            g_Geometry = new TwentyFourCell;
        }
        else if(g_ImGuiInput.clifford){
            g_Geometry = new Clifford;
        }
        else if(g_ImGuiInput.font){
            g_Geometry = new Font;
        }
        else if(g_ImGuiInput.cylinder){
            g_Geometry = new Cylinder;
        }
        else if(g_ImGuiInput.kleinBottle){
            g_Geometry = new KleinBottle;
        }
        else if(g_ImGuiInput.grid3d){
            g_Geometry = new Grid3D;
        }
        else if(g_ImGuiInput.grid4d){
            g_Geometry = new Grid4D;
        }
        else if(g_ImGuiInput.cylindrical){   
            g_Geometry = new Cylindrical;
        }
        else if(g_ImGuiInput.hypersphere){
            g_Geometry = new Hypersphere;
        }
#ifdef DEBUG
        else if(g_ImGuiInput.testGeometry){
            g_Geometry = new GeometryTest;
        }
#endif
        g_Geometry->Setup(g_VulkanDevice, g_VulkanQueue.graphics, g_VulkanPool);
        g_Geometry->Update(&g_ImGuiInput.parameter);
        UpdateUniform(g_VulkanDevice);
    }
    if(g_VulkanDevice.IsEnableDynamicRendering()){
        g_VulkanRenderer.DynamicRendering(g_VulkanDevice, g_VulkanQueue, RecordCommand, RecreateSwapchain, window);
    }
    else{
        g_VulkanRenderer.Render(g_VulkanDevice, g_VulkanQueue, RecordCommand, RecreateSwapchain, window);
    }
}
bool SelectPhysicalDevice(vk::PhysicalDevice physicalDevice){
    vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();
    return physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu;
}
void SetupVulkan(GLFWwindow *window){
    // if (volkInitialize() != VK_SUCCESS){
    //     std::cerr << "Cannot load Vulkan loader.\n";
    //     return;
    // }
    // VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    uint32_t count;
    const char** instanceExtension = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions(instanceExtension, instanceExtension + count);
#ifndef NDEBUG
    g_VulkanDevice.EnableValidation();
#endif
    g_VulkanDevice.CreateInstance(extensions);
    // VULKAN_HPP_DEFAULT_DISPATCHER.init(g_VulkanDevice.GetInstance());
    // volkLoadInstance(g_VulkanDevice.GetInstance());
    g_VulkanDevice.SelectPhysicalDevice(SelectPhysicalDevice);
    g_VulkanDevice.EnableDynamicRendering();
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(g_VulkanDevice.GetInstance(), window, nullptr, &surface);
    g_VulkanRenderer.SetSurface(surface);
    //版本较低时, 需要开启设备扩展:VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    g_VulkanDevice.CreateDevice(g_VulkanRenderer.GetSurface());
    g_VulkanDevice.CreateAllocator();
    g_VulkanPool.CreatePool(g_VulkanDevice, 5, vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    g_VulkanRenderer.Setup(g_VulkanDevice, g_WindowWidth, g_WindowHeight, g_VulkanPool);
    g_VulkanQueue.CreateQueue(g_VulkanDevice, g_VulkanRenderer.GetSurface());
    const char *deviceType;
    vk::PhysicalDeviceProperties physicalDeviceProperties = g_VulkanDevice.GetPhysicalDeviceProperties();
    switch (physicalDeviceProperties.deviceType){
    case vk::PhysicalDeviceType::eCpu:
        deviceType = "CPU";
        break;
    case vk::PhysicalDeviceType::eDiscreteGpu:
        deviceType = "DISCRETE GPU";
        break;
    case vk::PhysicalDeviceType::eIntegratedGpu:
        deviceType = "INTEGRATED GPU";
        break;
    case vk::PhysicalDeviceType::eVirtualGpu:
        deviceType = "VIRTUAL GPU";
        break;
    default:
        deviceType = "OTHER";
        break;
    }
	printf("gpu name:%s, gpu type:%s\n", physicalDeviceProperties.deviceName, deviceType);
}
void CleanupVulkan(){
    g_VulkanPool.Cleanup(g_VulkanDevice);

    g_VulkanRenderer.Cleanup();

    g_VulkanDevice.Cleanup();
}
#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    if (GLFW_FALSE == glfwInit()) {
        printf("initialize glfw error");
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(g_WindowWidth, g_WindowHeight, "demo", NULL, NULL);
    // Setup Vulkan
    if (!glfwVulkanSupported()){
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    SetupVulkan(window);
    Setup(window);

    const double targetFrameTime = 1.0 / 60.0; // 目标帧时间（例如60 FPS）
    auto previousTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime - previousTime).count();

        if (elapsedTime < targetFrameTime) {
            double sleepTime = (targetFrameTime - elapsedTime) * 1000;
            std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepTime));
        }

        previousTime = std::chrono::high_resolution_clock::now();
        // proccessInput(window);
        display(window);

        glfwPollEvents();
    }
	glfwTerminate();

    g_VulkanDevice.waitIdle();
    Cleanup(g_VulkanDevice);
    CleanupVulkan();
    spdlog::shutdown();
    return 0;
}
#else
int main(){
    if (GLFW_FALSE == glfwInit()) {
        printf("initialize glfw error");
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    GLFWwindow* window = glfwCreateWindow(g_WindowWidth, g_WindowHeight, "demo", NULL, NULL);
    // Setup Vulkan
    if (!glfwVulkanSupported()){
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    SetupVulkan(window);
    Setup(window);

    const double targetFrameTime = 1.0 / 60.0; // 目标帧时间（例如60 FPS）
    auto previousTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime - previousTime).count();

        if (elapsedTime < targetFrameTime) {
            double sleepTime = (targetFrameTime - elapsedTime) * 1000;
            std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepTime));
        }

        previousTime = std::chrono::high_resolution_clock::now();
        // proccessInput(window);
        glfwPollEvents();

        display(window);
    }
	glfwTerminate();

    g_VulkanDevice.waitIdle();
    Cleanup(g_VulkanDevice);
    CleanupVulkan();
    return 0;
}
#endif
