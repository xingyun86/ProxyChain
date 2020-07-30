// ProxyChain.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <string>
#include <vector>
#include <winsock2.h>

#ifdef _MSC_VER
#define NETWORK_STARTUP() WSADATA wsaData = { 0 }; WSAStartup(MAKEWORD(2, 2), &wsaData)
#define NETWORK_CLEANUP() WSACleanup()
#else
#define NETWORK_STARTUP()
#define NETWORK_CLEANUP()
#endif

// TODO: Reference additional headers your program requires here.
///定义的结构
#pragma pack(1)
struct sock4req1
{
    uint8_t VN;
    uint8_t CD;
    uint16_t Port;
    uint32_t IPAddr;
    uint8_t other[1];
};

struct sock4ans1
{
    uint8_t VN;
    uint8_t CD;
};

struct sock5req1
{
    uint8_t Ver;
    uint8_t nMethods;
    uint8_t Methods[255];
};

struct sock5ans1
{
    uint8_t Ver;
    uint8_t Method;
};

struct sock5req2
{
    uint8_t Ver;
    uint8_t Cmd;
    uint8_t Rsv;
    uint8_t Atyp;
    uint32_t IPAddr;
    uint16_t Port;
    //uint8_t other[1];
};

struct sock5ans2
{
    uint8_t Ver;
    uint8_t Rep;
    uint8_t Rsv;
    uint8_t Atyp;
    uint8_t other[1];
};

struct authreq
{
    uint8_t Ver;
    uint8_t Ulen;
    uint8_t Name[255];
    uint8_t PLen;
    uint8_t Pass[255];
};

struct authans
{
    uint8_t Ver;
    uint8_t Status;
};
#pragma pack()

#define DRIVE_CACHE_BYTES 4096
char DriveCache[DRIVE_CACHE_BYTES];
SOCKET clientSocket = INVALID_SOCKET;

///////////代码片段
void domain_to_ipaddr(std::vector<std::string>& ip_list, const char* domain_name)
{
    struct hostent* hostinfo = gethostbyname(domain_name);//信息结构体
    if (hostinfo != nullptr)
    {
        while ((hostinfo->h_addr_list != nullptr)
            && *(hostinfo->h_addr_list) != nullptr) {
            ip_list.emplace_back(inet_ntoa(*(struct in_addr*)(*hostinfo->h_addr_list)));
            hostinfo->h_addr_list++;
        }
    }
}

void send_and_recv(SOCKET sock, const char* send_data, int& send_size, char* recv_data, int& recv_size)
{
    send_size = send(sock, send_data, send_size, 0);
    printf("send %d bytes\n", send_size);
    if (recv_data != nullptr)
    {
        memset(recv_data, 0, recv_size);
        recv_size = recv(clientSocket, recv_data, recv_size, 0);
        printf("recv %d bytes\n", recv_size);
    }
}
__inline static
void OnSocks4()
{
    int ret = 0;
    int send_size = 0;
    int recv_size = 0;
    std::string m_sError;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in nameSockAddr;
    int nameSockAddrSize = sizeof(nameSockAddr);
    nameSockAddr.sin_family = AF_INET;
    nameSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    nameSockAddr.sin_port = htons(1080);
    ret = connect(clientSocket, (const sockaddr *)&nameSockAddr, nameSockAddrSize);

    //Socks4代理服务器端口及地址
    if(ret != 0)
    {
        m_sError = ("不能连接到Socks4代理服务器!");
        closesocket(clientSocket);
        MessageBox(NULL, m_sError.c_str(), "提示", MB_OK);
        return;
    }

    struct sock4req1* m_proxyreq = (struct sock4req1*)DriveCache;
    m_proxyreq->VN = 0x4;
    m_proxyreq->CD = 0x1;
    m_proxyreq->Port = ntohs(21);
    m_proxyreq->IPAddr = inet_addr("219.201.39.50");
    memset(m_proxyreq->other, 0, sizeof(m_proxyreq->other));

    send_size = sizeof(struct sock4req1);
    recv_size = DRIVE_CACHE_BYTES;
    send_and_recv(clientSocket, DriveCache, send_size, DriveCache, recv_size);

    struct sock4ans1* m_proxyans = (struct sock4ans1*)DriveCache;
    if (m_proxyans->VN != 0 || m_proxyans->CD != 90)
    {
        m_sError = ("通过代理连接主站不成功!");
        closesocket(clientSocket);
        MessageBox(NULL, m_sError.c_str(), "提示", MB_OK);
        return;
    }

    //连接已经建立,发送及接收数据    
    m_sError = ("通过代理连接主站成功!连接已经建立,发送及接收数据.");
    MessageBox(NULL, m_sError.c_str(), "提示", MB_OK);

    const char* sHelloData = "Hello!";
    send_size = strlen(sHelloData);
    memcpy(DriveCache, sHelloData, send_size);
    recv_size = DRIVE_CACHE_BYTES;
    send_and_recv(clientSocket, DriveCache, send_size, DriveCache, recv_size);

    MessageBox(NULL, DriveCache, "提示", MB_OK);
    closesocket(clientSocket);
}

__inline static
void OnSocks5()
{
    // TODO: Add your control notification handler code here
    //http://my.nbip.net/homepage/nblulei/ttdl/sockdllb.htm

    uint16_t uSocks5ProxyPort = 1080;
    const char* sSocks5ProxyName = "127.0.0.1";
    std::vector<std::string> svDocks5ProxyAddr;
    domain_to_ipaddr(svDocks5ProxyAddr, sSocks5ProxyName);

    uint16_t uDstPort = 443;
    const char* sDstName = "www.facebook.com";
    std::vector<std::string> svDstAddr;
    domain_to_ipaddr(svDstAddr, sDstName);

    int ret = 0;
    int send_size = 0;
    int recv_size = 0;
    std::string m_sError;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in nameSockAddr;
    int nameSockAddrSize = sizeof(nameSockAddr);
    nameSockAddr.sin_family = AF_INET;
    nameSockAddr.sin_addr.s_addr = inet_addr(svDocks5ProxyAddr.begin()->c_str());
    nameSockAddr.sin_port = htons(uSocks5ProxyPort);
    ret = connect(clientSocket, (const sockaddr *)&nameSockAddr, nameSockAddrSize);

    //Socks5代理服务器端口及地址
    if (ret != 0)
    {
        m_sError = ("不能连接到Socks5代理服务器!");
        closesocket(clientSocket);
        MessageBox(NULL, m_sError.c_str(), "提示", MB_OK);
        return;
    }

    //socks5协议部分（1.发送数据至服务器）
    //|--------|-------------|-------------|  
    //|  VER   |   NMETHOD   |   METHODS   |  
    //|--------|-------------|-------------|
    //|   1	   |	  1      |    1-255    |
    //|--------|-------------|-------------|
    //	VER是SOCKS版本，这里应该是0x05.
    //	NMETHODS是METHODS部分的长度.
    //  METHODS是客户端支持的认证方式列表，每个方法占1字节。当前的定义是： 
    //		0x00 不需要认证
    //		0x01 GSSAPI
    //		0x02 用户名、密码认证
    //		0x03 - 0x7F由IANA分配（保留）
    //		0x80 - 0xFE为私人方法保留
    //		0xFF 无可接受的方法
    struct sock5req1* m_proxyreq1 = (struct sock5req1*)DriveCache;
    m_proxyreq1->Ver = 0x05;
    m_proxyreq1->nMethods = 0x02;
    m_proxyreq1->Methods[0] = 0x00;
    m_proxyreq1->Methods[1] = 0x02;

    send_size = sizeof(m_proxyreq1->Ver) + sizeof(m_proxyreq1->nMethods) + m_proxyreq1->nMethods * sizeof(*m_proxyreq1->Methods);
    recv_size = DRIVE_CACHE_BYTES;
    send_and_recv(clientSocket, DriveCache, send_size, DriveCache, recv_size);

    struct sock5ans1* m_proxyans1 = (struct sock5ans1*)DriveCache;
    if (m_proxyans1->Ver != 0x05 || (m_proxyans1->Method != 0x00 && m_proxyans1->Method != 0x02))
    {
        m_sError = ("通过代理连接主站不成功!");
        closesocket(clientSocket);
        MessageBox(NULL, m_sError.c_str(), "提示", MB_OK);
        return;
    }
    if (m_proxyans1->Method == 0x02)
    {
        const char* sUser = "alon";
        const char* sPass = "alon";
        int nUserLen = strlen(sUser);
        int nPassLen = strlen(sPass);
        //socks5协议部分（3.发送账号密码数据至服务器认证）
        //|--------|-------------|-------------|--------|-------| 
        //|  CVER  |  UserNameL  |   UserName  |  PWDL  |  PWD  |
        //|--------|-------------|-------------|--------|-------|
        //|   1	   |	  1      |     动态    |    1   |  动态 |
        //|--------|-------------|-------------|--------|-------|
        //  CVER是鉴定协议版本，这里应该是0x01.
        //	UserNameL是账户名的长度，大小为1位
        //  UserName是账户名本身 
        //	PWDL是密码的长度，大小为1位
        //	PWDL是PWD本身
        struct authreq* m_authreq = (struct authreq*)DriveCache;
        m_authreq->Ver = 0x1;
        m_authreq->Ulen = nUserLen;
        memcpy(m_authreq->Name, sUser, nUserLen);
        m_authreq->PLen = nPassLen;
        memcpy(m_authreq->Pass, sPass, nPassLen);

        send_size = sizeof(struct authreq);
        recv_size = DRIVE_CACHE_BYTES;
        send_and_recv(clientSocket, DriveCache, send_size, DriveCache, recv_size);
        //socks5协议部分（4.服务器响应数据）
        //|--------|------------|
        //|  CVER  |   Status   |
        //|--------|------------|
        //|   1	   |	 1      |
        //|--------|------------|
        // CVER是确认鉴定版本号
        // Status为状态码：
        //		1.0x00表示成功
        //		1.0x01表示失败
        struct authans* m_authans = (struct authans*)DriveCache;
        if (m_authans->Ver != 0x01 || m_authans->Status != 0x00)
        {
            m_sError = ("代理服务器用户验证不成功!");
            closesocket(clientSocket);
            MessageBox(NULL, m_sError.c_str(), "提示", MB_OK);
            return;
        }
    }

    //socks5协议部分（5.客户端认证成功后开始进行请求）
    //|-------|-------|-------|--------|-----------|------------| 
    //|  VER  |  CMD  |  RSV  |  ATYP  |  DST.ADDR |  DST.ADDR  |
    //|-------|-------|-------|--------|-----------|------------|
    //|   1	  |	  1   |  0x00 |    1   |   Active  |      2     |
    //|-------|-------|-------|--------|-----------|------------|
    //  VER.是SOCKS协议版本，这里应该是0x05.
    //	CMD.是SOCKS的命令码:
    //		0x01:表示CONNECT请求
    //		0x02:表示BIND请求
    //		0x03:表示UDP转发
    //  RSV.0x00:保留，无实际作用
    //  ATYP.DST.ADDR类型:
    //		0x01:表示IPV4地址
    //		0x03:表示域名格式
    //		0x04:表示IPV6地址
    //  DST.ADDR.目的地址
    //		当ATYP=0x01 DST.ADDR部分为四字节长度，内容为IP本身
    //		当ATYP=0x03 第一个部分为一个1字节表示域名长度，第二部分就是剩余内容为具体域名。Active表示长度不定。没有\0作为结尾
    //	DST.PORT 网络字节序表示的目的端口
    struct sock5req2* m_proxyreq2 = (struct sock5req2*)DriveCache;
    m_proxyreq2->Ver = 0x05;
    m_proxyreq2->Cmd = 0x01;
    m_proxyreq2->Rsv = 0x00;
    m_proxyreq2->Atyp = 0x01;
    m_proxyreq2->IPAddr = inet_addr(svDstAddr.begin()->c_str());
    m_proxyreq2->Port = ntohs(uDstPort);

    send_size = sizeof(struct sock5req2);
    recv_size = DRIVE_CACHE_BYTES;
    send_and_recv(clientSocket, DriveCache, send_size, DriveCache, recv_size);

    //socks5协议部分（6.代理服务器->客户端，确认连接）
    //|-------|-------|-------|--------|-----------|------------| 
    //|  VER  |  RCV  |  RSV  |  ATYP  |  DST.ADDR |  DST.ADDR  |
    //|-------|-------|-------|--------|-----------|------------|
    //|   1	  |	  1   |  0x00 |    1   |   Active  |      2     |
    //|-------|-------|-------|--------|-----------|------------|
    //  VER.返回必须为0x05.
    //	RCV.返回的状态码:
    //		0x00:succeeded
    //		0x01:general SOCKS server failure
    //		0x02:connection not allowed by ruleset
    //		0x03:Network unreachable
    //		0x04:Host unreachable
    //		0x05:Connection refused
    //		0x06:TTL expired
    //		0x07:Command not supported
    //		0x08: Address type not supported
    //		0x09:to X’FF’ unassigned
    //  RSV.0x00:保留，无实际作用
    //  ATYP.仅用于响应BIND命令:
    //		0x01:表示IPV4地址
    //		0x03:表示域名格式
    //		0x04:表示IPV6地址
    //  DST.ADDR.目的地址，仅用于响应BIND命令
    //		当ATYP=0x01 DST.ADDR部分为四字节长度，内容为IP本身
    //		当ATYP=0x03 第一个部分为一个1字节表示域名长度，第二部分就是剩余内容为具体域名。Active表示长度不定。没有\0作为结尾
    //	DST.PORT 网络字节序表示的目的端口，仅用于响应客户端BIND命令

    struct sock5ans2* m_proxyans2 = (struct sock5ans2*)DriveCache;

    if (m_proxyans2->Ver != 0x05 || m_proxyans2->Rep != 0x00)
    {
        m_sError = ("通过代理连接主站不成功!");
        closesocket(clientSocket);
        MessageBox(NULL, m_sError.c_str(), "提示", MB_OK);
        return;
    }

    //连接已经建立,发送及接收数据
    m_sError = ("通过代理连接主站成功!连接已经建立,发送及接收数据.");
    MessageBox(NULL, m_sError.c_str(), "提示", MB_OK);

    const char* helloData = "Hello!";
    send_size = strlen(helloData);
    memcpy(DriveCache, helloData, send_size);
    {
        std::string req = "GET / HTTP/1.1\r\nHost: " + std::string(sDstName) + "\r\n\r\n";
        send_size = req.size(); 
        memcpy(DriveCache, req.data(), send_size);
    }
    recv_size = DRIVE_CACHE_BYTES;
    send_and_recv(clientSocket, DriveCache, send_size, DriveCache, recv_size);

    MessageBox(NULL, DriveCache, "提示", MB_OK);
    closesocket(clientSocket);
}

__inline static
void OnHttp()
{
    // TODO: Add your control notification handler code here
    int ret = 0;
    int send_size = 0;
    int recv_size = 0;
    std::string m_sError;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in nameSockAddr;
    int nameSockAddrSize = sizeof(nameSockAddr);
    nameSockAddr.sin_family = AF_INET;
    nameSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    nameSockAddr.sin_port = htons(3128);
    ret = connect(clientSocket, (const sockaddr *)&nameSockAddr, nameSockAddrSize);

    if (ret != 0)
    {
        m_sError = ("不能连接到代理服务器!");
        closesocket(clientSocket);
        MessageBox(NULL, m_sError.c_str(), "提示", MB_OK);
        return;
    }

    //    sprintf( buff, "%s%s:%d%s","CONNECT ","219.201.39.50",21," HTTP/1.1/r/nUser-Agent: MyApp/0.1/r/n/r/n");
    send_size = sprintf(DriveCache, "%s%s:%d%s", "CONNECT ", "219.201.39.50", 21, " HTTP/1.1/r/nUser-Agent: CERN-LineMode/2.15 libwww/2.17b3/r/n/r/n");
    //    sprintf( buff, "%s%s:%d%s","CONNECT ","www.tom.com",80," HTTP/1.1/r/nUser-Agent: CERN-LineMode/2.15 libwww/2.17b3/r/n/r/n");
    //    sprintf( buff, "%s%s","GET ","HTTP://WWW.TOM.COM HTTP/1.1/r/n/r/n");
   
    recv_size = DRIVE_CACHE_BYTES;
    send_and_recv(clientSocket, DriveCache, send_size, DriveCache, recv_size);

    if (strstr(DriveCache, "HTTP/1.0 200 Connection established") == NULL) //连接不成功
    {
        m_sError = ("通过代理连接主站不成功!");
        closesocket(clientSocket);
        MessageBox(NULL, m_sError.c_str(), "提示", MB_OK);
        return;
    }

    /*
    if( strstr(buff, "Error 403") )
    {
        //return GoError(PROXYERROR_PROXYDISABLE);  //代理服务器拒绝请求
    }

    if( strstr(buff, "407 Proxy authentication required") )  //需要身份验证
    {
        //return GoError(PROXYERROR_USERNAMEORPASSWORD);  //用户身份校检不成功
    }
    if( strstr(buff, "Connection refuesed") )
    {
        //return GoError(PROXYERROR_CONNECTHOSTSERVER);  //通过代理连接主站不成功
    }
    if( strstr(buff, "Access Denied") )
    {
        //return GoError(PROXYERROR_USERNAMEORPASSWORD);  //用户身份校检不成功
    }
    if(strstr(buff, "Connection refused by Remote Host") )
    {
        //return GoError(PROXYERROR_CONNECTHOSTSERVER);  //通过代理连接主站不成功
    }

    ClientSock.Close();
*/
    m_sError = ("通过代理连接主站成功!连接已经建立,发送及接收数据.");
    MessageBox(NULL, m_sError.c_str(), "提示", MB_OK);

    //连接已经建立,发送及接收数据
    memset(DriveCache, 0, DRIVE_CACHE_BYTES);
    const char* helloData = "Hello!";
    send_size = strlen(helloData);
    memcpy(DriveCache, helloData, send_size);

    recv_size = DRIVE_CACHE_BYTES;
    send_and_recv(clientSocket, DriveCache, send_size, DriveCache, recv_size);

    MessageBox(NULL, DriveCache, "提示", MB_OK);
    closesocket(clientSocket);
}