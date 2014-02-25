#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

// program and shader handles
GLuint shader_program, vertex_shader, fragment_shader;


bool check_shader_compile_status(GLuint obj) {
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
        vector<char> log(length);
        glGetShaderInfoLog(obj, length, &length, &log[0]);
        cout << &log[0];
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
        cout << &log[0];
        return false;
    }
    return true;
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shader_program);    
    glBegin(GL_TRIANGLES);
        glVertex3f(-0.5,-0.5,0.0);
        glVertex3f(0.5,0.0,0.0);
        glVertex3f(0.0,0.5,0.0);
    glEnd();
    glFlush();
}

// void key_pressed(unsigned char key, int x, int y, int win){
//     cout << "Pressed key " << key << " on coordinates (" << x << "," << y << ")\n";
//     if(key == 'q'){
//         cout << "Got q, so quitting\n";
//         glutDestroyWindow(win);
//         exit(0);
//     }
// }


int main(int argc, char **argv)
{
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // glutInitContextVersion(3, 3);
    // glutInitContextProfile(GLUT_CORE_PROFILE);
    // #define GL_SHADING_LANGUAGE_VERSION <hex of version>

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(320,320);
    int win = glutCreateWindow("3D Tech- GLUT Tutorial");

    cout << "shader lang: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    // key_pressed(key, x, y, win);

    // shader source code
    std::string vertex_source =
        // "#version 330\n"
        // "layout(location = 0) in vec4 vposition;\n"
        // "layout(location = 1) in vec4 vcolor;\n"
        // "out vec4 fcolor;\n"
        // "void main() {\n"
        // "   fcolor = vcolor;\n"
        // "   gl_Position = vposition;\n"
        // "}\n";

        "#version 120\n"
        "attribute vec4 position;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = position;\n"
        "}\n";        

    std::string fragment_source =
        // "#version 330\n"
        // "in vec4 fcolor;\n"
        // "layout(location = 0) out vec4 FragColor;\n"
        // "void main() {\n"
        // "   FragColor = fcolor;\n"
        // "}\n";

        "#version 120\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);\n"
        "}\n";        

    // we need these to properly pass the strings
    const char *source;
    int length;

    // create and compiler vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    source = vertex_source.c_str();
    length = vertex_source.size();
    glShaderSource(vertex_shader, 1, &source, &length);
    glCompileShader(vertex_shader);
    if(!check_shader_compile_status(vertex_shader)) {
        cout << "vertex shader failed to compile" << endl;
        return 1;
    }

    // create and compiler fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragment_source.c_str();
    length = fragment_source.size();
    glShaderSource(fragment_shader, 1, &source, &length);
    glCompileShader(fragment_shader);
    if(!check_shader_compile_status(fragment_shader)) {
        cout << "fragment shader failed to compile" << endl;        
        return 1;
    }

    // create program
    shader_program = glCreateProgram();

    // attach shaders
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    // link the program and check for errors
    glLinkProgram(shader_program);
    if(!check_program_link_status(shader_program)) {
        cout << "program linking failed" << endl;
        return 1;
    }

    glutDisplayFunc(renderScene);
    glutMainLoop();
    return 0;
}