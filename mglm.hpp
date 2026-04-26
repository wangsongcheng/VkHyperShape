#ifndef MGLM_HPP
#define MGLM_HPP
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
namespace mglm{
    struct vec5 {
        float x, y, z, w, v;
        vec5() : x(0), y(0), z(0), w(0), v(0) {}
        vec5(float x, float y, float z, float w, float v) : x(x), y(y), z(z), w(w), v(v) {}
        explicit vec5(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar), v(scalar) {}
        vec5 operator-() const noexcept{ return vec5(-x, -y, -z, -w, -v); }
        
        vec5 operator+(const vec5& o) const noexcept{ return vec5(x+o.x, y+o.y, z+o.z, w+o.w, v+o.v); }
        vec5 operator-(const vec5& o) const noexcept{ return vec5(x-o.x, y-o.y, z-o.z, w-o.w, v-o.v); }
        
        vec5& operator+=(const vec5& o) { x+=o.x; y+=o.y; z+=o.z; w+=o.w; v+=o.v; return *this; }
        vec5& operator-=(const vec5& o) { x-=o.x; y-=o.y; z-=o.z; w-=o.w; v-=o.v; return *this; }
        
        vec5 operator*(float s) const noexcept{ return vec5(x*s, y*s, z*s, w*s, v*s); }
        vec5 operator/(float s) const noexcept{ return vec5(x/s, y/s, z/s, w/s, v/s); }
        vec5& operator*=(float s) { x*=s; y*=s; z*=s; w*=s; v*=s; return *this; }
        vec5& operator/=(float s) { x/=s; y/=s; z/=s; w/=s; v/=s; return *this; }
        
        float& operator[](int i) { return (&x)[i]; }
        const float& operator[](int i) const noexcept{ return (&x)[i]; }
        
        bool operator==(const vec5& o)const noexcept{ float eps = 1e-6f;return std::abs(x - o.x) < eps && std::abs(y - o.y) < eps && std::abs(z - o.z) < eps && std::abs(w - o.w) < eps && std::abs(v - o.v) < eps;}
        bool operator!=(const vec5& o) const noexcept{ return !(*this == o); }
    };
    struct vec4{
        float x, y, z, w;
        vec4() : x(0), y(0), z(0), w(0) {}
        vec4(const vec5&v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
        vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        explicit vec4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
        
        vec4 operator-() const noexcept{ return vec4(-x, -y, -z, -w); }
        
        vec4 operator+(const vec4& o) const noexcept{ return vec4(x+o.x, y+o.y, z+o.z, w+o.w); }
        vec4 operator-(const vec4& o) const noexcept{ return vec4(x-o.x, y-o.y, z-o.z, w-o.w); }
        
        vec4& operator+=(const vec4& o) { x+=o.x; y+=o.y; z+=o.z; w+=o.w; return *this; }
        vec4& operator-=(const vec4& o) { x-=o.x; y-=o.y; z-=o.z; w-=o.w; return *this; }
        
        vec4 operator*(float s) const noexcept{ return vec4(x*s, y*s, z*s, w*s); }
        vec4 operator/(float s) const noexcept{ return vec4(x/s, y/s, z/s, w/s); }
        vec4& operator*=(float s) { x*=s; y*=s; z*=s; w*=s; return *this; }
        vec4& operator/=(float s) { x/=s; y/=s; z/=s; w/=s; return *this; }
        
        float& operator[](int i) { return (&x)[i]; }
        const float& operator[](int i) const noexcept{ return (&x)[i]; }
        
        bool operator==(const vec4& o)const noexcept{ float eps = 1e-6f;return std::abs(x - o.x) < eps && std::abs(y - o.y) < eps && std::abs(z - o.z) < eps && std::abs(w - o.w) < eps;}
        bool operator!=(const vec4& o) const noexcept{ return !(*this == o); }
    };
    struct mat5 {
        vec5 data[5];
        
        mat5() = default;
        mat5(const vec5& c0, const vec5& c1, const vec5& c2, const vec5& c3, const vec5& c4) {
            data[0]=c0; data[1]=c1; data[2]=c2; data[3]=c3; data[4]=c4;
        }
        explicit mat5(float s) {
            data[0] = vec5(s,0,0,0,0);
            data[1] = vec5(0,s,0,0,0);
            data[2] = vec5(0,0,s,0,0);
            data[3] = vec5(0,0,0,s,0);
            data[4] = vec5(0,0,0,0,s);
        }
        vec5& operator[](int col) { return data[col]; }
        const vec5& operator[](int col) const { return data[col]; }
        float& at(int row, int col) { return data[col][row]; }
        const float& at(int row, int col) const { return data[col][row]; }
        mat5 operator+(const mat5& o) const {
            return mat5(data[0]+o[0], data[1]+o[1], data[2]+o[2], data[3]+o[3], data[4]+o[4]);
        }
        mat5 operator-(const mat5& o) const {
            return mat5(data[0]-o[0], data[1]-o[1], data[2]-o[2], data[3]-o[3], data[4]-o[4]);
        }
        mat5 operator*(float s) const {
            return mat5(data[0]*s, data[1]*s, data[2]*s, data[3]*s, data[4]*s);
        }
        vec5 operator*(const vec5& v) const {
            // M * v = v[0]*col0 + v[1]*col1 + ... + v[4]*col4
            return data[0]*v[0] + data[1]*v[1] + data[2]*v[2] + data[3]*v[3] + data[4]*v[4];
        }
        mat5 operator*(const mat5& o) const {
            mat5 result;
            // result[col][row] = sum_k this[k][row] * o[col][k]
            // 即 result 的第 col 列 = this * (o 的第 col 列)
            for (int col = 0; col < 5; ++col) {
                result[col] = (*this) * o[col];
            }
            return result;
        }
    };
    struct mat4 {
        vec4 data[4];
        
        mat4() = default;
        mat4(const vec4& c0, const vec4& c1, const vec4& c2, const vec4& c3) {
            data[0] = c0; data[1] = c1; data[2] = c2; data[3] = c3;
        }
        explicit mat4(float s) {
            data[0] = vec4(s, 0, 0, 0);
            data[1] = vec4(0, s, 0, 0);
            data[2] = vec4(0, 0, s, 0);
            data[3] = vec4(0, 0, 0, s);
        }
        mat4(const mat5& m, int skipRow, int skipCol) {
            int subRow = 0;
            for (int row = 0; row < 5; ++row) {
                if (row == skipRow) continue;
                int subCol = 0;
                for (int col = 0; col < 5; ++col) {
                    if (col == skipCol) continue;
                    data[subCol][subRow] = m.at(row, col);
                    ++subCol;
                }
                ++subRow;
            }
        }
        mat4(const mat5& m) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    data[j][i] = m.at(i, j);
                }
            }
        }
        
        vec4& operator[](int col) { return data[col]; }
        const vec4& operator[](int col) const { return data[col]; }
        float& at(int row, int col) { return data[col][row]; }
        const float& at(int row, int col) const { return data[col][row]; }
        
        mat4 operator+(const mat4& o) const {
            return mat4(data[0]+o[0], data[1]+o[1], data[2]+o[2], data[3]+o[3]);
        }
        mat4 operator-(const mat4& o) const {
            return mat4(data[0]-o[0], data[1]-o[1], data[2]-o[2], data[3]-o[3]);
        }
        mat4 operator*(float s) const {
            return mat4(data[0]*s, data[1]*s, data[2]*s, data[3]*s);
        }
        vec4 operator*(const vec4& v) const {
            return data[0]*v[0] + data[1]*v[1] + data[2]*v[2] + data[3]*v[3];
        }
        mat4 operator*(const mat4& o) const {
            mat4 result;
            for (int col = 0; col < 4; ++col) {
                result[col] = (*this) * o[col];
            }
            return result;
        }
        mat4& operator=(const mat4& other) {
            if (this != &other) {
                data[0] = other[0];
                data[1] = other[1];
                data[2] = other[2];
                data[3] = other[3];
            }
            return *this;
        }
        mat4& operator=(const mat5& m) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    data[j][i] = m.at(i, j);
                }
            }
            return *this;
        }
    };
    
    template<typename VecType>
    inline VecType operator*(float s, const VecType& v) { return v * s; }

    inline float dot(const vec4& a, const vec4& b) {
        return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
    }
    inline float dot(const vec5& a, const vec5& b) {
        return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w + a.v*b.v;
    }

    template<typename VecType>
    inline float length(const VecType& v) { return std::sqrt(dot(v, v)); }
    template<typename VecType>
    inline float distance(const VecType& a, const VecType& b) { return length(a - b); }

    template<typename VecType>
    inline VecType normalize(const VecType& v) {
        float len = length(v);
        return (len > 0) ? (v / len) : v;
    }

    template<typename VecType>
    inline VecType mix(const VecType& a, const VecType& b, float t) {
        return a * (1.0f - t) + b * t;
    }
    inline vec5 abs(const vec5& v) {
        return vec5(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w), std::abs(v.v));
    }

    inline vec4 abs(const vec4& v) {
        return vec4(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
    }
    inline vec4 operator*(const vec4& a, const vec4& b) {
        return vec4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
    }
    inline vec5 operator*(const vec5& a, const vec5& b) {
        return vec5(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w, a.v*b.v);
    }
    /*
        i       j       k      l
        a.x  a.y  a.z  a.w
        b.x  b.y  b.z  b.w
        c.x  c.y  c.z  c.w
        //余子式+行列式
        a.y  a.z  a.w //x，其他分量同理
        b.y  b.z  b.w
        c.y  c.z  c.w
    */
    inline vec4 cross(const vec4& a, const vec4& b, const vec4& c) {
        return vec4(
            a.y * (b.z * c.w - b.w * c.z) - a.z * (b.y * c.w - b.w * c.y) + a.w * (b.y * c.z - b.z * c.y),
            -a.x * (b.z * c.w - b.w * c.z) + a.z * (b.x * c.w - b.w * c.x) - a.w * (b.x * c.z - b.z * c.x),
            a.x * (b.y * c.w - b.w * c.y) - a.y * (b.x * c.w - b.w * c.x) + a.w * (b.x * c.y - b.y * c.x),
            -a.x * (b.y * c.z - b.z * c.y) + a.y * (b.x * c.z - b.z * c.x) - a.z * (b.x * c.y - b.y * c.x)
        );
    }
    inline vec5 cross(const vec5& a, const vec5& b, const vec5& c, const vec5& d) {
        return vec5(
            a.y * (b.z * (c.w * d.v - c.v * d.w) - b.w * (c.z * d.v - c.v * d.z) + b.v * (c.z * d.w - c.w * d.z)) - a.z * (b.y * (c.w * d.v - c.v * d.w) - b.w * (c.y * d.v - c.v * d.y) + b.v * (c.y * d.w - c.w * d.y)) +
            a.w * (b.y * (c.z * d.v - c.v * d.z) - b.z * (c.y * d.v - c.v * d.y) + b.v * (c.y * d.z - c.z * d.y)) - a.v * (b.y * (c.z * d.w - c.w * d.z) - b.z * (c.y * d.w - c.w * d.y) + b.w * (c.y * d.z - c.z * d.y)),
            //y
            -a.x * (b.z * (c.w * d.v - c.v * d.w) - b.w * (c.z * d.v - c.v * d.z) + b.v * (c.z * d.w - c.w * d.z)) + a.z * (b.x * (c.w * d.v - c.v * d.w) - b.w * (c.x * d.v - c.v * d.x) + b.v * (c.x * d.w - c.w * d.x)) -
            a.w * (b.x * (c.z * d.v - c.v * d.z) - b.z * (c.x * d.v - c.v * d.x) + b.v * (c.x * d.z - c.z * d.x)) + a.v * (b.x * (c.z * d.w - c.w * d.z) - b.z * (c.x * d.w - c.w * d.x) + b.w * (c.x * d.z - c.z * d.x)),
            //z
            a.x * (b.y * (c.w * d.v - c.v * d.w) - b.w * (c.y * d.v - c.v * d.y) + b.v * (c.y * d.w - c.w * d.y)) - a.y * (b.x * (c.w * d.v - c.v * d.w) - b.w * (c.x * d.v - c.v * d.x) + b.v * (c.x * d.w - c.w * d.x)) +
            a.w * (b.x * (c.y * d.v - c.v * d.y) - b.y * (c.x * d.v - c.v * d.x) +b.v * (c.x * d.y - c.y * d.x)) - a.v * (b.x * (c.y * d.w - c.w * d.y) - b.y * (c.x * d.w - c.w * d.x) + b.w * (c.x * d.y - c.y * d.x)),
            //w
            -a.x * (b.y * (c.z * d.v - c.v * d.z) - b.z * (c.y * d.v - c.v * d.y) + b.v * (c.y * d.z - c.z * d.y)) + a.y * (b.x * (c.z * d.v - c.v * d.z) - b.z * (c.x * d.v - c.v * d.x) + b.v * (c.x * d.z - c.z * d.x)) -
            a.z * (b.x * (c.y * d.v - c.v * d.y) - b.y * (c.x * d.v - c.v * d.x) + b.v * (c.x * d.y - c.y * d.x)) + a.v * (b.x * (c.y * d.z - c.z * d.y) - b.y * (c.x * d.z - c.z * d.x) +b.z * (c.x * d.y - c.y * d.x)),
            //v
            a.x * (b.y * (c.z * d.w - c.w * d.z) - b.z * (c.y * d.w - c.w * d.y) + b.w * (c.y * d.z - c.z * d.y)) - a.y * (b.x * (c.z * d.w - c.w * d.z) - b.z * (c.x * d.w - c.w * d.x) + b.w * (c.x * d.z - c.z * d.x)) +
            a.z * (b.x * (c.y * d.w - c.w * d.y) - b.y * (c.x * d.w - c.w * d.x) + b.w * (c.x * d.y - c.y * d.x)) - a.w * (b.x * (c.y * d.z - c.z * d.y) - b.y * (c.x * d.z - c.z * d.x) +b.z * (c.x * d.y - c.y * d.x))
        );
    }
    inline mat4 operator*(float s, const mat4& m) { return m * s; }
    inline mat4 transpose(const mat4& m) {
        mat4 t;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                t[i][j] = m[j][i];
        return t;
    }

    inline mat4 outerProduct(const vec4& a, const vec4& b) {
        mat4 m;
        for (int col = 0; col < 4; ++col)
            for (int row = 0; row < 4; ++row)
                m[row][col] = a[row] * b[col];
        return m;
    }

    inline float determinant(const mat4& m) {
        return dot(m[0], cross(m[1], m[2], m[3]));
    }
    inline float determinant(const mat5& m) {
        return dot(m[0], cross(m[1], m[2], m[3], m[4]));
    }

    inline mat5 operator*(float s, const mat5& m) { return m * s; }

    template<typename MatType>
    inline MatType diagonal(float s) { return MatType(s); }

    // 外积 (Outer Product): result = a * b^T(矩阵)
    inline mat5 outerProduct(const vec5& a, const vec5& b) {
        mat5 m;
        for (int col = 0; col < 5; ++col)
            for (int row = 0; row < 5; ++row)
                m[row][col] = a[row] * b[col];
        return m;
    }
    inline mat5 transpose(const mat5& m) {
        mat5 t;
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j)
                t[i][j] = m[j][i];
        return t;
    }
    inline mat5 inverse(const mat5& m) {
        float det = determinant(m);
        const float eps = 1e-6f;
        if (std::abs(det) < eps) {
            return mat5(1.0f); 
        }
        float invDet = 1.0f / det;
        mat5 inv;
        for (int col = 0; col < 5; ++col) {
            for (int row = 0; row < 5; ++row) {
                mat4 subMatrix(m, col, row);
                float minor = determinant(subMatrix);
                //加上代数余子式的符号:(-1)^(col+row)
                float cofactor = ((row + col) % 2 == 0) ? minor : -minor;
                inv[col][row] = cofactor * invDet;
            }
        }
        return inv;
    }

}
#endif