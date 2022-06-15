#include "circular_buffer.h"
#include "color.h"
#include "command.h"
#include "packet.h"
#include "receiver.h"
#include "transmitter.h"

static unsigned char g_cb_data[6144];
static CircularBuffer g_cb;
static Rgb666 g_converted_colors[PACKET_MAX_SIZE / sizeof(Bgr888)];

void ProcessCommands(const uint8_t* data, uint8_t count)
{
	while (count--)
	{
		CmdExecute(*data++);
	}
}

void ProcessData(const unsigned char* data, uint8_t count)
{
	const Bgr888* colors = (const Bgr888*)data;
	uint8_t pos, idx;

	for (pos = 0, idx = 0; pos < count; pos += sizeof(Bgr888), ++idx)
	{
		g_converted_colors[idx] = ColorCompress(colors[idx]);
	}
	TrmSendData(g_converted_colors, idx);
}

int main(void)
{
	uint8_t header;
	unsigned char buffer[PACKET_MAX_SIZE];

	CmdInit();
	RcvInit();
	TrmInit();
	CbInit(&g_cb, g_cb_data, sizeof g_cb_data);
	RcvListen(&g_cb, RCV_FLAG_RETRY_IF_OVERWRITE);
	CmdListen();

	for (;;)
	{
		if (CbConsumeOne(&g_cb, &header))
		{
			CbConsumeBlocking(&g_cb, buffer, PACKET_SIZE(header));
			if (PACKET_DATA(header))
			{
				ProcessData(buffer, PACKET_SIZE(header));
			}
			else
			{
				ProcessCommands(buffer, PACKET_SIZE(header));
			}
		}
	}
}
