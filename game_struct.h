#ifndef __GAME_STRUCT_H__
#define __GAME_STRUCT_H__
#include "queue.h"
struct game_s {
  shape **tab_shape;
  direction **tab_dir;
  uint rows;
  uint cols;
  bool wrapping;
  queue *undo_moves;
  queue *redo_moves;
  queue *pointers;
};
#endif  // __GAME_STRUCT_H__