#include "main.h"
#include "circular_buffer.h"
#include "color.h"
#include "command.h"
#include "receiver.h"
#include "transmitter.h"

#include <string.h>

static unsigned char g_cb_data[6144];

int main(void) {
  uint8_t header;
  CircularBuffer cb;
  Storage data = {.handler = &ProcessData};
  Storage commands = {.handler = &ProcessCommands};

  CmdInit();
  RcvInit();
  TrmInit();
  CbInit(&cb, g_cb_data, sizeof g_cb_data);
  RcvListen(&cb, RCV_FLAG_RETRY_IF_OVERWRITE);
  CmdListen();

  for (;;) {
    if (CbConsumeOne(&cb, &header)) {
      if (PACKET_DATA(header)) {
        ProcessNextPacket(&cb, &data, PACKET_SIZE(header));
      } else if (PACKET_COMMAND(header)) {
        ProcessNextPacket(&cb, &commands, PACKET_SIZE(header));
      }
    }
  }
}

void ProcessNextPacket(CircularBuffer* cb,
                       Storage* storage,
                       uint16_t packet_size) {
  uint16_t bytes_available, chunk_size, bytes_processed;

  for (;;) {
    bytes_available = TMP_BUFFER_SIZE - storage->bytes_unprocessed;
    memmove(storage->buffer, storage->buffer + bytes_available,
            storage->bytes_unprocessed);  // Shifting unprocessed bytes left
    chunk_size = MIN(packet_size, TMP_BUFFER_SIZE - storage->bytes_unprocessed);
    if (chunk_size) {
      CbConsumeBlocking(cb, storage->buffer + storage->bytes_unprocessed,
                        chunk_size);
      storage->bytes_unprocessed += chunk_size;
    }
    bytes_processed = storage->handler(storage->buffer, storage->bytes_unprocessed);
    storage->bytes_unprocessed -= bytes_processed;
    if (!bytes_processed) {
      break;
    }
  }
}

uint16_t ProcessCommands(const unsigned char* data, uint16_t bytes_count) {
  uint16_t idx;

  for (idx = 0; idx < bytes_count; ++idx) {
    CmdExecute(data[idx]);
  }
  return bytes_count;
}

uint16_t ProcessData(const unsigned char* data, uint16_t bytes_count) {
  const Bgr888* colors = (const Bgr888*)data;
  uint16_t pos = 0, idx = 0;
  Rgb666 converted_colors[PACKET_MAX_SIZE / sizeof(Bgr888)];

  while (pos + sizeof(Bgr888) <= bytes_count) {
    converted_colors[idx] = ColorCompress(colors[idx]);
    ++idx;
    pos += sizeof(Bgr888);
  }
  TrmSendData(converted_colors, idx * sizeof(Rgb666));
  return pos;
}
