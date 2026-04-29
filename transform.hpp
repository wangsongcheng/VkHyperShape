#ifndef TRANS_FORM_HPP
#define TRANS_FORM_HPP
#include "mglm.hpp"
namespace mglm{
    struct Plane {
        vec4 u;
        vec4 v;
        Plane()=default;
        Plane(const vec4& u, const vec4& v) : u(u), v(v) {}

        bool operator==(const Plane&plane)const noexcept{
            return u == plane.u && v == plane.v;
        }
        Plane orthonormalize() const noexcept{
            vec4 newU = normalize(u);
            // v在newU上的投影 = dot(v, newU) * newU
            vec4 newV = v - dot(v, newU) * newU;
            return Plane(newU, normalize(newV));
        }
        vec4 orthogonalize(const vec4& vec) {
            const float eps = 1e-6f;
            const mat4 basic = mat4(1.0f);
            for (size_t i = 0; i < 4; i++){
                if (std::abs(dot(vec, basic[i])) < eps) {
                    return basic[i];
                }                
            }            
            vec4 result;
            vec4 absVec = abs(vec);
            int maxIdx = 0;
            float maxVal = absVec[0];
            for (int i = 1; i < 4; ++i) {
                if (absVec[i] > maxVal) {
                    maxVal = absVec[i];
                    maxIdx = i;
                }
            }
            switch (maxIdx) {
                case 0: // x最大
                    result = vec4(-vec.y, vec.x, 0.0f, 0.0f);
                    break;
                case 1: // y最大
                    result = vec4(vec.y, -vec.x, 0.0f, 0.0f);
                    break;
                case 2: // z最大
                    result = vec4(0.0f, -vec.z, vec.y, 0.0f);
                    break;
                case 3: // w最大
                    result = vec4(0.0f, 0.0f, -vec.w, vec.z);
                    break;
            }
            return normalize(result);
        }
    };
    namespace planes {
        const Plane XY(vec4(1,0,0,0), vec4(0,1,0,0));
        const Plane XZ(vec4(1,0,0,0), vec4(0,0,1,0));
        const Plane XW(vec4(1,0,0,0), vec4(0,0,0,1));
        const Plane YZ(vec4(0,1,0,0), vec4(0,0,1,0));
        const Plane YW(vec4(0,1,0,0), vec4(0,0,0,1));
        const Plane ZW(vec4(0,0,1,0), vec4(0,0,0,1));
    }
    inline Plane abs(const Plane& p) {
        return Plane(abs(p.u), abs(p.v));
    }
    inline Plane orthogonal(const Plane& plane) {
        Plane orthoPlane = plane.orthonormalize();
        const vec4& u = orthoPlane.u, & v = orthoPlane.v;
        
        vec4 w1;
        const mat4 basis = mat4(1.0f);
        for (int i = 0; i < 4; ++i) {
            vec4 candidate = basis[i];
            vec4 proj = dot(candidate, u) * u + dot(candidate, v) * v;
            w1 = candidate - proj;
            
            if (length(w1) > 1e-6f) {
                w1 = normalize(w1);
                vec4 w2 = cross(u, v, w1);
                w2 = normalize(w2);
                return Plane(w1, w2);
            }
        }

        const vec4 altBasis[4] = {
            vec4(1, 1, 0, 0),
            vec4(0, 1, 1, 0),
            vec4(0, 0, 1, 1),
            vec4(1, 0, 0, 1)
        };
        
        for (int i = 0; i < 4; ++i) {
            vec4 candidate = altBasis[i];
            vec4 proj = dot(candidate, u) * u + dot(candidate, v) * v;
            w1 = candidate - proj;
            
            if (length(w1) > 1e-6f) {
                w1 = normalize(w1);
                vec4 w2 = cross(u, v, w1);
                w2 = normalize(w2);
                return Plane(w1, w2);
            }
        }
        w1 = vec4(0, 0, 1, 0);
        vec4 w2 = cross(u, v, w1);
        w2 = normalize(w2);
        return Plane(w1, w2);
    }
    inline mat5 rotate(float angle, const Plane& plane) {
        Plane p = plane.orthonormalize();

        vec4 u = p.u, v = p.v;
        mat5 I = mglm::mat5(1.0f);
        float c = std::cos(angle), s = std::sin(angle);        
        vec5 u5(u.x, u.y, u.z, u.w, 0.0f), v5(v.x, v.y, v.z, v.w, 0.0f);
        
        // (cosθ-1)(uu^T + vv^T)
        mat5 term1 = (c - 1.0f) * (outerProduct(u5, u5) + outerProduct(v5, v5));
        // sinθ(uv^T - vu^T)  
        mat5 term2 = s * (outerProduct(u5, v5) - outerProduct(v5, u5));
        return I + term1 + term2;
    }
    inline mat5 rotate(const mat5& m, float theta, const Plane& plane, float theta1, const Plane& plane1) {
        mat5 R1 = rotate(theta, plane);
        mat5 R2 = rotate(theta1, plane1);
        // 组合：m * R1 * R2(先应用R1，再R2)
        return m * R1 * R2;
    }
    inline mat5 lookAt(const vec4& eye, const vec4& target, const vec4& up, const vec4& over) {
        vec4 f = normalize(target - eye);
        vec4 s = normalize(cross(over, up, f));
        vec4 u = normalize(cross(f, s, over));
        vec4 o = normalize(cross(s, u, f));
        
        mat5 view = mglm::mat5(1.0f);
        view[0][0] = s.x;   view[1][0] = s.y;   view[2][0] = s.z;   view[3][0] = s.w;
        view[0][1] = u.x;   view[1][1] = u.y;   view[2][1] = u.z;   view[3][1] = u.w;
        view[0][2] = o.x;   view[1][2] = o.y;   view[2][2] = o.z;   view[3][2] = o.w;
        view[0][3] = -f.x;  view[1][3] = -f.y;  view[2][3] = -f.z;  view[3][3] = -f.w;
        
        view[4][0] = -dot(s, eye);
        view[4][1] = -dot(u, eye);
        view[4][2] = -dot(o, eye);
        view[4][3] = dot(f, eye);
        
        return view;
    }
    inline mat5 lookAt(const vec4& eye, const vec4& target, const vec4& up) {
        vec4 forward4 = normalize(target - eye);
        vec4 over(0, 0, 0, 1);
        if (std::abs(dot(over, forward4)) > 0.99f) {
            over = vec4(0, 0, 1, 0);
        }
        return lookAt(eye, target, up, over);
    }
    inline mat5 perspective(float fovY, float aspect, float wNear, float wFar) {
        mat5 m(0.0f);
        
        float tanHalfFovY = std::tan(fovY / 2.0f);
        
        float yScale = 1.0f / tanHalfFovY;
        float xScale = yScale / aspect;
        float zScale = yScale / aspect;
        
        m[0][0] = xScale;
        m[1][1] = yScale;
        m[2][2] = zScale;

        m.at(3, 3) = -(wFar + wNear) / (wFar - wNear);
        m.at(4, 3) = -1.0f;
        m.at(3, 4) = -(2.0f * wFar * wNear) / (wFar - wNear);
        
        return m;
    }
    inline mat5 perspective(float fovXY, float fovYZ, float fovZW, float near, float far) {
        mat5 proj;
        float cotXY = 1.0f / std::tan(fovXY * 0.5f);
        float cotYZ = 1.0f / std::tan(fovYZ * 0.5f);
        float cotZW = 1.0f / std::tan(fovZW * 0.5f);
        float A = -(far + near) / (far - near);
        float B = -2.0f * far * near / (far - near);

        proj[0] = vec5(cotXY, 0, 0, 0, 0);
        proj[1] = vec5(0, cotYZ, 0, 0, 0);
        proj[2] = vec5(0, 0, cotZW, 0, 0);
        proj[3] = vec5(0, 0, 0, A, -1.0f);
        proj[4] = vec5(0, 0, 0, B, 0.0f);
        return proj;
    }
    inline mat5 ortho(float left, float right, float bottom, float top, float zNear, float zFar, float wNear, float wFar) {
        mat5 m = mglm::mat5(1.0f);
        m[0][0] = 2.0f / (right - left);
        m[1][1] = 2.0f / (top - bottom);
        m[2][2] = 2.0f / (zFar - zNear);
        m[3][3] = -2.0f / (wFar - wNear);
        
        m[4][0] = -(right + left) / (right - left);
        m[4][1] = -(top + bottom) / (top - bottom);
        m[4][2] = -(zFar + zNear) / (zFar - zNear);
        m[4][3] = -(wFar + wNear) / (wFar - wNear);
        
        return m;
    }
    inline mglm::mat5 scale(const mglm::mat5& m, const mglm::vec4& factors) {
        mglm::mat5 scaleMat(1.0f);
        scaleMat[0][0] = factors.x;
        scaleMat[1][1] = factors.y;
        scaleMat[2][2] = factors.z;
        scaleMat[3][3] = factors.w;
        return m * scaleMat;
    }
    inline mglm::mat5 translate(const mglm::mat5& m, const mglm::vec4& pos) {
        mglm::mat5 transMat(1.0f);
        transMat[4][0] = pos.x;
        transMat[4][1] = pos.y;
        transMat[4][2] = pos.z;
        transMat[4][3] = pos.w;
        return m * transMat;
    }
}
#endif
