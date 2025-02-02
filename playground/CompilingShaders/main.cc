#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>

class Shader {
    std::string src; 
protected:
    const char *getsrc() {
        return src.data();
    }
    GLuint shader_id = 0;
protected:
    virtual const char *getClassName() = 0;
public:
    virtual void compile() = 0;
    void setSource(const char *s) {
        std::ifstream sourceFile(s);
        if (!sourceFile.is_open())
            return;
        char buffer[8192];
        while (sourceFile.read(buffer, 8192)) {
            src.append(buffer, 8192);
        }
        if (!sourceFile.eof()) {
            src.clear();
            return;
        }
        src.append(buffer, sourceFile.gcount());
    }
    void sendError(GLuint shader_id) {
        char buffer[1024];
        glGetShaderInfoLog(shader_id, 1024, NULL, buffer);
        std::cerr << "ERROR::" << getClassName() << " - " << buffer;
    }
    GLint getCompilationStatus(GLuint shader_id) {
        int status;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
        return status;
    }
};


class VertexShader : public Shader {
    virtual const char *getClassName() override {
        return "VertexShader";
    }
public:
    virtual void compile() override {
        if (shader_id)
            glDeleteShader(shader_id);
        shader_id = glCreateShader(GL_VERTEX_SHADER);
        const char *src = getsrc();
        glShaderSource(shader_id, 1, &src, NULL);
        glCompileShader(shader_id);
        if (!getCompilationStatus(shader_id)) {
            sendError(shader_id);
        }
    }
};

void resizeEvent(GLFWwindow *win, int width, int height) {
    glViewport(0, 0, width, height);
}


int main() {
    if (glfwInit() != GLFW_TRUE) {
        glfwTerminate();
        std::cerr << "Failed to initialize GLFW";
        return EXIT_FAILURE;
    }
    // setting OpenGL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow *win = glfwCreateWindow(600, 600, "This is a hello window!", NULL, NULL);
    glViewport(0, 0, 600, 600);
    glfwSetFramebufferSizeCallback(win, resizeEvent);
    // setting 'context' for OpenGL, i.e. where to draw on current thread
    glfwMakeContextCurrent(win);
    // all it does is fetches us the implemented functions of OpenGL
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        std::cerr << "Failed to initialize GLEW\n";
        return EXIT_FAILURE;
    }

    VertexShader vs;
    vs.setSource("./vertex.glsl");
    vs.compile();

    
    while (!glfwWindowShouldClose(win)) {
        // polls different kinds of events, for example, when we close an application, it fetches that event
        // or it fetches events like movement of the window.
        // Without it you can neither move the window or close the window
        glfwPollEvents();
        // have you drawn the image, it is stored in the buffer. You can now swap this buffer with main buffer
        // so the image appears
        glfwSwapBuffers(win);
    }
    glfwTerminate();
    
    std::cout << "Window should close now!\n";

    return EXIT_SUCCESS;

}
