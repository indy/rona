#include <stdint.h>
#include <stdbool.h>

#include "stdio.h"

#include <GL/gl.h>

#include "gfx.h"
#include "host_data.h"
#include "game.h"

static GLuint CreateShaderType(Gfx *hd, GLenum type, const char *source) {
    GLuint shaderId = hd->glCreateShader(type);

    hd->glShaderSource(shaderId, 1, &source, NULL);
    hd->glCompileShader(shaderId);

    char info[1024 * 10];
    int i = 0;
    for (i = 0; i < 1024 * 10; i++) {
      info[i] = 0;
    }

    GLint compileResult;
    hd->glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileResult);
    if(!compileResult) {
        GLint infoLen;
        hd->glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLen);
        // fplAssert(infoLen <= fplArrayCount(info));
        hd->glGetShaderInfoLog(shaderId, infoLen, &infoLen, info);
        // fplConsoleFormatError("Failed compiling %s shader!\n", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
        // fplConsoleFormatError("%s\n", info);
    }

    return(shaderId);
}

static GLuint CreateShaderProgram(Gfx *hd, const char *name, const char *vertexSource, const char *fragmentSource) {
    GLuint programId = hd->glCreateProgram();

    GLuint vertexShader = CreateShaderType(hd, GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = CreateShaderType(hd, GL_FRAGMENT_SHADER, fragmentSource);

    hd->glAttachShader(programId, vertexShader);
    hd->glAttachShader(programId, fragmentShader);
    hd->glLinkProgram(programId);
    hd->glValidateProgram(programId);

    char info[1024 * 10];
    int i = 0;
    for (i = 0; i < 1024 * 10; i++) {
      info[i] = 0;
    }

    GLint linkResult;
    hd->glGetProgramiv(programId, GL_LINK_STATUS, &linkResult);
    if(!linkResult) {
        GLint infoLen;
        hd->glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLen);
        // fplAssert(infoLen <= fplArrayCount(info));
        hd->glGetProgramInfoLog(programId, infoLen, &infoLen, info);
        // fplConsoleFormatError("Failed linking '%s' shader!\n", name);
        // fplConsoleFormatError("%s\n", info);
    }

    hd->glDeleteShader(fragmentShader);
    hd->glDeleteShader(vertexShader);

    return(programId);
}

void game_init(HostData* hd) {
  Gfx *gl = &(hd->gl);


  printf("game_init\n");
    gl->glGenVertexArrays(1, &hd->vertexArrayID);
    gl->glBindVertexArray(hd->vertexArrayID);

    // const char *glslVersion = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    // fplConsoleFormatOut("OpenGL GLSL Version %s:\n", glslVersion);

    int profileMask;
    int contextFlags;
    gl->glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
    gl->glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    // fplConsoleFormatOut("OpenGL supported profiles:\n");
    // fplConsoleFormatOut("\tCore: %s\n", ((profileMask & GL_CONTEXT_CORE_PROFILE_BIT) ? "yes" : "no"));
    // fplConsoleFormatOut("\tForward: %s\n", ((contextFlags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) ? "yes" : "no"));

    // fplConsoleOut("Running modern opengl\n");

    const char vertexSource[] = {
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 inPosition;\n"
        "\n"
        "void main() {\n"
        "\tgl_Position = inPosition;\n"
        "}\n"
    };

    const char fragmentSource[] = {
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 outColor;\n"
        "\n"
        "void main() {\n"
        "\toutColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n"
    };

    GLuint shaderProgram = CreateShaderProgram(&(hd->gl), "Test", vertexSource, fragmentSource);

    float vertices[] = {
        0.0f, 0.5f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };
    GLuint buffer;
    gl->glGenBuffers(1, &buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);

    gl->glUseProgram(shaderProgram);

    gl->glBindBuffer(GL_ARRAY_BUFFER, buffer);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);

    gl->glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
}

void game_shutdown(HostData* hd) {
  Gfx *gl = &(hd->gl);

  gl->glDisableVertexAttribArray(0);
  gl->glBindBuffer(GL_ARRAY_BUFFER, 0);

  gl->glBindVertexArray(0);
  gl->glDeleteVertexArrays(1, &hd->vertexArrayID);
}

void game_tick(HostData* hd) {
  Gfx *gl = &(hd->gl);

  gl->glViewport(0, 0, hd->window_width, hd->window_height);
  gl->glClear(GL_COLOR_BUFFER_BIT);
  gl->glDrawArrays(GL_TRIANGLES, 0, 3);
}
