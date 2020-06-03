#include <stdint.h>
#include <stdbool.h>

#include "stdio.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <math.h>
#include "linmath.h"

#include "host_data.h"
#include "game.h"

typedef struct Vertex
{
    vec2 pos;
    vec3 col;
} Vertex;

static const Vertex vertices[3] =
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
};

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";


void game_init(HostData* hd) {
  printf("game_init\n");


    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    hd->program = glCreateProgram();
    glAttachShader(hd->program, vertex_shader);
    glAttachShader(hd->program, fragment_shader);
    glLinkProgram(hd->program);

    hd-> mvp_location = glGetUniformLocation(hd->program, "MVP");
    const GLint vpos_location = glGetAttribLocation(hd->program, "vPos");
    const GLint vcol_location = glGetAttribLocation(hd->program, "vCol");


    glGenVertexArrays(1, &hd->vertex_array);
    glBindVertexArray(hd->vertex_array);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) sizeof(vec2));

}

void game_shutdown(HostData* hd) {
}

void game_tick(HostData* hd) {
  // hd->quit_game = true;

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  float ratio = 1.3f;

  mat4x4 m, p, mvp;
  mat4x4_identity(m);
  mat4x4_rotate_Z(m, m, 0.0f);  // get time
  mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
  mat4x4_mul(mvp, p, m);


  glUseProgram(hd->program);
  glUniformMatrix4fv(hd->mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
  glBindVertexArray(hd->vertex_array);
  glDrawArrays(GL_TRIANGLES, 0, 3);

}
