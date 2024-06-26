#include "netpcap.h"
#include <windows.h>
#include <winsock.h>
#include <QDebug>

NetPcap::NetPcap()
{
    qRegisterMetaType< QPair<QString,QString> >("QPair<QString,QString>");
    handle = NULL;
}

NetPcap::~NetPcap()
{
    pcap_freealldevs(alldevs);
    //WSACleanup();
    if(handle != NULL)pcap_close(handle);
}

QVector<DEVInfo> NetPcap::findAllDev()
{
    QVector<DEVInfo> allDev;
    DEVInfo tempDevInfo;
    pcap_if_t *p;
    char errbuf[PCAP_ERRBUF_SIZE];
    closeLiveDev();

    // 获取本地机器设备列表
    if(pcap_findalldevs(&alldevs,errbuf) == -1){
        printf("Find all devices is error: %s\n",errbuf);
        exit(1);
    }
    for(p = alldevs;p;p = p->next){
        tempDevInfo.name = p->name;
        //printf("\tIS loopback address : %s\n",(p->flags & PCAP_IF_LOOPBACK)?"yes":"no");
        if(p->description){
            tempDevInfo.description = p->description;
        }
        else{
            tempDevInfo.description = "(No description available)";
        }
        pcap_addr_t *a;
        for(a = p->addresses;a;a = a->next){
            switch(a->addr->sa_family){
            case AF_INET:
                tempDevInfo.familyName = "AF_INET";
                if (a->addr){
                    //tempDevInfo.address = my_iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr);
                }
                if (a->netmask){
                    //tempDevInfo.netmask = my_iptos(((struct sockaddr_in *)a->netmask)->sin_addr.s_addr);
                }
                if (a->broadaddr)
                    //printf("\tBroadcast Address: %s\n",my_iptos(((struct sockaddr_in *)a->broadaddr)->sin_addr.s_addr));
                    if (a->dstaddr)
                        //printf("\tDestination Address: %s\n",my_iptos(((struct sockaddr_in *)a->dstaddr)->sin_addr.s_addr));
                        break;
            case AF_INET6:
                if (a->addr)
                    //printf("\tAddress: %d\n", inet_ntop(a->addr, ip6str, sizeof(ip6str)));
                    break;
            default:
                break;
            }
        }
        allDev.append(tempDevInfo);
    }
    return allDev;
}

// 打开一个适配器
bool NetPcap::openLiveDev(const char *dev)
{
    closeLiveDev();
    char errBuf[PCAP_ERRBUF_SIZE] = {0};
    //混杂模式
    handle = pcap_open_live(dev,65535,1,0,errBuf);
    if (handle == NULL) {
        return false;
    }
    return true;
}
//关闭适配器
bool NetPcap::closeLiveDev()
{
    if (handle != NULL) {
        pcap_close(handle);
        handle = NULL;
        return true;
    }
    return false;
}

int NetPcap::captureOnce()
{
    if (handle == NULL) {
        return -2;
    }
    return pcap_next_ex(handle, &header, &pkt_data);
}

int NetPcap::sendOnce(const u_char *send_data, qint64 len)
{
    if (handle == NULL) {
        return -2;
    }
    return pcap_sendpacket(handle, send_data, len);
}

void NetPcap::sendtrigger()
{
    mseccount++;
    if(type==0){//连续发包模式
        while(sendcount<sendspeed*mseccount/100+1){
            int flag = sendOnce(send_data, send_len);
            if (flag == 0){
                sendcount++;//发包成功，加1
            }
        }
    }
    else{//固定长度发包模式
        while(sendcount<sendspeed*mseccount/100+1){
            if(sendcount < num){
                int flag = sendOnce(send_data, send_len);
                if (flag == 0){
                    sendcount++;
                }
            }
            else{
                emit signalSendcomplete();
                return;
            }
        }
    }
}
