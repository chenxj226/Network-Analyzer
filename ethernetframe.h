#ifndef ETHERNETFRAME_H
#define ETHERNETFRAME_H

#define FRAME_DATA_LENGTH		1024
#define FRAME_DATA_HEAD_LENGTH	18

#pragma pack(1)

struct ethernet_frame {
    unsigned char dst_mac[6];
    unsigned char src_mac[6];
    unsigned char type[2];
}ethernet_frame;


extern volatile int self_check_flag;
extern volatile int handshake_flag;
extern volatile int handshake_count;
extern volatile int config_device_flag;
int parse_ethernet_frame(unsigned char *frame, int frame_length);

#endif // ETHERNETFRAME_H
