#include <ca_ringbuffer.h>

ca_ringbuffer *ca_ringbuffer_new(int length) {
  ca_ringbuffer *buffer = (ca_ringbuffer *)calloc(1, sizeof(ca_ringbuffer));
  buffer->length = length + 1;
  buffer->start = 0;
  buffer->end = 0;
  buffer->buffer = (char *)calloc(buffer->length, 1);
  return buffer;
}

void ca_ringbuffer_destroy(ca_ringbuffer *r) {
  if (r) {
    free(r->buffer);
    free(r);
  }
}

int ca_ringbuffer_write(ca_ringbuffer *r, char *data, int length) {
  if (ca_ringbuffer_avaliable_data(r) == 0) {
    r->start = r->end = 0;
  }

  check(length <= ca_ringbuffer_avaliable_space(r),
        "not enough space: %d request, %d avaliable",
        ca_ringbuffer_avaliable_data(r), length);
  void *result = memcpy(ca_ringbuffer_ends_at(r), data, length);
  check(result != NULL, "Failed To Write Data int buffer");

  ca_ringbuffer_commit_write(r, length);
  return length;

error:
  return -1;
}

int ca_ringbuffer_Read(ca_ringbuffer *r, char *target, int amount) {
  check(amount <= ca_ringbuffer_avaliable_data(r),
        "Not enough in the buffer: has %d, needs %d",
        ca_ringbuffer_avaliable_data(r), amount);
  void *result = memcpy(target, ca_ringbuffer_starts_at(r), amount);
  check(result != NULL, "Failed to write buffer into data.");

  if (r->end == r->start) {
    r->start = r->end = 0;
  }

  return amount;
error:
  return -1;
}

bstring ca_ringbuffer_gets(ca_ringbuffer *r, int amount) {
  check(amount > 0, "Need more than 0 for gets, you gave: %d", amount);
  check(amount <= ca_ringbuffer_avaliable_data(r), "not enough in the buffer.");

  bstring result = blk2bstr(ca_ringbuffer_starts_at(r), amount);

  check(result != NULL, "Failed to create result");
  check(blength(result) == amount, "Wrong length result");

  ca_ringbuffer_commit_read(r, amount);
  check(ca_ringbuffer_avaliable_data(r) >= 0, "error in read commit");

  return result;
error:
  return NULL;
}
