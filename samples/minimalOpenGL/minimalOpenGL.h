/**
  \file minimalOpenGL/minimalOpenGL.h
  \author Morgan McGuire, http://graphics.cs.williams.edu

  Minimal headers emulating a basic set of 3D graphics classes.

  All 3D math from http://graphicscodex.com
*/
#pragma once

#ifdef __APPLE__
#   define _OSX
#elif defined(_WIN64)
#   ifndef _WINDOWS
#       define _WINDOWS
#   endif
#elif defined(__linux__)
#   define _LINUX
#endif

#include <GL/glew.h>
#ifdef _WINDOWS
#   include <GL/wglew.h>
#elif defined(_LINUX)
#   include <GL/xglew.h>
#endif
#include <GLFW/glfw3.h> 


#ifdef _WINDOWS
    // Link against OpenGL
#   pragma comment(lib, "opengl32")
#   pragma comment(lib, "glew_x64")
#   pragma comment(lib, "glfw_x64")
#endif

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cassert>
#include <vector>


void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    if ((type == GL_DEBUG_TYPE_ERROR) || (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)) {
        fprintf(stderr, "GL Debug: %s\n", message);
    }
}


#define PI (3.1415927f)

class Vector3 {
public:
    float x, y, z;
    /** initializes to zero */
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    explicit Vector3(const class Vector4&);
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    Vector3 cross(const Vector3& other) const {
        return Vector3(y * other.z - z * other.y,
                       z * other.x - x * other.z,
                       x * other.y - y * other.x);
    }
    float& operator[](int i) {
        return (&x)[i];
    }
    float operator[](int i) const {
        return (&x)[i];
    }
};


class Vector4 {
public:
    float x, y, z, w;
    /** initializes to zero */
    Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4(const Vector3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
    float dot(const Vector4& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }
    float& operator[](int i) {
        return (&x)[i];
    }
    float operator[](int i) const {
        return (&x)[i];
    }
};


Vector3::Vector3(const Vector4& v) : x(v.x), y(v.y), z(v.z) {}


class Matrix4x4 {
public:
    /** row-major */
    float data[16];

    /** row-major */
    Matrix4x4(float a, float b, float c, float d,
              float e, float f, float g, float h,
              float i, float j, float k, float l,
              float m, float n, float o, float p) {
        data[0]  = a; data[1]  = b; data[2]  = c; data[3]  = d;
        data[4]  = e; data[5]  = f; data[6]  = g; data[7]  = h;
        data[8]  = i; data[9]  = j; data[10] = k; data[11] = l;
        data[12] = m; data[13] = n; data[14] = o; data[15] = p;
    }

    /** initializes to the identity matrix */
    Matrix4x4() {
        memset(data, 0, sizeof(float) * 16);
        data[0] = data[5] = data[10] = data[15] = 1.0f;
    }

    Matrix4x4(const Matrix4x4& M) {
        memcpy(data, M.data, sizeof(float) * 16);
    }

    Matrix4x4& operator=(const Matrix4x4& M) {
        memcpy(data, M.data, sizeof(float) * 16);
        return *this;
    }

    static Matrix4x4 zero() {
        return Matrix4x4(0.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 0.0f);
    }

    static Matrix4x4 roll(float radians) {
        const float c = cos(radians);
        const float s = sin(radians);
        return Matrix4x4(   c,  -s,  0.0f, 0.0f, 
                            s,   c,  0.0f, 0.0f,
                         0.0f, 0.0f, 1.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);
    }

    static Matrix4x4 yaw(float radians) {
        const float c = cos(radians);
        const float s = sin(radians);
        return Matrix4x4(   c, 0.0f,    s, 0.0f, 
                         0.0f, 1.0f, 0.0f, 0.0f,
                            s, 0.0f,    c, 0.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);
    }

    static Matrix4x4 pitch(float radians) {
        const float c = cos(radians);
        const float s = sin(radians);
        return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 
                         0.0f, 1.0f, 0.0f, 0.0f,
                         0.0f,    c,  -s, 0.0f,
                         0.0f,    s,   c, 1.0f);
    }

    static Matrix4x4 scale(float x, float y, float z) {
        return Matrix4x4(   x, 0.0f, 0.0f, 0.0f, 
                         0.0f,    y, 0.0f, 0.0f,
                         0.0f, 0.0f,    z, 0.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);
    }

    static Matrix4x4 translate(float x, float y, float z) {
        return Matrix4x4(1.0f, 0.0f, 0.0f,    x, 
                         0.0f, 1.0f, 0.0f,    y,
                         0.0f, 0.0f, 1.0f,    z,
                         0.0f, 0.0f, 0.0f, 1.0f);
    }

    /** 
        Maps the view frustum to the cube [-1, +1]^3 in the OpenGL style.

        \param verticalRadians Vertical field of view from top to bottom
        \param nearZ Negative number
        \param farZ Negative number less than (higher magnitude than) nearZ. May be negative infinity 
    */
    static Matrix4x4 perspective(float pixelWidth, float pixelHeight, float nearZ, float farZ, float verticalRadians, float subpixelShiftX = 0.0f, float subpixelShiftY = 0.0f) {
        const float k = 1.0f / tan(verticalRadians / 2.0f);

        const float c = (farZ == -INFINITY) ? -1.0f : (nearZ + farZ) / (nearZ - farZ);
        const float d = (farZ == -INFINITY) ?  1.0f : farZ / (nearZ - farZ);

        Matrix4x4 P(k * pixelHeight / pixelWidth, 0.0f, subpixelShiftX * k / (nearZ * pixelWidth), 0.0f,
                    0.0f, k, subpixelShiftY * k / (nearZ * pixelHeight), 0.0f,
                    0.0f, 0.0f, c, -2.0f * nearZ * d,
                    0.0f, 0.0f, -1.0f, 0.0f);

        return P;        
    }

    /** 
        Maps the view frustum to the cube [-1, +1]^3 in the OpenGL
        style by orthographic projection in which (0, 0) will become
        the top-left corner of the screen after the viewport is
        applied and (pixelWidth - 1, pixelHeight - 1) will be the
        lower-right corner.
        
        \param nearZ Negative number
        \param farZ Negative number less than (higher magnitude than) nearZ. Must be finite
    */
    static Matrix4x4 ortho(float pixelWidth, float pixelHeight, float nearZ, float farZ) {
        return Matrix4x4(2.0f / pixelWidth, 0.0f, 0.0f, -1.0f,
                         0.0f, -2.0f / pixelHeight, 0.0f, 1.0f,
                         0.0f, 0.0f, -2.0f / (nearZ - farZ), (farZ + nearZ) / (nearZ - farZ),
                         0.0f, 0.0f, 0.0f, 1.0f);
    }

    Matrix4x4 transpose() const {
        return Matrix4x4(data[ 0], data[ 4], data[ 8], data[12],
                         data[ 1], data[ 5], data[ 9], data[13],
                         data[ 2], data[ 6], data[10], data[14],
                         data[ 3], data[ 7], data[11], data[15]);
    }

    /** Computes the inverse by Cramer's rule (based on MESA implementation) */
    Matrix4x4 inverse() const {
        Matrix4x4 result;
        const float* m = data;
        float* inv = result.data;

        inv[0] = m[5] * m[10] * m[15] -
            m[5] * m[11] * m[14] -
            m[9] * m[6] * m[15] +
            m[9] * m[7] * m[14] +
            m[13] * m[6] * m[11] -
            m[13] * m[7] * m[10];

        inv[4] = -m[4] * m[10] * m[15] +
            m[4] * m[11] * m[14] +
            m[8] * m[6] * m[15] -
            m[8] * m[7] * m[14] -
            m[12] * m[6] * m[11] +
            m[12] * m[7] * m[10];

        inv[8] = m[4] * m[9] * m[15] -
            m[4] * m[11] * m[13] -
            m[8] * m[5] * m[15] +
            m[8] * m[7] * m[13] +
            m[12] * m[5] * m[11] -
            m[12] * m[7] * m[9];

        inv[12] = -m[4] * m[9] * m[14] +
            m[4] * m[10] * m[13] +
            m[8] * m[5] * m[14] -
            m[8] * m[6] * m[13] -
            m[12] * m[5] * m[10] +
            m[12] * m[6] * m[9];

        inv[1] = -m[1] * m[10] * m[15] +
            m[1] * m[11] * m[14] +
            m[9] * m[2] * m[15] -
            m[9] * m[3] * m[14] -
            m[13] * m[2] * m[11] +
            m[13] * m[3] * m[10];

        inv[5] = m[0] * m[10] * m[15] -
            m[0] * m[11] * m[14] -
            m[8] * m[2] * m[15] +
            m[8] * m[3] * m[14] +
            m[12] * m[2] * m[11] -
            m[12] * m[3] * m[10];

        inv[9] = -m[0] * m[9] * m[15] +
            m[0] * m[11] * m[13] +
            m[8] * m[1] * m[15] -
            m[8] * m[3] * m[13] -
            m[12] * m[1] * m[11] +
            m[12] * m[3] * m[9];

        inv[13] = m[0] * m[9] * m[14] -
            m[0] * m[10] * m[13] -
            m[8] * m[1] * m[14] +
            m[8] * m[2] * m[13] +
            m[12] * m[1] * m[10] -
            m[12] * m[2] * m[9];

        inv[2] = m[1] * m[6] * m[15] -
            m[1] * m[7] * m[14] -
            m[5] * m[2] * m[15] +
            m[5] * m[3] * m[14] +
            m[13] * m[2] * m[7] -
            m[13] * m[3] * m[6];

        inv[6] = -m[0] * m[6] * m[15] +
            m[0] * m[7] * m[14] +
            m[4] * m[2] * m[15] -
            m[4] * m[3] * m[14] -
            m[12] * m[2] * m[7] +
            m[12] * m[3] * m[6];

        inv[10] = m[0] * m[5] * m[15] -
            m[0] * m[7] * m[13] -
            m[4] * m[1] * m[15] +
            m[4] * m[3] * m[13] +
            m[12] * m[1] * m[7] -
            m[12] * m[3] * m[5];

        inv[14] = -m[0] * m[5] * m[14] +
            m[0] * m[6] * m[13] +
            m[4] * m[1] * m[14] -
            m[4] * m[2] * m[13] -
            m[12] * m[1] * m[6] +
            m[12] * m[2] * m[5];

        inv[3] = -m[1] * m[6] * m[11] +
            m[1] * m[7] * m[10] +
            m[5] * m[2] * m[11] -
            m[5] * m[3] * m[10] -
            m[9] * m[2] * m[7] +
            m[9] * m[3] * m[6];

        inv[7] = m[0] * m[6] * m[11] -
            m[0] * m[7] * m[10] -
            m[4] * m[2] * m[11] +
            m[4] * m[3] * m[10] +
            m[8] * m[2] * m[7] -
            m[8] * m[3] * m[6];

        inv[11] = -m[0] * m[5] * m[11] +
            m[0] * m[7] * m[9] +
            m[4] * m[1] * m[11] -
            m[4] * m[3] * m[9] -
            m[8] * m[1] * m[7] +
            m[8] * m[3] * m[5];

        inv[15] = m[0] * m[5] * m[10] -
            m[0] * m[6] * m[9] -
            m[4] * m[1] * m[10] +
            m[4] * m[2] * m[9] +
            m[8] * m[1] * m[6] -
            m[8] * m[2] * m[5];

        float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
        return result / det;
    }

    float& operator()(int r, int c) {
        return data[r * 4 + c];         
    }

    float operator()(int r, int c) const {
        return data[r * 4 + c];         
    }

    Vector4 row(int r) const {
        const int i = r * 4;
        return Vector4(data[i], data[i + 1], data[i + 2], data[i + 3]);
    }

    Vector4 col(int c) const {
        return Vector4(data[c], data[c + 4], data[c + 8], data[c + 12]);
    }

    Matrix4x4 operator*(const Matrix4x4& B) const {
        Matrix4x4 D;
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                D(r, c) = row(r).dot(B.col(c));
            }
        }
        return D;
    }

    Matrix4x4 operator*(const float s) const {
        Matrix4x4 D;
        for (int i = 0; i < 16; ++i) {
            D.data[i] = data[i] * s;
        }
        return D;
    }

    Matrix4x4 operator/(const float s) const {
        Matrix4x4 D;
        for (int i = 0; i < 16; ++i) {
            D.data[i] = data[i] / s;
        }
        return D;
    }

    Vector4 operator*(const Vector4& v) const {
        Vector4 d;
        for (int r = 0; r < 4; ++r) {
            d[r] = row(r).dot(v);
        }
        return d;
    }
};


std::ostream& operator<<(std::ostream& os, const Vector3& v) {
    return os << "Vector3(" << std::setprecision(2) << v.x << ", " << v.y << ")";
}


std::ostream& operator<<(std::ostream& os, const Vector4& v) {
    return os << "Vector4(" << v.x << ", " << v.y << ", " << v.z << ")";
}


std::ostream& operator<<(std::ostream& os, const Matrix4x4& M) {
    os << "\nMatrix4x4(";
    
    for (int r = 0, i = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c, ++i) {
            os << M.data[i];
            if (c < 3) { os << ", "; }
        }
        if (r < 3) { os << ",\n          "; }
    }

    return os << ")\n";
}

GLFWwindow* initOpenGL(int width, int height, const std::string& title) {
    if (! glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW\n");
        ::exit(1);
    } 

    // Without these, shaders actually won't initialize properly
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#   ifdef _DEBUG
       glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#   endif

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (! window) {
        fprintf(stderr, "ERROR: could not open window with GLFW\n");
        glfwTerminate();
        ::exit(2);
    }
    glfwMakeContextCurrent(window);

    // Start GLEW extension handler, with improved support for new features
    glewExperimental = GL_TRUE;
    glewInit();

#   ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glEnable(GL_DEBUG_OUTPUT);
#       ifndef _OSX
            // Causes a segmentation fault on OS X
            glDebugMessageCallback(debugCallback, nullptr);
#       endif
#   endif

    printf("Renderer:       %s\n", glGetString(GL_RENDERER));
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    // Bind a single global vertex array (done this way since OpenGL 3)
    {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    return window;
}


std::string loadTextFile(const std::string& filename) {
    std::stringstream buffer;
    buffer << std::ifstream(filename.c_str()).rdbuf();
    return buffer.str();
}


GLuint compileShaderStage(GLenum stage, const std::string& source) {
    GLuint shader = glCreateShader(stage);
    const char* srcArray[] = { source.c_str() };

    glShaderSource(shader, 1, srcArray, NULL);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE) {
        GLint logSize = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

        std::vector<GLchar> errorLog(logSize);
        glGetShaderInfoLog(shader, logSize, &logSize, &errorLog[0]);

        fprintf(stderr, "Error while compiling\n %s\n\nError: %s\n", source.c_str(), &errorLog[0]);
        assert(false);

        glDeleteShader(shader);
        shader = GL_NONE;
    }

    return shader;
}


GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& pixelShaderSource) {
    GLuint shader = glCreateProgram();

    glAttachShader(shader, compileShaderStage(GL_VERTEX_SHADER, vertexShaderSource));
    glAttachShader(shader, compileShaderStage(GL_FRAGMENT_SHADER, pixelShaderSource));
    glLinkProgram(shader);

    return shader;
}


GLuint loadShaderProgram(const std::string& vertexFilename, const std::string& pixelFilename) {
    const std::string& vertexShaderSource = loadTextFile(vertexFilename);
    const std::string& pixelShaderSource  = loadTextFile(pixelFilename);
    return createShaderProgram(vertexShaderSource, pixelShaderSource);
}


/** Submits a full-screen quad at the far plane and runs a procedural sky shader on it */
void drawSky(int windowWidth, int windowHeight, float nearPlaneZ, float farPlaneZ, float verticalFieldOfView, const Matrix4x4& cameraToWorldMatrix) {
#   define VERTEX_SHADER(s) "#version 410\n" #s
#   define PIXEL_SHADER(s) VERTEX_SHADER(s)

    static const GLuint skyShader =
        createShaderProgram(VERTEX_SHADER
    (void main() {
        gl_Position = vec4(gl_VertexID >> 1, gl_VertexID & 1, 0.0, 0.5) * 4.0 - 1.0;
    }),

    PIXEL_SHADER
    (out vec3 pixelColor;

    uniform vec2  resolution;
    uniform float tanVerticalFieldOfView;
    uniform mat4  cameraToWorldMatrix;

    float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

    float noise(vec2 x) {
        vec2 i = floor(x);
        float a = hash(i);
        float b = hash(i + vec2(1.0, 0.0));
        float c = hash(i + vec2(0.0, 1.0));
        float d = hash(i + vec2(1.0, 1.0));

        vec2 f = fract(x);
        vec2 u = f * f * (3.0 - 2.0 * f);
        return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
    }

    float fbm(vec2 p) {
        const mat2 m2 = mat2(0.8, -0.6, 0.6, 0.8);
        float f = 0.5000 * noise(p); p = m2 * p * 2.02;
        f += 0.2500 * noise(p); p = m2 * p * 2.03;
        f += 0.1250 * noise(p); p = m2 * p * 2.01;
        f += 0.0625 * noise(p);
        return f / 0.9375;
    }

    vec3 render(in vec3 sun, in vec3 ro, in vec3 rd, in float resolution) {
        vec3 col;
        if (rd.y < 0.0) {
            float t = -ro.y / rd.y;
            vec2 P = ro.xz + t * rd.xz;
            vec2 Q = floor(P);
            P = mod(P, 1.0);

            const float gridLineWidth = 0.1;
            float res = clamp(2048.0 / resolution, 1.0, 3.0);
            P = 1.0 - abs(P - 0.5) * 2.0;
            float d = clamp(min(P.x, P.y) / (gridLineWidth * clamp(t + res * 2.0, 1.0, 2.0)) + 0.5, 0.0, 1.0);
            float shade = mix(hash(100.0 + Q * 0.1) * 0.4, 0.3, min(t * t * 0.001, 1.0)) + 0.6;
            col = vec3(pow(d, clamp(150.0 / (pow(max(t - 2.0, 0.1), res) + 1.0), 0.1, 15.0))) * shade + 0.1;
        }
        else {
            col = vec3(0.3, 0.55, 0.8) * (1.0 - 0.8 * rd.y) * 0.9;
            float sundot = clamp(dot(rd, sun) / length(sun), 0.0, 1.0);
            col += 0.25 * vec3(1.0, 0.7, 0.4) * pow(sundot, 8.0);
            col += 0.75 * vec3(1.0, 0.8, 0.5) * pow(sundot, 64.0);
            col = mix(col, vec3(1.0, 0.95, 1.0), 0.5 * smoothstep(0.5, 0.8, fbm((ro.xz + rd.xz * (250000.0 - ro.y) / rd.y) * 0.000008)));
        }
        return mix(col, vec3(0.7, 0.75, 0.8), pow(1.0 - max(abs(rd.y), 0.0), 8.0));
    }

    void main() {
        vec3 ro = cameraToWorldMatrix[3].xyz;
        vec3 rd = normalize(mat3(cameraToWorldMatrix) * 
            vec3(gl_FragCoord.xy - resolution.xy / 2.0,
                resolution.y / -tanVerticalFieldOfView));

        pixelColor = 
            render(vec3(1, 0.5, 0.0), ro, rd, resolution.x);
            //vec3(gl_FragCoord.xy / 1000.0, 1.0);

    }));

    static const GLint resolutionUniform             = glGetUniformLocation(skyShader, "resolution");
    static const GLint tanVerticalFieldOfViewUniform = glGetUniformLocation(skyShader, "tanVerticalFieldOfView");
    static const GLint cameraToWorldMatrixUniform    = glGetUniformLocation(skyShader, "cameraToWorldMatrix");

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glUseProgram(skyShader);
    glUniform2f(resolutionUniform, float(windowWidth), float(windowHeight));
    glUniform1f(tanVerticalFieldOfViewUniform, tan(verticalFieldOfView));
    glUniformMatrix4fv(cameraToWorldMatrixUniform, 1, GL_TRUE, cameraToWorldMatrix.data);
    glDrawArrays(GL_TRIANGLES, 0, 3);

#   undef PIXEL_SHADER
#   undef VERTEX_SHADER
}


namespace Cube {
    const const float position[] = { -.5f, .5f, -.5f, -.5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, -.5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, -.5f, .5f };
    const const float normal[]   = { 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f };
    const const float tangent[]  = { 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, -1.f, 1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f };
    const const float texCoord[] = { 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f };
    const const int   index[]    = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23 };
};
