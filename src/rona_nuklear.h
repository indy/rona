#ifdef RONA_NUKLEAR

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define UNUSED(a) (void)a
#define LEN(a) (sizeof(a) / sizeof(a)[0])

#ifdef __APPLE__
#define NK_SHADER_VERSION "#version 150\n"
#else
#define NK_SHADER_VERSION "#version 300 es\n"
#endif

typedef struct {
#ifdef RONA_NUKLEAR_DEMO_WITH_IMAGES
  struct nk_font* font_14;
  struct nk_font* font_18;
  struct nk_font* font_20;
  struct nk_font* font_22;

  struct nk_image unchecked;
  struct nk_image checked;
  struct nk_image rocket;
  struct nk_image cloud;
  struct nk_image pen;
  struct nk_image play;
  struct nk_image pause;
  struct nk_image stop;
  struct nk_image prev;
  struct nk_image next;
  struct nk_image tools;
  struct nk_image dir;
  struct nk_image copy;
  struct nk_image convert;
  struct nk_image del;
  struct nk_image edit;
  struct nk_image images[9];
  struct nk_image menu[6];
#else
  struct nk_font* default_font;
#endif /* RONA_NUKLEAR_DEMO_WITH_IMAGES */
} NuklearMedia;

typedef struct {
  struct nk_font_atlas atlas;
  struct nk_context    ctx;

  usize nuklear_memory_size;
  void* nuklear_memory;

  // -----------------------------------------------------------------------------
  //
  // memory management variables for font atlas functionality
  //
  struct nk_allocator persistent; // todo: can't these nk_allocators just be local variables?
  struct nk_allocator transient;

  BumpAllocator bump_permanent; // reserves MEMORY_ALLOCATION_NUKLEAR_ATLAS from arena_permanent
  GroupedAllocator allocator_permanent;

  BumpAllocator bump_transient; // reserves transient memory, so should be used with caution
  GroupedAllocator allocator_transient;

  bool transient_allocation_calls_expected;
  // -----------------------------------------------------------------------------

  // render the stage_texture onto stage_in_nuklear_texture_id applying colour space conversion
  GLuint stage_in_nuklear_texture_id;
  GLuint depth_texture_id;
  GLuint framebuffer_id;

  struct nk_buffer            cmds;
  struct nk_draw_null_texture null;
  GLuint                      vbo, vao, ebo;
  GLuint                      prog;
  GLuint                      vert_shdr;
  GLuint                      frag_shdr;
  GLint                       attrib_pos;
  GLint                       attrib_uv;
  GLint                       attrib_col;
  GLint                       uniform_tex;
  GLint                       uniform_proj;
  GLuint                      font_tex;
} NuklearState;

static NuklearState CR_STATE nuklear_state;
static NuklearMedia CR_STATE nuklear_media;

void *nuklear_persistent_alloc(nk_handle h, void *mem, nk_size bytes);
void nuklear_persistent_free(nk_handle h, void *mem);
void *nuklear_transient_alloc(nk_handle h, void *mem, nk_size bytes);
void nuklear_transient_free(nk_handle h, void *mem);


#endif /* RONA_NUKLEAR */
