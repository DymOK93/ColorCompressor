#include "circular_buffer.h"
#include "color.h"
#include "command.h"
#include "main.h"
#include "packet.h"
#include "receiver.h"
#include "transmitter.h"

#include <string.h>

static unsigned char g_cb_data[6144];

int main(void)
{
	uint8_t header;
	uint16_t bytes_unprocessed = 0;
	uint16_t bytes_processed = 0;
	
	CircularBuffer cb;
	unsigned char buffer[PACKET_MAX_SIZE];

	CmdInit();
	RcvInit();
	TrmInit();
	CbInit(&cb, g_cb_data, sizeof g_cb_data);
	RcvListen(&cb, RCV_FLAG_RETRY_IF_OVERWRITE);
	CmdListen();

	for (;;)
	{
		if (CbConsumeOne(&cb, &header) && PACKET_VALID(header))
		{
			bytes_processed += PACKET_SIZE(header);
			bytes_unprocessed = ProcessNextPacket(&cb, buffer, sizeof buffer, bytes_unprocessed, header);
			bytes_processed -= bytes_unprocessed;
		}
	}
}

uint16_t ProcessNextPacket(
	CircularBuffer* cb,
	unsigned char* buffer,
	uint16_t buffer_size,
	uint16_t bytes_unprocessed,
	uint8_t header)
{
	uint16_t packet_size = PACKET_SIZE(header);
	uint16_t bytes_read, bytes_available;

	while (packet_size > 0)
	{
		bytes_available = buffer_size - bytes_unprocessed;
		memmove(buffer, buffer + bytes_available, bytes_unprocessed); // Shift unprocessed bytes left
		bytes_read = MIN(packet_size, bytes_available);
		CbConsumeBlocking(cb, buffer + bytes_unprocessed, bytes_read);
		packet_size -= bytes_read;
		bytes_unprocessed += bytes_read;

		if (PACKET_DATA(header))
		{
			bytes_unprocessed -= ProcessData(buffer, bytes_unprocessed);
		}
		else
		{
			bytes_unprocessed -= ProcessCommands(buffer, bytes_unprocessed);
		}
	}
	if (bytes_unprocessed > 2)
	{
		return bytes_unprocessed;
	}
	return bytes_unprocessed;
}

uint16_t ProcessCommands(const uint8_t* data, uint16_t count)
{
	uint16_t idx;

	for (idx = 0; idx < count; ++idx)
	{
		CmdExecute(data[idx]);
	}
	return count;
}

uint16_t ProcessData(const unsigned char* data, uint16_t count)
{
	const Bgr888* colors = (const Bgr888*)data;
	uint16_t pos = 0, idx = 0;
	Rgb666 converted_colors[PACKET_MAX_SIZE / sizeof(Bgr888)];

	while (pos + sizeof(Bgr888) <= count)
	{
		converted_colors[idx] = ColorCompress(colors[idx]);
		++idx;
		pos += sizeof(Bgr888);
	}
	TrmSendData(converted_colors, idx * sizeof(Rgb666));
	return pos;
}
