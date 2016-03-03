/**
  \file minGL4/minGL4.h
  \author Morgan McGuire, http://graphics.cs.williams.edu

  Minimal headers emulating a basic set of 3D graphics classes.

  All 3D math from http://graphicscodex.com
*/
#pragma once

#ifdef __APPLE__
#   define G3D_OSX
#elif defined(_WIN64)
#   define G3D_WINDOWS
#elif defined(__linux__)
#   define G3D_LINUX
#endif

#include <GL/glew.h>
#ifdef G3D_WINDOWS
#   include <GL/wglew.h>
#elif defined(G3D_LINUX)
#   include <GL/xglew.h>
#endif
#include <GLFW/glfw3.h> 

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

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

    Vector4 operator*(const Vector4& v) const {
        Vector4 d;
        for (int r = 0; r < 4; ++r) {
            d[r] = row(r).dot(v);
        }
        return d;
    }
};



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

    printf("Renderer:       %s\n", glGetString(GL_RENDERER));
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    return window;
}


std::string loadTextFile(const std::string& filename) {
    std::stringstream buffer;
    buffer << std::ifstream(filename.c_str()).rdbuf();
    return buffer.str();
}


GLuint compileShader(const std::string& vertexShaderSource, const std::string& pixelShaderSource) {
    GLuint shader = glCreateProgram();

    const char* vSrcArray[] = {vertexShaderSource.c_str()};
    const char* pSrcArray[] = {pixelShaderSource.c_str()};
    
    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource(vs, 1, vSrcArray, NULL);
    glCompileShader(vs);
    
    GLuint ps = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(ps, 1, pSrcArray, NULL);
    glCompileShader(ps);
    
    glAttachShader(shader, ps);
    glAttachShader(shader, vs);
    glLinkProgram(shader);

    return shader;
}


GLuint loadShader(const std::string& vertexFilename, const std::string& pixelFilename) {
    const std::string& vertexShaderSource = loadTextFile(vertexFilename);
    const std::string& pixelShaderSource  = loadTextFile(pixelFilename);
    return compileShader(vertexShaderSource, pixelShaderSource);
}


/** Submits OpenGL geometry to attribute positionAttribute for a 2D rectangle */
void drawRect(GLint positionAttribute, int width, int height, float z = -1.0f) {
    static GLuint positionBuffer = GL_NONE;

    if (positionBuffer == GL_NONE) {
        // Only allocate during the first call
        glGenBuffers(1, &positionBuffer);
    }
    
    const Vector3 cpuPosition[] = {
        Vector3( 0.0f,   0.0f, z),
        Vector3(width, height, z),
        Vector3( 0.0f, height, z),
        Vector3(width,   0.0f, z)
    };

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vector3), cpuPosition, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(positionAttribute, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


#define SHADER_SOURCE(s) "#version 410\n" #s
void drawSky(float windowWidth, float windowHeight, float nearPlaneZ, float farPlaneZ, float verticalFieldOfView) {
    static const GLuint skyShader = 
        compileShader(SHADER_SOURCE
                      (layout(location = 0) uniform mat4x4 MVP;
                       layout(location = 0) in vec3 position;

                       void main() {
                           gl_Position = MVP * vec4(position, 1.0);
                       }), 

                      SHADER_SOURCE
                      (out vec4 pixelColor;
                       
                       void main() { 
                           pixelColor = vec4(0.0, 1.0, 1.0, 1.0);
                       }));

    // Orthographic matrix
    glUniformMatrix4fv(0, 1, GL_TRUE, Matrix4x4::ortho(windowWidth, windowHeight, nearPlaneZ, farPlaneZ).data);

    glUseProgram(skyShader);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_CLAMP);

    const GLint positionAttribute = 0;
    drawRect(positionAttribute, windowWidth, windowHeight, farPlaneZ + 0.1f);
}