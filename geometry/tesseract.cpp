#include "tesseract.h"
void Tesseract::CreateTesseract(){
    const std::array vertices = {
        Vertex(glm::vec4(TESSERACT_NEGATIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE), glm::vec3(1, 0, 0)), // 顶点 0
        Vertex(glm::vec4(TESSERACT_POSITIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE), glm::vec3(0, 1, 0)), // 顶点 1
        Vertex(glm::vec4(TESSERACT_POSITIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE), glm::vec3(0, 0, 1)), // 顶点 2
        Vertex(glm::vec4(TESSERACT_NEGATIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE), glm::vec3(1, 1, 0)), // 顶点 3
        Vertex(glm::vec4(TESSERACT_NEGATIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE), glm::vec3(1, 0, 1)), // 顶点 4
        Vertex(glm::vec4(TESSERACT_POSITIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE), glm::vec3(.5, .5, .5)), // 顶点 5
        Vertex(glm::vec4(TESSERACT_POSITIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE), glm::vec3(1, 1, 1)), // 顶点 6
        Vertex(glm::vec4(TESSERACT_NEGATIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE), glm::vec3(1, .5, .5)), // 顶点 7

        Vertex(glm::vec4(TESSERACT_NEGATIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE), glm::vec3(1, 0, 0)),
        Vertex(glm::vec4(TESSERACT_POSITIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE), glm::vec3(0, 1, 0)),
        Vertex(glm::vec4(TESSERACT_POSITIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE), glm::vec3(0, 0, 1)),
        Vertex(glm::vec4(TESSERACT_NEGATIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE), glm::vec3(1, 1, 0)),
        Vertex(glm::vec4(TESSERACT_NEGATIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE), glm::vec3(1, 0, 1)),
        Vertex(glm::vec4(TESSERACT_POSITIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE), glm::vec3(0, 1, 1)),
        Vertex(glm::vec4(TESSERACT_POSITIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE), glm::vec3(.5, .5, .5)),
        Vertex(glm::vec4(TESSERACT_NEGATIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE), glm::vec3(1, 1, 1))
    };
    const uint16_t indices[] = {
        //w = -1(内层)
        // 前(z = -1): 0,1,2,3
        0, 2, 1,  0, 3, 2,
        // 后 (z = +1): 4,5,6,7  
        4, 5, 6,  4, 6, 7,
        // 左 (x = -1): 0,3,7,4
        0, 7, 3,  0, 4, 7,
        // 右 (x = +1): 1,2,6,5
        1, 6, 2,  1, 5, 6,
        // 底 (y = -1): 0,1,5,4
        0, 1, 5,  0, 5, 4,
        // 顶 (y = +1): 3,2,6,7
        3, 2, 6,  3, 6, 7,

        //w = +1(外层)
        // 前 (z = -1): 8,9,10,11
        8, 9, 10,  8, 10, 11,
        // 后 (z = +1): 12,13,14,15
        12, 14, 13,  12, 15, 14,
        // 左 (x = -1): 8,11,15,12
        8, 11, 15,  8, 15, 12,
        // 右 (x = +1): 9,10,14,13
        9, 14, 10,  9, 13, 14,
        // 底 (y = -1): 8,9,13,12
        8, 13, 9,  8, 12, 13,
        // 顶 (y = +1): 11,10,14,15
        11, 10, 14,  11, 14, 15,

        //z = -1(连接前后-内外)
        // 外 (w = -1): 0,1,2,3
        0, 1, 2,  0, 2, 3,
        // 内 (w = +1): 8,9,10,11
        8, 10, 9,  8, 11, 10,
        // 左 (x = -1): 0,3,11,8
        0, 3, 11,  0, 11, 8,
        // 右 (x = +1): 1,2,10,9
        1, 10, 2,  1, 9, 10,
        // 底 (y = -1): 0,1,9,8
        0, 9, 1,  0, 8, 9,
        // 顶 (y = +1): 3,2,10,11
        3, 2, 10,  3, 10, 11,

        //z = +1(连接前后-内外)
        // 外 (w = -1): 4,5,6,7
        4, 6, 5,  4, 7, 6,
        // 内 (w = +1): 12,13,14,15
        12, 13, 14,  12, 14, 15,
        // 左 (x = -1): 4,7,15,12
        4, 15, 7,  4, 12, 15,
        // 右 (x = +1): 5,6,14,13
        5, 14, 6,  5, 13, 14,
        // 底 (y = -1): 4,5,13,12
        4, 13, 5,  4, 12, 13,
        // 顶 (y = +1): 7,6,14,15
        7, 14, 6,  7, 15, 14,

        //y = -1 (连接上下-内外)
        // 外 (w = -1): 0,1,5,4
        0, 5, 1,  0, 4, 5,
        // 内 (w = +1): 8,9,13,12
        8, 9, 13,  8, 13, 12,
        // 前 (z = -1): 0,1,9,8
        0, 1, 9,  0, 9, 8,
        // 后 (z = +1): 4,5,13,12
        4, 13, 5,  4, 12, 13,
        // 左 (x = -1): 0,4,12,8
        0, 4, 12,  0, 12, 8,
        // 右 (x = +1): 1,5,13,9
        1, 13, 5,  1, 9, 13,

        //y = +1 (连接上下-内外)
        // 外 (w = -1): 3,2,6,7
        3, 6, 2,  3, 7, 6,
        // 内 (w = +1): 11,10,14,15
        11, 10, 14,  11, 14, 15,
        // 前 (z = -1): 3,2,10,11
        3, 2, 10,  3, 10, 11,
        // 后 (z = +1): 7,6,14,15
        7, 14, 6,  7, 15, 14,
        // 左 (x = -1): 3,7,15,11
        3, 7, 15,  3, 15, 11,
        // 右 (x = +1): 2,6,14,10
        2, 14, 6,  2, 10, 14,

        //x = -1 (连接左右-内外)
        // 外 (w = -1): 0,3,7,4
        0, 7, 3,  0, 4, 7,
        // 内 (w = +1): 8,11,15,12
        8, 11, 15,  8, 15, 12,
        // 前 (z = -1): 0,3,11,8
        0, 3, 11,  0, 11, 8,
        // 后 (z = +1): 4,7,15,12
        4, 7, 15,  4, 15, 12,
        // 底 (y = -1): 0,4,12,8
        0, 4, 12,  0, 12, 8,
        // 顶 (y = +1): 3,7,15,11
        3, 15, 7,  3, 11, 15,

        //x = +1 (连接左右-内外)
        // 外 (w = -1): 1,2,6,5
        1, 6, 2,  1, 5, 6,
        // 内 (w = +1): 9,10,14,13
        9, 14, 10,  9, 13, 14,
        // 前 (z = -1): 1,2,10,9
        1, 2, 10,  1, 10, 9,
        // 后 (z = +1): 5,6,14,13
        5, 14, 6,  5, 13, 14,
        // 底 (y = -1): 1,5,13,9
        1, 13, 5,  1, 9, 13,
        // 顶 (y = +1): 2,6,14,10
        2, 14, 6,  2, 10, 14
    };
    if(!mTesseract.IsVaildIndex() || !mTesseract.IsVaildVertex()){
        mTesseract.CreateIndexBuffer(*gpu.device, indices, sizeof(indices), gpu.graphics, *gpu.pool);
        mTesseract.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mTesseract.UpdateIndexData(*gpu.device, indices, gpu.graphics, *gpu.pool);
        mTesseract.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}
void Tesseract::CreateWireframe(){
    const std::array vertices = {
        Vertex(glm::vec4(TESSERACT_NEGATIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE), glm::vec3(1, 0, 0)), // 顶点 0
        Vertex(glm::vec4(TESSERACT_POSITIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE), glm::vec3(0, 1, 0)), // 顶点 1
        Vertex(glm::vec4(TESSERACT_POSITIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE), glm::vec3(0, 0, 1)), // 顶点 2
        Vertex(glm::vec4(TESSERACT_NEGATIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE), glm::vec3(1, 1, 0)), // 顶点 3
        Vertex(glm::vec4(TESSERACT_NEGATIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE), glm::vec3(1, 0, 1)), // 顶点 4
        Vertex(glm::vec4(TESSERACT_POSITIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE), glm::vec3(.5, .5, .5)), // 顶点 5
        Vertex(glm::vec4(TESSERACT_POSITIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE), glm::vec3(1, 1, 1)), // 顶点 6
        Vertex(glm::vec4(TESSERACT_NEGATIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE), glm::vec3(1, .5, .5)), // 顶点 7

        Vertex(glm::vec4(TESSERACT_NEGATIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE), glm::vec3(1, 0, 0)),
        Vertex(glm::vec4(TESSERACT_POSITIVE, TESSERACT_NEGATIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE), glm::vec3(0, 1, 0)),
        Vertex(glm::vec4(TESSERACT_POSITIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE), glm::vec3(0, 0, 1)),
        Vertex(glm::vec4(TESSERACT_NEGATIVE,  TESSERACT_POSITIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE), glm::vec3(1, 1, 0)),
        Vertex(glm::vec4(TESSERACT_NEGATIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE), glm::vec3(1, 0, 1)),
        Vertex(glm::vec4(TESSERACT_POSITIVE, TESSERACT_NEGATIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE), glm::vec3(0, 1, 1)),
        Vertex(glm::vec4(TESSERACT_POSITIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE), glm::vec3(.5, .5, .5)),
        Vertex(glm::vec4(TESSERACT_NEGATIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE,  TESSERACT_POSITIVE), glm::vec3(1, 1, 1))
    };
    const uint16_t indices[] = {
        // 内立方体边 (w=-1层，顶点0-7) - 12条边
        0,1, 1,2, 2,3, 3,0,  // 前面
        4,5, 5,6, 6,7, 7,4,  // 后面  
        0,4, 1,5, 2,6, 3,7,  // 侧面
        
        // 外立方体边 (w=+1层，顶点8-15) - 12条边
        8,9, 9,10, 10,11, 11,8,    // 前面
        12,13, 13,14, 14,15, 15,12, // 后面
        8,12, 9,13, 10,14, 11,15,   // 侧面
        
        // 超边（连接内外）- 8条边
        0,8, 1,9, 2,10, 3,11,   // 前面对应
        4,12, 5,13, 6,14, 7,15  // 后面对应
    };
    if(!mWireframe.IsVaildIndex() || !mWireframe.IsVaildVertex()){
        mWireframe.CreateIndexBuffer(*gpu.device, indices, sizeof(indices), gpu.graphics, *gpu.pool);
        mWireframe.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mWireframe.UpdateIndexData(*gpu.device, indices, gpu.graphics, *gpu.pool);
        mWireframe.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}
Tesseract::Tesseract(/* args */){
}

Tesseract::~Tesseract(){
}
void Tesseract::Cleanup(){
    mTesseract.Destroy(*gpu.device);
    mWireframe.Destroy(*gpu.device);
}

void Tesseract::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mTesseract.Bind(command);
    mTesseract.Draw(command);
}
void Tesseract::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void Tesseract::Update(const void *useData){
    CreateTesseract();
    CreateWireframe();
}
