#include "cylinder.h"
mglm::vec4 bezier(const mglm::vec4& p0, const mglm::vec4& p1, const mglm::vec4& p2, const mglm::vec4& p3, float t) {
    float u = 1.0f - t;
    return p0 * (u*u*u) + p1 * (3*u*u*t) + p2 * (3*u*t*t) + p3 * (t*t*t);
}

// 四维贝塞尔曲线求导
mglm::vec4 bezierTangent(const mglm::vec4& p0, const mglm::vec4& p1, const mglm::vec4& p2, const mglm::vec4& p3, float t) {
    float u = 1.0f - t;
    return (p1 - p0) * (3*u*u) + (p2 - p1) * (6*u*t) + (p3 - p2) * (3*t*t);
}

// 构建与切向量 T 正交的三维初始标架 (N1, N2, N3)
void findInitialFrame(const mglm::vec4& T, mglm::vec4& N1, mglm::vec4& N2, mglm::vec4& N3) {
    mglm::vec4 axes[4] = {
        mglm::vec4(1,0,0,0), mglm::vec4(0,1,0,0),
        mglm::vec4(0,0,1,0), mglm::vec4(0,0,0,1)
    };

    // 排除与 T 最平行的轴，用剩下的 3 个轴做 Gram-Schmidt 正交化
    int skip = 0;
    float maxDot = std::abs(mglm::dot(T, axes[0]));
    for (int i = 1; i < 4; ++i) {
        float d = std::abs(mglm::dot(T, axes[i]));
        if (d > maxDot) { maxDot = d; skip = i; }
    }

    mglm::vec4 a, b, c;
    int idx = 0;
    for (int i = 0; i < 4; ++i) {
        if (i == skip) continue;
        if (idx == 0) a = axes[i];
        else if (idx == 1) b = axes[i];
        else c = axes[i];
        idx++;
    }

    N1 = a - T * mglm::dot(T, a);
    N1 = mglm::normalize(N1);

    N2 = b - T * mglm::dot(T, b) - N1 * mglm::dot(N1, b);
    N2 = mglm::normalize(N2);

    N3 = c - T * mglm::dot(T, c) - N1 * mglm::dot(N1, c) - N2 * mglm::dot(N2, c);
    N3 = mglm::normalize(N3);
}

// 四维标架平行移动 (将三个法向量一起投影并重新正交化，消除多余旋转自由度)
bool transportFrame(const mglm::vec4& N1_prev, const mglm::vec4& N2_prev, const mglm::vec4& N3_prev, const mglm::vec4& T_curr, mglm::vec4& N1_curr, mglm::vec4& N2_curr, mglm::vec4& N3_curr) {
    mglm::vec4 u1 = N1_prev - T_curr * mglm::dot(T_curr, N1_prev);
    mglm::vec4 u2 = N2_prev - T_curr * mglm::dot(T_curr, N2_prev);
    mglm::vec4 u3 = N3_prev - T_curr * mglm::dot(T_curr, N3_prev);

    float len1 = mglm::length(u1);
    if (len1 < 1e-4f) return false; 
    N1_curr = u1 / len1;

    u2 = u2 - N1_curr * mglm::dot(N1_curr, u2);
    float len2 = mglm::length(u2);
    if (len2 < 1e-4f) return false;
    N2_curr = u2 / len2;

    u3 = u3 - N1_curr * mglm::dot(N1_curr, u3) - N2_curr * mglm::dot(N2_curr, u3);
    float len3 = mglm::length(u3);
    if (len3 < 1e-4f) return false;
    N3_curr = u3 / len3;

    return true;
}
void generateCurvedCylinder(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices,
    const mglm::vec4& p0, const mglm::vec4& p1, const mglm::vec4& p2, const mglm::vec4& p3,
    float radius, int samples, int segments
) {
    uint16_t baseIndex = static_cast<uint16_t>(vertices.size());
    
    std::vector<mglm::vec4> centers(samples + 1);
    std::vector<mglm::vec4> tangents(samples + 1);
    std::vector<mglm::vec4> normals1(samples + 1);
    std::vector<mglm::vec4> normals2(samples + 1);
    std::vector<mglm::vec4> normals3(samples + 1); // 用于锚定姿态，不直接参与画圆
    
    for (int i = 0; i <= samples; ++i) {
        float t = static_cast<float>(i) / samples;
        centers[i] = bezier(p0, p1, p2, p3, t);
        tangents[i] = mglm::normalize(bezierTangent(p0, p1, p2, p3, t));
    }
    
    findInitialFrame(tangents[0], normals1[0], normals2[0], normals3[0]);
    
    for (int i = 1; i <= samples; ++i) {
        if (!transportFrame(normals1[i-1], normals2[i-1], normals3[i-1],
                              tangents[i], normals1[i], normals2[i], normals3[i])) {
            findInitialFrame(tangents[i], normals1[i], normals2[i], normals3[i]);
        }
    }
    
    for (int i = 0; i <= samples; ++i) {
        const mglm::vec4& center = centers[i];
        const mglm::vec4& N1 = normals1[i];
        const mglm::vec4& N2 = normals2[i];
        
        for (int j = 0; j < segments; ++j) {
            float angle = 2.0f * M_PI * static_cast<float>(j) / segments;
            
            mglm::vec4 offset = N1 * (std::cos(angle) * radius) + N2 * (std::sin(angle) * radius);
            mglm::vec4 vertexPos = center + offset;
            
            vertices.push_back(Vertex(glm::vec4(vertexPos.x, vertexPos.y, vertexPos.z, vertexPos.w), glm::vec3(vertexPos.x)));
        }
    }
    
    for (int i = 0; i < samples; ++i) {
        int row0 = baseIndex + i * segments;
        int row1 = row0 + segments;
        
        for (int j = 0; j < segments; ++j) {
            int next_j = (j + 1) % segments;
            
            indices.push_back(row0 + j);
            indices.push_back(row1 + j);
            indices.push_back(row0 + next_j);
            
            indices.push_back(row0 + next_j);
            indices.push_back(row1 + j);
            indices.push_back(row1 + next_j);
        }
    }
}
Cylinder::Cylinder(/* args */){
}

Cylinder::~Cylinder(){
}
void Cylinder::Cleanup(){
    mGeometry.Destroy(*gpu.device);
    mWireframe.Destroy(*gpu.device);
}

void Cylinder::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Cylinder::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void Cylinder::Update(const void *useData){
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    CylinderParameter parameter = *(CylinderParameter *)useData;
    generateCurvedCylinder(vertices, indices,
        mglm::vec4(parameter.point[0][0], parameter.point[0][1], parameter.point[0][2], parameter.point[0][3]),
        mglm::vec4(parameter.point[1][0], parameter.point[1][1], parameter.point[1][2], parameter.point[1][3]),
        mglm::vec4(parameter.point[2][0], parameter.point[2][1], parameter.point[2][2], parameter.point[2][3]),
        mglm::vec4(parameter.point[3][0], parameter.point[3][1], parameter.point[3][2], parameter.point[3][3]), parameter.radius, parameter.samples, parameter.segments);
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