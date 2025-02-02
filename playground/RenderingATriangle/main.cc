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
    bool isCompiled = false;
protected:
    virtual const char *getClassName() = 0;
    GLint getCompilationStatus(GLuint shader_id) {
        int status;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
        return status;
    }
    void sendError() {
        char buffer[1024];
        glGetShaderInfoLog(shader_id, 1024, NULL, buffer);
        std::cerr << "ERROR::" << getClassName() << " - " << buffer;
    }
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
    friend class Program;
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
            sendError();
        }
        isCompiled = true;
    }
};

class FragmentShader : public Shader {
    const char *getClassName() override {
        return "FragmentShader";
    }
public:
    virtual void compile() override {
        if (shader_id)
            glDeleteShader(shader_id);
        shader_id = glCreateShader(GL_FRAGMENT_SHADER);
        const char *src = getsrc();
        glShaderSource(shader_id, 1, &src, NULL);
        glCompileShader(shader_id);
        if (!getCompilationStatus(shader_id)) {
            sendError();
        }
        isCompiled = true;
    }
};


class Program {
    GLuint program_id = 0;
    void sendError() {
        char buffer[1024];
        glGetProgramInfoLog(program_id, 1024, NULL, buffer);
        std::cerr << "ERROR::PROGRAM: " << " - " << buffer;
    }
    bool linkStatus() {
        int status = 0;
        glGetProgramiv(program_id, GL_LINK_STATUS, &status);
        return status;
    }
public:
    Program() {
        program_id = glCreateProgram();
    }
    ~Program() {
        glDeleteProgram(program_id);
    }
    void AttachShaders(std::initializer_list<Shader*> shaders) {
        auto i = shaders.begin();
        while (i != shaders.end()) {
            if (!(*i)->isCompiled)
                (*i)->compile();
            glAttachShader(program_id, (*i)->shader_id);
            ++i;
        }
        glLinkProgram(program_id);
        if (!linkStatus()) {
            sendError();
        }
    }
    void UseProgram() {
        glUseProgram(program_id);
    }
};




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
    // setting 'context' for OpenGL, i.e. where to draw on current thread
    glfwMakeContextCurrent(win);
    // all it does is fetches us the implemented functions of OpenGL
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        std::cerr << "Failed to initialize GLEW\n";
        return EXIT_FAILURE;
    }

    float triangle_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo); 
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_data), triangle_data, GL_STATIC_DRAW);
    
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    VertexShader vs;
    FragmentShader fs;
    vs.setSource("./vertex.glsl");
    fs.setSource("./frag.glsl");
    Program prog;
    prog.AttachShaders({&vs, &fs});
    prog.UseProgram();

    
    while (!glfwWindowShouldClose(win)) {
        glDrawArrays(GL_TRIANGLES, 0, 3);
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
