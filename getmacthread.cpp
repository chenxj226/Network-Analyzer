#include "getmacthread.h"
#include <QDebug>


GetMacThread::GetMacThread(const char *devname,const char *ipAddr)
{ 
    char errBuf[PCAP_ERRBUF_SIZE] = {0};

    strcpy(hostIp,ipAddr);
    //混杂模式
    handle = pcap_open_live(devname,65535,1,0,errBuf);

    if(!handle){
        qDebug()<< "Open live dev is error: " << errBuf;
        this->destroyed();
    }
    qDebug()<< devname << " " << hostIp;
}

GetMacThread::GetMacThread(pcap_t *handle,const char *ipAddr)
{
    this->handle = handle;
    strcpy(hostIp,ipAddr);
}

GetMacThread::~GetMacThread()
{
    delete this;
}

// 获取本机Mac线程
QString GetMacThread::getSelfMac()
{
    unsigned char mac[6] = {0};
    unsigned char sendbuf[42] = {0};
    int res;
    EthernetHeader eh;
    ArpHeader ah;

    if(!handle){
        printf("The Adapter is not be opened! Please Check!\n");
        return NULL;
    }
    //将已开辟内存空间 eh.dest_mac_add 的首6个字节的值设为值 0xff。
    memset(eh.DestMAC, 0xFF, 6);
    memset(eh.SourMAC, 0x00, 6);
    eh.EthType = my_htons(ARP_TYPE);

    ah.HardwareType= my_htons(ARP_HARDWARE);
    ah.ProtocolType = my_htons(IP_TYPE);
    ah.HardwareAddLen = 6;
    ah.ProtocolAddLen = 4;
    ah.OperationField = my_htons(ARP_REQUEST);
    memset(ah.SourceMacAdd, 0x00, 6);
    memset(ah.SourceIpAdd,0x00,4);
    memset(ah.DestMacAdd, 0xFF, 6);    
    u_long hostNIp = my_htonl(my_inet_addr(hostIp));
    memcpy(ah.DestIpAdd,(u_char *)&hostNIp,4);

    memset(sendbuf, 0, ARP_PACKET_LENGTH);
    memcpy(sendbuf, &eh,ETHERNET_HEAD_LENGTH);
    memcpy(sendbuf + ETHERNET_HEAD_LENGTH, &ah, ARP_PACKET_LENGTH);
    // 发送N个广播包
    for(int i = 0; i < 5 ; ++i){
        if(pcap_sendpacket(handle, sendbuf, ARP_PACKET_LENGTH) == 0) {
        }
        else{
            printf("PacketSendPacket in getmine Error:\n");
            return NULL;
        }
    }

    struct pcap_pkthdr * pktHeader;
    const u_char * pktData;

    union IP{
        unsigned int ip;
        unsigned char nip[4];
    }ipUnion;

    char ipStr[3*4+3+1] = {0};

    //
    while((res = pcap_next_ex(handle, &pktHeader, &pktData)) >= 0){
        if (*(unsigned short *) (pktData + 12) == my_htons(ARP_TYPE)
                && *(unsigned short*) (pktData + 20) == my_htons(ARP_REPLY)){
            //获取Source ip
            for(int i=0; i < 4 ; i++){
                ipUnion.nip[i] = *(unsigned char *)(pktData + 28 + i);
            }
            iptos(ipUnion.ip,ipStr);
            //收到的arp包的源ip等于本机ip,则获取本机Mac（源Mac）
            if(strncmp(hostIp,ipStr,strlen(hostIp)) == 0){
                for(int i = 0; i < 6; i++) {
                    mac[i] = *(unsigned char *) (pktData + 22 + i);
                }
                break;
            }
            //获取接收的Dest ip
            for(int i = 0; i < 4 ; i++){
                ipUnion.nip[i] = *(unsigned char *)(pktData + 38 + i);
            }
            iptos(ipUnion.ip,ipStr);
            //收到的arp包的源ip等于本机ip,则获取本机Mac（源Mac）
            if(strncmp(hostIp,ipStr,strlen(hostIp)) == 0){
                for (int i = 0; i < 6; i++) {
                    mac[i] = *(unsigned char *) (pktData + 32 + i);
                }
                break;
            }
        }
        else{
            //qDebug() << "Not Reply Packet";
        }       
    }

    char macStr[64] = {0};
    sprintf(macStr,"%02x-%02x-%02x-%02x-%02x-%02x",mac[0],mac[1],mac[2],mac[3], mac[4], mac[5]);
    // qDebug("Thread get mac: %s\n ",macStr);
    // 发送获取本机Mac地址完成信号(获取Mac地址要一定的时间)
    emit getSelfMacFinishedSig(QString(macStr));

    return QString(macStr);
}

void GetMacThread::run()
{

    getSelfMac();    
}
