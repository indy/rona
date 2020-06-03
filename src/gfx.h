#ifndef GFX_H
#define GFX_H

#include <GL/gl.h>

#ifndef APIENTRY
#define APIENTRY
#endif
#define APIENTRYP APIENTRY *

#include <stddef.h>
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef char GLchar;

#ifndef GL_CONTEXT_PROFILE_MASK

#define GL_CONTEXT_PROFILE_MASK           0x9126
#define GL_CONTEXT_FLAGS                  0x821E

#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT  0x0001
#define GL_CONTEXT_FLAG_DEBUG_BIT               0x00000002
#define GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT       0x00000004
#define GL_CONTEXT_FLAG_NO_ERROR_BIT            0x00000008
#define GL_CONTEXT_CORE_PROFILE_BIT             0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT    0x00000002

#endif // GL_CONTEXT_PROFILE_MASK

#define GL_COMPILE_STATUS                 0x8B81
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_LINK_STATUS                    0x8B82
#define GL_STATIC_DRAW                    0x88E4


typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void (APIENTRYP PFNGLGETINTEGERVPROC)(GLenum pname, GLint * params);

typedef GLuint(APIENTRYP PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef GLuint(APIENTRYP PFNGLCREATEPROGRAMPROC) (void);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRYP PFNGLDELETESHADERPROC) (GLuint shader);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC) (GLuint program);

#define GL_ARRAY_BUFFER                   0x8892

typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint *arrays);


#ifdef __cplusplus
extern "C" {
#endif

  typedef struct  {
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    PFNGLCLEARPROC glClear;
    PFNGLVIEWPORTPROC glViewport;
    PFNGLDRAWARRAYSPROC glDrawArrays;
    PFNGLCLEARCOLORPROC glClearColor;
    PFNGLGETINTEGERVPROC glGetIntegerv;

    PFNGLCREATESHADERPROC glCreateShader;
    PFNGLSHADERSOURCEPROC glShaderSource;
    PFNGLCOMPILESHADERPROC glCompileShader;
    PFNGLGETSHADERIVPROC glGetShaderiv;
    PFNGLATTACHSHADERPROC glAttachShader;
    PFNGLCREATEPROGRAMPROC glCreateProgram;
    PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
    PFNGLLINKPROGRAMPROC glLinkProgram;
    PFNGLVALIDATEPROGRAMPROC glValidateProgram;
    PFNGLGETPROGRAMIVPROC glGetProgramiv;
    PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
    PFNGLDELETESHADERPROC glDeleteShader;
    PFNGLUSEPROGRAMPROC glUseProgram;
    PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
    PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
    PFNGLGENBUFFERSPROC glGenBuffers;
    PFNGLBINDBUFFERPROC glBindBuffer;
    PFNGLBUFFERDATAPROC glBufferData;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
    PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
    PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

  } Gfx;

#ifdef __cplusplus
}
#endif

#endif
