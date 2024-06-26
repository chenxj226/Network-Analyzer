// ---------------------------------------------
// TCP/IP数据包操作类
// ---------------------------------------------
#ifndef TCPIPPROTOCOL_H
#define TCPIPPROTOCOL_H

#include "tcpipcommon.h"
#include <QString>


//namespace TcpIpProtocol {
//    class YEthernetPacket;
//    class YArpPacket;
//    class YIPPacket;
//    class YTcpPacket;
//    class YUdpPacket;
//    class YHttPPacket;
//}


//***********************************************************************************
//YEthernetPacket Class
//***********************************************************************************
class YEthernetPacket
{
public:
    YEthernetPacket();

public:
    /** 单独设置 **/
    // setSrcMac(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
    void setSrcMac(u_char m0,u_char m1,u_char m2,u_char m3,u_char m4,u_char m5);
    // setDestMac(0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
    void setDestMac(u_char m0,u_char m1,u_char m2,u_char m3,u_char m4,u_char m5);
    // setType(0x0806);
    void setType(u_short type);
    // void getData(&dataStruct);
    u_char* getData();
    QString getEtherSrcMacAdd(void);
    QString getEtherDestMacAdd(void);
    u_short getEtherNetType();
    void setData(const u_char *data);
    /** 一次设置 **/
    void fillEthernetHeader(u_char* srcMac, u_char* detMac, u_short type);
private:    
    u_char data[ETHERNET_HEAD_LENGTH];
};

//***********************************************************************************
//YIPHeaderPacket Class
//***********************************************************************************
class YIPHeaderPacket
{
public:
    YIPHeaderPacket();
    void setData(const u_char *pktData);
    // TCP/UDP/ICMP
    QString getProtocolType(void);
    QString getSourceIpAddStr(void);
    QString getDestIpAddStr(void);
private:
    u_char data[IP_HEAD_LENGTH];
};

//***********************************************************************************
//YICMPHeaderPacket Class Ping实现，无需data,只需要ICMP头即可，关键是校验和生成
//***********************************************************************************
class YICMPHeaderPacket
{
public:
    YICMPHeaderPacket();
    void setType(u_char type);
    void setCode(u_char code);
    void setIdent(u_short ident);
    void setSeq(u_short seq);
    // 不包括校验和
    void fillICMPHeader(u_char type,u_char code,u_short ident,u_short seq);
    // 计算校验和
    u_short calcCheckSum(u_short *icmpHeader,int headerLen);
    //

private:
    u_char data[ICMP_HEAD_LENGTH];
};

//***********************************************************************************
//YArpPacket Class
//***********************************************************************************
class YArpPacket
{
public:
    YArpPacket();
public:
    // setHardwareType(ARP_HARDWARE);
    void setHardwareType(u_short type = 0x0001);
    //
    void setProtocolType(u_short type = 0x0800);
    //
    void setHardwareAddLen(u_char len = 0x06);
    //
    void setProtocolAddLen(u_char len = 0x04);
    //
    void setOperationField(u_short type);
    //
    void setSourceMacAdd(u_char m0,u_char m1,u_char m2,u_char m3,u_char m4,u_char m5);
    //
    void setSourIpAdd(u_int);
    //
    void setSourIpAdd(const char *ipStr);
    //
    void setDestMacAdd(u_char m0,u_char m1,u_char m2,u_char m3,u_char m4,u_char m5);\
    //
    void setDestIpAdd(u_int);
    //
    void setDestIpAdd(const char *ipStr);
    /** fillArpPacket 硬件len=6,协议len=4已默认 **/
    void fillArpPacket(u_short hdType,u_short proType,u_short opFilt,
                       u_char *srcMac,char *srcIp,u_char *destMac,char *destIp);
    // 将EthernetHeader 和 ArpHeader 整合成ArpPacket data
    void setData();
    void setData(const u_char *data);
    /** 获取数据 **/
    void clearData();
    //
    u_char * getData();
    //
    u_short getEtherNetType();
    //
    u_short getHardwareType();
    //
    u_short getProtocolType();
    //
    u_short getOperationField();
    //
    QString getSourceMacAdd();
    //    
    QString getSourceIpAddStr();
    //
    u_long getSourceIpAdd();
    //
    QString getDestIpAddStr();
    //
    u_long getDestIpAdd();
    //
    QString getDestMacAdd();
    //
    YEthernetPacket &getEthernetPacket();
private:
    YEthernetPacket etherPacket;
    ArpPacket arpPacket;
    ArpHeader arpBody;
    u_char data[ARP_PACKET_LENGTH];
};

//***********************************************************************************
//YIPPacket Class
//***********************************************************************************
class YIPPacket
{

};

//***********************************************************************************
//YTcpPacket Class
//***********************************************************************************

#define HOST_PORT  0xc522        // 本机端口（随便搞个）：

class YTcpPacket
{
public:
    YTcpPacket();
    YTcpPacket(u_char *srcMac,u_char *destMac,char *srcIp,char *destIp,u_short port);

private:
    YEthernetPacket *ethernetHeader;
    YIPHeaderPacket *ipHeader;

    u_char data[TCP_PACKET_LENGTH];
private:
    u_short calcCheckNum(u_short* buffer, int size);
public:
    void setData(const u_char *pktData);
    u_char* getData(void);
    u_char getFlag();
    u_short getSrcPort();
    QString getDestIpAddStr(void);
    void setScanPort(u_short port);
};

//***********************************************************************************
//YUdpPacket Class
//***********************************************************************************
class YUdpPacket
{

};

//***********************************************************************************
//YHttPPacket Class
//***********************************************************************************
class YHttPPacket
{

};

#endif // TCPIPPROTOCOL_H
