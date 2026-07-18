//
// Created by 15813 on 2026/7/18.
//

#ifndef TOYPROJECTS_OPENGL_TEST_H
#define TOYPROJECTS_OPENGL_TEST_H
// 注意：glad.h 必须放在所有 OpenGL 相关头文件之前
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

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
