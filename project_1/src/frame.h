#ifndef FRAME_H
#define FRAME_H

#include "config.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Contains data for one disk block.
 */
typedef struct Frame {
  unsigned char content[BLOCK_SIZE];
  // 1-indexed. -1 indicates a free frame.
  int8_t block_id;
  bool dirty;
  bool pinned;
} Frame;

void frame_init(Frame *frame);
void print_frame(const Frame *frame);
/**
 * `block_id` is 1-indexed or -1 (free frame).
 */
void frame_set_block_id(Frame *frame, int8_t block_id);
void frame_set_content(Frame *frame, const unsigned char *content);
void frame_set_dirty(Frame *frame, bool dirty);
void frame_set_pinned(Frame *frame, bool pinned);
int8_t frame_get_block_id(const Frame *frame);
const unsigned char *frame_get_content(const Frame *frame);
/**
 * Unsafe getter: Use only when modification is necessary
 */
unsigned char *frame_get_content_mutable(Frame *frame);
bool frame_is_dirty(const Frame *frame);
bool frame_is_pinned(const Frame *frame);

void print_record(const unsigned char *record);
/**
 * `record_id` is 1-indexed and cannot exceed `NUM_RECORDS_PER_BLOCK`.
 */
const unsigned char *frame_get_record(const Frame *frame, uint8_t record_id);
/**
 * `record_id` is 1-indexed and cannot exceed `NUM_RECORDS_PER_BLOCK`. Sets
 * the `dirty` flag if the record is modified.
 */
int frame_set_record(Frame *frame, uint8_t record_id,
                     const unsigned char *new_record);

#endif // FRAME_H
