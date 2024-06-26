#include "tcpipprotocol.h"

//***********************************************************************************
//EthernetPacket Class
//***********************************************************************************
YEthernetPacket::YEthernetPacket()
{

}

void YEthernetPacket::fillEthernetHeader(u_char* srcMac, u_char* detMac, u_short type)
{
    memset(data,0,ETHERNET_HEAD_LENGTH);
    EthernetHeader* ethHdr = (EthernetHeader*)data;

    memcpy(ethHdr->SourMAC, srcMac, sizeof(u_char) * 6);
    memcpy(ethHdr->DestMAC, detMac, sizeof(u_char) * 6);
    ethHdr->EthType = my_htons(type);
}

u_char* YEthernetPacket::getData()
{
    return data;
}

QString YEthernetPacket::getEtherSrcMacAdd()
{
    u_char mac[6] = {0};
    char macBuf[64] = {0};
    for (int i = 0; i < 6; i++) {
        mac[i] = *(unsigned char *) (data + 6 + i);
    }
    sprintf(macBuf,"%02x-%02x-%02x-%02x-%02x-%02x",mac[0],mac[1],mac[2],mac[3], mac[4], mac[5]);
    return QString(macBuf);
}

QString YEthernetPacket::getEtherDestMacAdd()
{
    u_char mac[6] = {0};
    char macBuf[64] = {0};
    for (int i = 0; i < 6; i++) {
        mac[i] = *(unsigned char *) (data + 0 + i);
    }
    sprintf(macBuf,"%02x-%02x-%02x-%02x-%02x-%02x",mac[0],mac[1],mac[2],mac[3], mac[4], mac[5]);
    return QString(macBuf);
}

// 只能判断是否是ARP包或者是IP包（TCP/UDP/ICMP包）
u_short YEthernetPacket::getEtherNetType()
{
    u_short etherType = (*(u_short *)(data + 12));
    return etherType;
}

void YEthernetPacket::setData(const u_char *data)
{
    memcpy(this->data,data,ETHERNET_HEAD_LENGTH);
}

//***********************************************************************************
//YIPHeaderPacket Class
//***********************************************************************************
YIPHeaderPacket::YIPHeaderPacket()
{
    memset(data,0,IP_HEAD_LENGTH);
}

void YIPHeaderPacket::setData(const u_char *pktData)
{
    memcpy(data,pktData+14,IP_HEAD_LENGTH);
}

QString YIPHeaderPacket::getProtocolType()
{
    u_char protocolType = (*(u_char *)(data + 9));

    if(protocolType == (IP_TCP_TYPE)){
        return "TCP";
    }
    else if(protocolType == (IP_ICMP_TYPE)){
        return "ICMP";
    }
    else if(protocolType == (IP_UDP_TYPE)){
        return "UDP";
    }

    return "ERROR";
}

QString YIPHeaderPacket::getSourceIpAddStr()
{
    u_long ipN = *(u_long *) (data + 12);
    char *str = my_iptos(ipN);
    return QString(str);
}

QString YIPHeaderPacket::getDestIpAddStr()
{
    u_long ipN = *(u_long *) (data + 16);
    char *str = my_iptos(ipN);
    return QString(str);
}
//***********************************************************************************
//YICMPHeaderPacket Class Ping实现，无需data,只需要ICMP头即可，关键是校验和生成
//***********************************************************************************

YICMPHeaderPacket::YICMPHeaderPacket()
{

}

void YICMPHeaderPacket::setType(u_char type)
{

}

void YICMPHeaderPacket::setCode(u_char code)
{

}

void setIdent(u_short ident)
{

}

void setSeq(u_short seq)
{

}

u_short calcCheckSum(u_short *icmpHeader,int headerLen)
{
    int nleft = headerLen;
    int sum = 0;
    unsigned short* w = icmpHeader;
    unsigned short answer = 0;

    while(nleft > 1){
        sum += *w++;
        nleft -= 2;
    }

    if(nleft == 1){
        *(unsigned char*)(&answer) = *(unsigned char*)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff); //高16位 + 低16位
    sum += (sum >> 16);                 //+进位
    answer = ~sum;                      //取反

    return (answer);
}


//***********************************************************************************
//ArpPacket Class
//***********************************************************************************
YArpPacket::YArpPacket()
{
    memset(data,0,ARP_PACKET_LENGTH);
}

//
YEthernetPacket &YArpPacket::getEthernetPacket()
{
    return etherPacket;
}

void YArpPacket::setDestIpAdd(u_int hdestIp)
{
    ArpPacket *packet = (ArpPacket *)data;
    memcpy(packet->ah.DestIpAdd,(u_char*)&hdestIp,4);
}

// fillArpPacket 硬件len=6,协议len=4已默认
void YArpPacket::fillArpPacket(u_short hdType,u_short proType,u_short opFilt,
                   u_char* srcMac,char *srcIp,u_char *destMac,char *destIp)
{
    ArpHeader *ah = &arpBody;

    ah->HardwareAddLen = 6;
    ah->ProtocolAddLen = 4;

    ah->HardwareType = my_htons(hdType);
    ah->ProtocolType = my_htons(proType);
    ah->OperationField = my_htons(opFilt);
    memcpy(ah->SourceMacAdd, srcMac, 6);
    memcpy(ah->DestMacAdd, destMac, 6);    
    u_long srcIpN = my_htonl(my_inet_addr(srcIp));
    memcpy(ah->SourceIpAdd,(u_char*)&srcIpN,4);
    u_long destIpN = my_htonl(my_inet_addr(destIp));
    memcpy(ah->DestIpAdd,(u_char*)&destIpN,4);
}

// 将EthernetHeader 和 ArpHeader 整合成ArpPacket data
void YArpPacket::setData()
{
    memcpy(data,etherPacket.getData(),ETHERNET_HEAD_LENGTH);
    memcpy(data+ETHERNET_HEAD_LENGTH,&arpBody,ARP_BODY_LENGTH);
}

void YArpPacket::setData(const u_char *data)
{
    memcpy(this->data,data,ARP_PACKET_LENGTH);
}

void YArpPacket::clearData()
{
    memset(data,0x00,ARP_PACKET_LENGTH);
}

u_char * YArpPacket::getData()
{
    return data;
}

u_short YArpPacket::getEtherNetType()
{
    u_short etherType = (*(u_short *)(data + 12));
    return etherType;
}

//
u_short YArpPacket::getHardwareType()
{

}

//
u_short YArpPacket::getProtocolType()
{

}

//
u_short YArpPacket::getOperationField()
{
    u_short opFiled = (*(u_short *)(data + 20));
    return opFiled;
}

//
QString YArpPacket::getSourceMacAdd()
{
    u_char mac[6] = {0};
    char macBuf[64] = {0};
    for (int i = 0; i < 6; i++) {
        mac[i] = *(unsigned char *) (data + 22 + i);
    }
    sprintf(macBuf,"%02x-%02x-%02x-%02x-%02x-%02x",mac[0],mac[1],mac[2],mac[3], mac[4], mac[5]);
    return QString(macBuf);
}

//
u_long YArpPacket::getSourceIpAdd()
{
    u_long ipN = *(u_long *) (data + 28);
    return ipN;
}

QString YArpPacket::getSourceIpAddStr()
{
    u_long ipN = *(u_long *) (data + 28);
    char *str = my_iptos(ipN);
    return QString(str);
}


//
QString YArpPacket::getDestMacAdd()
{
    u_char mac[6] = {0};
    char macBuf[64] = {0};
    for (int i = 0; i < 6; i++) {
        mac[i] = *(unsigned char *) (data + 32 + i);
    }
    sprintf(macBuf,"%02x-%02x-%02x-%02x-%02x-%02x",mac[0],mac[1],mac[2],mac[3], mac[4], mac[5]);
    return QString(macBuf);
}

QString YArpPacket::getDestIpAddStr()
{
    u_long ipN = *(u_long *) (data + 38);
    char *str = my_iptos(ipN);
    return QString(str);
}

//
u_long YArpPacket::getDestIpAdd()
{
    u_long ipN = *(u_long *) (data + 38);
    return ipN;
}

//***********************************************************************************
//YTcpPacket Class
//***********************************************************************************
YTcpPacket::YTcpPacket()
{
    memset(data,0,TCP_PACKET_LENGTH);
}

YTcpPacket::YTcpPacket(u_char *srcMac,u_char *destMac,char *srcIp,char *destIp,u_short port)
{
    EthernetHeader eh;
    IPHeader ih;
    TCPPacketHeader th;
    memset(data,0,TCP_PACKET_LENGTH);
    // 以太网头部
    memcpy(eh.SourMAC, srcMac, 6);          //源MAC地址为自己的MAC地址
    memcpy(eh.DestMAC, destMac, 6);         //目的MAC地址
    eh.EthType = htons(IP_TYPE);            //IPV4类型
    // IP头部
    ih.VerIhl = 0x45;                       //IPV4为4，长度0101（20字节）,已是网络字节序
    ih.Tos = 0x00;                          //区分服务，置零即可
    ih.Tlen = my_htons(IP_HEAD_LENGTH+TCP_HEAD_LENGTH);
    ih.Identification = my_htons(GetCurrentProcessId());
    ih.FlagsFo = 0x00;                      //标志和偏移，置零即可
    ih.Ttl = 128;                           //TTL
    ih.Proto = IP_TCP_TYPE;                 //协议名设置为TCP
    ih.Crc = 0x0000;                        //校验和置零，填充完头部后计算
    u_long srcIpN = my_htonl(my_inet_addr(srcIp));
    memcpy(ih.SourceIpAdd,(u_char*)&srcIpN,4);
    u_long destIpN = my_htonl(my_inet_addr(destIp));
    memcpy(ih.DestIpAdd,(u_char*)&destIpN,4);
    // IP头部校验和
    char *cksbuf = new char[20];
    memcpy(cksbuf, &ih, 20);
    ih.Crc = (calcCheckNum((u_short *)cksbuf, IP_HEAD_LENGTH));
    delete cksbuf;
    // TCP数据
    th.SrcPort = my_htons(HOST_PORT);       //我的端口
    th.DestPort = my_htons(port);           //当前扫描的端口号
    th.Seq = my_htons(1);
    th.Ack = 0;
    th.len = 0x50;                          //TCP首部长度（数值*4）
    th.flags = 0x02;                        //SYN
    th.Win = my_htons(1024);                //窗口大小
    th.Sum = 0x0000;                        //校验和先置零
    th.Urp = 0x0000;                        //URGent Number
    // TCP伪头部填充
    PsedoTCPHead pth;
    srcIpN = my_htonl(my_inet_addr(srcIp));
    memcpy(pth.source_addr,(u_char*)&srcIpN,4);
    destIpN = my_htonl(my_inet_addr(destIp));
    memcpy(pth.dest_addr,(u_char*)&destIpN,4);
    pth.zero = 0;
    pth.protocol = (IP_TCP_TYPE);
    pth.seg_len = my_htons(TCP_HEAD_LENGTH);//长度20（6bit）

    // 计算TCP首部的校验和：伪首部在前，与首部一起参与计算，随后丢弃伪首部
    char *buf = new char[32];
    memcpy(buf, &pth, sizeof(PsedoTCPHead));
    memcpy(buf + sizeof(PsedoTCPHead), &th, TCP_HEAD_LENGTH);
    th.Sum = (calcCheckNum((u_short *)buf, sizeof(PsedoTCPHead)+TCP_HEAD_LENGTH));
    delete buf;
    // 把做好的数据包装入缓存
    memset(data, 0, sizeof(data));
    memcpy(data, &eh, ETHERNET_HEAD_LENGTH);
    memcpy(data + ETHERNET_HEAD_LENGTH,&ih, IP_HEAD_LENGTH);
    memcpy(data + ETHERNET_HEAD_LENGTH + IP_HEAD_LENGTH,&th,TCP_HEAD_LENGTH);
}


u_short YTcpPacket::calcCheckNum(u_short* buffer, int size)
{
    u_long cksum = 0;
    while(size > 1){
        cksum += *buffer++;
        size -= sizeof(u_short);                //按双字节（16位）对齐
    }
    if(size){
        cksum += *(u_char*)buffer;             //二进制反码求和
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);//将高16bit与低16bit相加
    cksum += (cksum >> 16);                  //将进位到高位的16bit与低16bit 再相加
    return (u_short)(~cksum);
}

void YTcpPacket::setData(const u_char *pktData)
{
    memcpy(this->data,pktData,TCP_PACKET_LENGTH);
}

u_char* YTcpPacket::getData(void)
{
    return data;
}

u_short YTcpPacket::getSrcPort()
{
    u_short * p = (u_short *)(data+34);
    return *p;
}

u_char YTcpPacket::getFlag()
{
    u_char * p = (u_char *)(data+47);
    return *p;
}

QString YTcpPacket::getDestIpAddStr(void)
{
    u_long ipN = *(u_long *) (data + 16);
    char *str = my_iptos(ipN);
    return QString(str);
}


void YTcpPacket::setScanPort(u_short port)
{
    u_short *p = (u_short*)(data+36);
    *p = my_htons(port);
}
