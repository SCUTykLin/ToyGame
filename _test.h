//
// Created by 15813 on 2026/7/18.
//

/*
 * 该头文件只是为了测试各种库是否正确运行
 */

#ifndef TOYPROJECTS_OPENGL_TEST_H
#define TOYPROJECTS_OPENGL_TEST_H
// 注意：glad.h 必须放在所有 OpenGL 相关头文件之前
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // 平移/旋转/缩放变换
#include <glm/gtc/type_ptr.hpp>         // 矩阵转原生指针（传给Shader用）
#include <glm/gtc/constants.hpp>        // π 等数学常量

// 窗口大小变化时的回调：自动调整 OpenGL 视口
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 顶点着色器源码
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

// 片段着色器源码
const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 ourColor;
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
    )";


int test_glm() {
    // ========== 1. 向量与几何运算 ==========
    glm::vec3 position(100.0f, 200.0f, 0.0f);
    glm::vec3 direction(1.0f, 0.0f, 0.0f);
    float moveSpeed = 50.0f;
    float deltaTime = 0.016f; // 60帧帧间隔

    // 游戏物体位移计算（帧率无关）
    glm::vec3 newPos = position + direction * moveSpeed * deltaTime;
    std::cout << "移动后位置: (" << newPos.x << ", " << newPos.y << ")\n";

    // 点积、叉积、归一化（光照、碰撞检测常用）
    glm::vec3 v1(1.0f, 0.0f, 0.0f);
    glm::vec3 v2(0.0f, 1.0f, 0.0f);
    float dotResult = glm::dot(v1, v2);
    glm::vec3 crossResult = glm::cross(v1, v2);
    glm::vec3 normalized = glm::normalize(glm::vec3(3.0f, 4.0f, 0.0f));

    std::cout << "点积: " << dotResult << "\n";
    std::cout << "叉积: (" << crossResult.x << ", " << crossResult.y << ", " << crossResult.z << ")\n";
    std::cout << "归一化后模长: " << glm::length(normalized) << "\n\n";

    // ========== 2. 模型变换矩阵 ==========
    glm::mat4 model(1.0f);
    // 平移
    model = glm::translate(model, glm::vec3(200.0f, 300.0f, 0.0f));
    // 旋转：绕Z轴转45度，角度必须转弧度
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // 缩放
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));

    // 矩阵乘顶点：计算最终屏幕坐标
    glm::vec4 localVertex(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 worldVertex = model * localVertex;
    std::cout << "变换后顶点: (" << worldVertex.x << ", " << worldVertex.y << ")\n";

    // 转原生float数组，直接传给OpenGL glUniformMatrix4fv
    const float* matPtr = glm::value_ptr(model);
    std::cout << "矩阵首元素: " << matPtr[0] << "\n\n";

    // ========== 3. 投影矩阵（相机/视口） ==========
    // 2D正交投影（像素级坐标，左上原点）
    glm::mat4 ortho = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    // 3D透视投影
    glm::mat4 perspective = glm::perspective(
        glm::radians(45.0f),   // 视场角
        800.0f / 600.0f,       // 宽高比
        0.1f,                  // 近裁剪面
        100.0f                 // 远裁剪面
    );

    std::cout << "正交投影[0][0]: " << ortho[0][0] << "\n";
    std::cout << "透视投影[0][0]: " << perspective[0][0] << "\n\n";

    std::cout << "GLM 综合功能测试全部通过！\n";
    return 0;
}

int test_opengl(){
    // 1. 初始化 GLFW
    glfwInit();
    // 指定 OpenGL 3.3 核心模式
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Test", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // 注册窗口大小变化回调
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 3. 初始化 GLAD（加载所有 OpenGL 函数指针）
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 4. 编译顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // 5. 编译片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // 6. 链接着色器程序
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 用完删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 7. 定义顶点数据：位置 + 颜色（三个顶点构成三角形）
    float vertices[] = {
            // 位置            // 颜色
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // 左下角：红色
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // 右下角：绿色
            0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f   // 顶部：蓝色
    };

    // 8. 创建 VAO / VBO 并绑定顶点数据
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 设置顶点属性：位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 设置顶点属性：颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 9. 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 清屏（深灰色背景）
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 使用着色器程序，绑定顶点数组，绘制三角形
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // 交换缓冲，处理输入事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 10. 释放资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

#endif //TOYPROJECTS_OPENGL_TEST_H
