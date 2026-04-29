#include "font.h"
glm::vec3 bezier(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float t) {
    float u = 1.0f - t;
    return p0 * (u*u*u) + p1 * (3*u*u*t) + p2 * (3*u*t*t) + p3 * (t*t*t);
}
glm::vec3 bezierTangent(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float t) {
    float u = 1.0f - t;
    return (p1 - p0) * (3*u*u) + (p2 - p1) * (6*u*t) + (p3 - p2) * (3*t*t);
}

void generateCurvedCylinder(std::vector<Vertex>& outVertices, std::vector<uint16_t>& outIndices,
    const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
    float radius, int samples, int segments
) {
    uint16_t baseIndex = static_cast<uint16_t>(outVertices.size());
    
    std::vector<glm::vec3> centers(samples + 1);
    std::vector<glm::vec3> tangents(samples + 1);
    
    for (int i = 0; i <= samples; ++i) {
        float t = static_cast<float>(i) / samples;
        centers[i] = bezier(p0, p1, p2, p3, t);
        tangents[i] = glm::normalize(bezierTangent(p0, p1, p2, p3, t));
    }
    
    for (int i = 0; i <= samples; ++i) {
        const glm::vec3& center = centers[i];
        const glm::vec3& tangent = tangents[i];
        
        glm::vec3 up(0, 1, 0);
        if (std::abs(glm::dot(tangent, up)) > 0.99f) {
            up = glm::vec3(1, 0, 0);
        }
        
        glm::vec3 right = glm::normalize(glm::cross(up, tangent));
        up = glm::normalize(glm::cross(tangent, right));
        
        for (int j = 0; j < segments; ++j) {
            float angle = 2.0f * M_PI * static_cast<float>(j) / segments;
            
            glm::vec3 offset = right * std::cos(angle) * radius + up * std::sin(angle) * radius;
            glm::vec3 vertexPos = center + offset;
            
            outVertices.emplace_back(glm::vec4(vertexPos, 0), vertexPos); 
        }
    }
    
    for (int i = 0; i < samples; ++i) {
        int row0 = baseIndex + i * segments;
        int row1 = row0 + segments;
        
        for (int j = 0; j < segments; ++j) {
            int next_j = (j + 1) % segments;
            
            outIndices.push_back(row0 + j);
            outIndices.push_back(row1 + j);
            outIndices.push_back(row0 + next_j);
            
            outIndices.push_back(row0 + next_j);
            outIndices.push_back(row1 + j);
            outIndices.push_back(row1 + next_j);
        }
    }
}

Font::Font(/* args */){
}

Font::~Font(){
}
void Font::Cleanup(){
    mGeometry.Destroy(*gpu.device);
    mWireframe.Destroy(*gpu.device);
}

void Font::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Font::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void Font::Update(const void *useData){
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    CylinderParameter parameter = *(CylinderParameter *)useData;
    generateCurvedCylinder(vertices, indices,
        glm::vec3(parameter.point[0][0], parameter.point[0][1], parameter.point[0][2]),
        glm::vec3(parameter.point[1][0], parameter.point[1][1], parameter.point[1][2]),
        glm::vec3(parameter.point[2][0], parameter.point[2][1], parameter.point[2][2]),
        glm::vec3(parameter.point[3][0], parameter.point[3][1], parameter.point[3][2]), parameter.radius, parameter.samples, parameter.segments);
    if(!mGeometry.IsVaildIndex() || !mGeometry.IsVaildVertex()){
        mGeometry.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGeometry.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGeometry.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGeometry.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
    if(!mWireframe.IsVaildIndex() || !mWireframe.IsVaildVertex()){
        mWireframe.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mWireframe.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mWireframe.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mWireframe.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}