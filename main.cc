#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include <Magick++.h>

using namespace std;

#define GLSL330(src) "#version 330 core\n" #src

void gl_GenTexture(GLuint &id, int width, int height,void *ptr = NULL)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id); 
  
    // GL_UNSIGNED_BYTE specifier is apparently ignored..
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, ptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glFinish();
}

// glfw3: http://www.glfw.org/

// helper to check and display for shader compiler errors
bool check_shader_compile_status(GLuint obj) {
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
        vector<char> log(length);
        glGetShaderInfoLog(obj, length, &length, &log[0]);
        cerr << &log[0];
        return false;
    }
    return true;
}

// helper to check and display for shader linker error
bool check_program_link_status(GLuint obj) {
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
        vector<char> log(length);
        glGetProgramInfoLog(obj, length, &length, &log[0]);
        cerr << &log[0];
        return false;
    }
    return true;
}

static void error_callback(int error, const char *msg) {
    cerr << "GLWT error " << error << ": " << msg << endl;
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) {
        cout << "usage: " << argv[0] << " image " << endl;
        exit(1);
    }

    Magick::InitializeMagick(NULL);

    Magick::Image image(argv[1]);
    int width = image.baseColumns();
    int height = image.baseRows();

    vector<unsigned char> imageData(width*height*4);
    image.write(0,0,width,height,"BGRA",Magick::CharPixel,(void *)&(imageData[0]));
    cout << "got here" << endl;

    glfwSetErrorCallback(error_callback);

    if(glfwInit() == GL_FALSE) {
        cerr << "failed to init GLFW" << endl;
        return 1;
    }

    // GLenum err = glewInit();
    // if (GLEW_OK != err)
    // {
    //     // Problem: glewInit failed, something is seriously wrong.
    //     cout << "Error: " << glewGetErrorString(err) << endl;
    //     return 1;
    // }

    // select opengl version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create a window
    GLFWwindow *window;
    if((window = glfwCreateWindow(width, height, "glfw3", NULL, NULL)) == 0) {
        cerr << "failed to open window" << endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    // safe to make gl calls

    // get version info
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "shader lang: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;


    GLuint texID;
    gl_GenTexture(texID,width,height,(void *)&(imageData[0]));


    // shader source code
    string vertex_source = GLSL330(
        layout(location = 0) in vec4 vposition;
        out vec2 VertTexCoord;
        void main() {
           VertTexCoord = vposition.xy;
           gl_Position = vposition;
        }
    );

    // string fragment_source = GLSL330(
    //     in vec2 VertTexcoord;
        
    //     uniform sampler2D Diffuse;
    //     void main() {
    //        gl_FragColor = texture2D(Diffuse, VertTexcoord);
    //     }
    // );

    string fragment_source = GLSL330(
        in vec2 VertTexCoord;
        layout(location = 0) out vec4 outColor;
        uniform sampler2D Diffuse;
        void main() {
            outColor = texture(Diffuse, VertTexCoord); 
            // outColor = vec4(1.0,0,0,1.0);
        }
    );    


    // program and shader handles
    GLuint shader_program, vertex_shader, fragment_shader, UniformDiffuse;



    // we need these to properly pass the strings
    const char *source;
    int length;

    // create and compiler vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    source = vertex_source.c_str();
    length = vertex_source.size();
    glShaderSource(vertex_shader, 1, &source, &length);
    glCompileShader(vertex_shader);


    GLint success = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);    
    if(success == GL_FALSE) {
        cout << "vertex shader busted messelaneous" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // create and compiler fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragment_source.c_str();
    length = fragment_source.size();
    glShaderSource(fragment_shader, 1, &source, &length);
    glCompileShader(fragment_shader);
    
    success = 0;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);    
    if(success == GL_FALSE) {
        cout << "fragment shader busted messelaneous" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // create program
    shader_program = glCreateProgram();

    // attach shaders
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glBindAttribLocation(shader_program, 0, "texCoord");
    glBindFragDataLocation(shader_program, 0, "color");

    // link the program and check for errors
    glLinkProgram(shader_program);

    //Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(shader_program, GL_LINK_STATUS, (int *)&isLinked);
    if(isLinked == GL_FALSE) { 
        cout << "shader linking into program busted messelaneous" << endl; 
        return 1;
    }
    UniformDiffuse = glGetUniformLocation(shader_program, "Diffuse");

    // vao and vbo handle
    GLuint vao, vbo;

    // generate and bind the vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // generate and bind the buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // data for a fullscreen quad
    GLfloat vertexData[] = {
    //  X     Y     Z   
      -1.0f,-1.0f, 0.0f,
      -1.0f, 1.0f, 0.0f,
       1.0f, 1.0f, 0.0f,
       1.0f, 1.0f, 0.0f,
       1.0f,-1.0f, 0.0f,
      -1.0f,-1.0f, 0.0f
    }; // 2 triangles to cover a rectangle

    // fill with data
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6*3, vertexData, GL_STATIC_DRAW);

    // set up generic attrib pointers, 2 triangles
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // clear first
        glClear(GL_COLOR_BUFFER_BIT);

        // use the shader program
        glUseProgram(shader_program);

        // bind the input texture
        glEnable(GL_TEXTURE_2D); //Enable texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);
        // set uniform sampler for texture input
        glUniform1i(UniformDiffuse, 0);

        // bind the vao
        glBindVertexArray(vao);

        // draw
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // check for errors
        GLenum error = glGetError();
        if(error != GL_NO_ERROR) {
            static int first = 1;
            if (first && error == 1280) {
                cout << "GL error!" << error << " string " << gluErrorString(error) << endl;
                first = 0;
            }
            
            if (error != 1280) {
                cout << "GL error!" << error << " string " << gluErrorString(error) << endl;
                break; // skip invalid enumerant
            }
        }

        // finally swap buffers
        glfwSwapBuffers(window);
    }

    // delete the created objects

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glDetachShader(shader_program, vertex_shader);
    glDetachShader(shader_program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(shader_program);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


