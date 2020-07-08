

/*
  command
  - union struct
  - execute/undo parameters are absolute, don't do deltas

  command_types:
  command_type_entity_move
  command_type_delimiter

  command buffers
  - prev,next
  - size
  - used

  command buffers allocated from level storage

  test command buffers
*/

typedef enum { CommandType_Delimiter = 0, CommandType_EntityMove } CommandType;

typedef struct {
  CommandType type;

  union {
    struct {
      Entity* entity;
      f32     some_f;
    } entity_move;
    struct {
      Entity* entity;
      i32     some_i;
    } entity_rotate;
  } data;
} Command;

typedef struct CommandBuffer {
  Command* command;

  struct CommandBuffer* prev;
  struct CommandBuffer* next;
  usize                 size;
  usize                 used;
} CommandBuffer;

void some_fun() {
  Command command;

  command.type = CommandType_EntityMove;
  command.data.entity_move.some_f = 3.14f;

  RONA_LOG("%.2f", command.data.entity_move.some_f);
}
