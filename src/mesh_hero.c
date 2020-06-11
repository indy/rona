
// void mesh_hero_render(Mesh *mesh, RonaGl *gl, Mat4 *proj_matrix) {
//   gl->useProgram(mesh->shader_program);
//   gl->uniform4f(mesh->uniform_colour, 0.0f, 1.0f, 0.0f, 1.0f);
//   gl->uniformMatrix4fv(mesh->uniform_proj_matrix, 1, false, (GLfloat *)&(proj_matrix->v));

//   gl->bindVertexArray(mesh->vao);
//   gl->drawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
// }

void mesh_hero_lib_load(Mesh *mesh, RonaGl *gl, MemoryArena *transient) {
  gl->genVertexArrays(1, &mesh->vao); // Vertex Array Object
  gl->bindVertexArray(mesh->vao);

  #include "../target/shader.vert.c"
  SHADER_AS_STRING(transient, vertexSource, shader_vert);

  #include "../target/shader.frag.c"
  SHADER_AS_STRING(transient, fragmentSource, shader_frag);

  mesh->shader_program = create_shader_program(gl, vertexSource, fragmentSource);

  f32 half_dim = 0.4f;
  f32 vertices[] = {
    -half_dim,  half_dim,
    -half_dim, -half_dim,
     half_dim, -half_dim,
     half_dim,  half_dim
  };
  u32 indices[] = {
    0, 1, 2,
    0, 2, 3
  };

  mesh->num_elements = 6;

  // the type of a Vertex Buffer Object is GL_ARRAY_BUFFER
  //
  GLuint vbo;
  gl->genBuffers(1, &vbo);
  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // the data is set only once and used many times.
  gl->bindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint ebo;
  gl->genBuffers(1, &ebo);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  gl->bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  gl->useProgram(mesh->shader_program);

  mesh->uniform_colour = gl->getUniformLocation(mesh->shader_program, "colour");
  mesh->uniform_proj_matrix = gl->getUniformLocation(mesh->shader_program, "proj_matrix");
  mesh->uniform_pos = gl->getUniformLocation(mesh->shader_program, "pos");

  gl->bindBuffer(GL_ARRAY_BUFFER, vbo);
  gl->enableVertexAttribArray(0);
  gl->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);

  gl->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  gl->bindVertexArray(0);
}

void mesh_hero_lib_unload(Mesh *mesh, RonaGl *gl) {
  gl->deleteVertexArrays(1, &mesh->vao);
}
