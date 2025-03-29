#include "frame.h"
#include "config.h"
#include <stdio.h>
#include <string.h>

void frame_init(Frame *frame) {
  if (frame == NULL) {
    return;
  }

  memset(frame->content, 0, BLOCK_SIZE);
  frame->block_id = -1;
  frame->dirty = false;
  frame->pinned = false;
}

void print_frame(const Frame *frame) {
  if (frame == NULL) {
    return;
  }

  printf("Block ID %d, dirty %d, pinned %d. Records:\n", frame->block_id,
         frame->dirty, frame->pinned);
  for (uint8_t i = 1; i <= NUM_RECORDS_PER_BLOCK; i++) {
    printf("    Record ID: %d, bytes: ", i);
    print_record(frame_get_record(frame, i));
  }
  printf("\n");
}

/* Returns 0 if frame is NULL, -1 if free */
int8_t frame_get_block_id(const Frame *frame) {
  if (frame == NULL) {
    return 0;
  }

  return frame->block_id;
}

const unsigned char *frame_get_content(const Frame *frame) {
  if (frame == NULL) {
    return NULL;
  }

  return frame->content;
}

unsigned char *frame_get_content_mutable(Frame *frame) {
  return (unsigned char *)frame_get_content(frame);
}

bool frame_is_dirty(const Frame *frame) {
  if (frame == NULL) {
    return false;
  }

  return frame->dirty;
}

bool frame_is_pinned(const Frame *frame) {
  if (frame == NULL) {
    return false;
  }

  return frame->pinned;
}

void frame_set_block_id(Frame *frame, int8_t block_id) {
  if (frame == NULL || block_id < -1 || block_id == 0) {
    return;
  }

  frame->block_id = block_id;
}

void frame_set_content(Frame *frame, const unsigned char *content) {
  if (frame == NULL) {
    return;
  }

  memcpy(frame->content, content, BLOCK_SIZE);
}

void frame_set_dirty(Frame *frame, bool dirty) {
  if (frame == NULL) {
    return;
  }

  frame->dirty = dirty;
}

void frame_set_pinned(Frame *frame, bool pinned) {
  if (frame == NULL) {
    return;
  }

  if (frame->pinned == pinned) {
    printf("Frame is already %s\n", pinned ? "pinned" : "unpinned");
    return;
  }

  frame->pinned = pinned;
}

void print_record(const unsigned char *record) {
  if (record == NULL) {
    return;
  }

  fwrite(record, RECORD_SIZE, 1, stdout);
  printf("\n");
}

const unsigned char *frame_get_record(const Frame *frame, uint8_t record_id) {
  if (frame == NULL || record_id == 0 || record_id > NUM_RECORDS_PER_BLOCK) {
    return NULL;
  }

  return frame->content + RECORD_OFFSET(record_id);
}

int frame_set_record(Frame *frame, uint8_t record_id,
                     const unsigned char *new_record) {
  if (frame == NULL || record_id == 0 || record_id >= NUM_RECORDS_PER_BLOCK) {
    return -1;
  }

  // only update if record is different
  if (memcmp(frame->content + RECORD_OFFSET(record_id), new_record,
             RECORD_SIZE) == 0) {
    printf("New data for record #%d is the same. No writes.\n", record_id);
    return -1;
  }

  frame_set_dirty(frame, true);
  memcpy(frame->content + RECORD_OFFSET(record_id), new_record, RECORD_SIZE);
  printf("Wrote new data for record #%d\n", record_id);
  return 0;
}
