#ifndef MESH_BLOCK_H
#define MESH_BLOCK_H

// void mesh_block_render(Mesh *mesh, RonaGl *gl, Mat4 *proj_matrix);
void mesh_block_lib_load(Mesh *mesh, RonaGl *gl, MemoryArena *transient);
void mesh_block_lib_unload(Mesh *mesh, RonaGl *gl);

#endif /* MESH_BLOCK_H */
