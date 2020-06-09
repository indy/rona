#ifndef MESH_HERO_H
#define MESH_HERO_H

// void mesh_hero_render(Mesh *mesh, RonaGl *gl, Mat4 *proj_matrix);
void mesh_hero_lib_load(Mesh *mesh, RonaGl *gl, MemoryArena *transient);
void mesh_hero_lib_unload(Mesh *mesh, RonaGl *gl);

#endif /* MESH_HERO_H */
