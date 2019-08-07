/*!
 * @file TCP.h
 * @brief ����SOCKETͨ�ŵ�TCP/IPͨ��
 * @author FanKaiyu
 * @date 2018-03-24
 * @version 2.1
*/

#pragma once
#ifndef _TCP_H
#define _TCP_H

#ifdef _WIN32
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <WinDNS.h>

#pragma comment(lib, "WS2_32")
#else
#include <netdb.h>
#endif
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <set>

#ifndef TM_TCPCLIENT_RECEIVE
#define TM_TCPCLIENT_RECEIVE WM_USER+1	/*!< �ͻ������ݽ������ݵ��߳���Ϣ */
#endif //!TM_TCPCLIENT_RECEIVE

#ifndef TM_TCPSERVER_ACCEPT
#define TM_TCPSERVER_ACCEPT WM_USER+2	/*!< ����˽��տͻ�������������߳���Ϣ*/
#endif //!TM_TCPSERVER_ACCEPT

#ifdef _WIN64
#ifndef ULL
#define ULL unsigned long long
#endif //!ULL
#ifndef GetTickCountEX
#define GetTickCountEX() GetTickCount64()
#endif //!GetTickCountEX()
#else
#ifndef ULL
#define ULL unsigned long
#endif //!ULL
#ifndef GetTickCountEX
#define GetTickCountEX() GetTickCount()
#endif//!GetTickCountEX()
#endif//_WIN64

namespace TCP
{
	/*!
	 * @brief ��ȡԶ��IP��ַ�Ͷ˿ں�
	 * @param const SOCKET socketͨ��������
	 * @param char* IP��ַ
	 * @param unsigned short& �˿�
	 * @return bool ��ȡ�ɹ�����true,���򷵻�false
	 */
	static bool GetPeerIPandPort(const SOCKET socket, char* lpszIP, unsigned short &usPort)
	{
		if (socket == INVALID_SOCKET)
		{
			return false;
		}

		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);

		//�����׽��ֻ�ȡ��ַ��Ϣ
		if (::getpeername(socket, (SOCKADDR*)&addrClient, &addrClientlen) != 0)
		{
			return false;
		}

		if (inet_ntop(AF_INET, (void*)&addrClient.sin_addr, lpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return false;
		}

		usPort = htons(addrClient.sin_port);

		return true;
	}

	/*!
	 * @brief ��ȡԶ��IP��ַ�Ͷ˿ں�
	 * @param const SOCKET socketͨ��������
	 * @param wchar_t* IP��ַ
	 * @param unsigned short& �˿�
	 * @return bool ��ȡ�ɹ�����true,���򷵻�false
	 */
	static bool GetPeerIPandPort(const SOCKET socket, wchar_t* wlpszIP, unsigned short &usPort)
	{
		if (socket == INVALID_SOCKET)
		{
			return false;
		}

		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);

		//�����׽��ֻ�ȡ��ַ��Ϣ
		if (::getpeername(socket, (SOCKADDR*)&addrClient, &addrClientlen) != 0)
		{
			return false;
		}

		if (InetNtopW(AF_INET, (void*)&addrClient.sin_addr, wlpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return false;
		}

		usPort = htons(addrClient.sin_port);

		return true;
	}

	/*!
	 * @brief ��ȡԶ��IP��ַ�Ͷ˿ں�
	 * @param const SOCKET socketͨ��������
	 * @param string& IP��ַ
	 * @param unsigned short& �˿�
	 * @return bool ��ȡ�ɹ�����true,���򷵻�false
	 */
	static bool GetPeerIPandPort(const SOCKET socket, std::string &strIP, unsigned short &usPort)
	{
		char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		if (GetPeerIPandPort(socket, lpszIP, usPort) == false)
		{
			return false;
		}

		strIP = lpszIP;

		return true;
	}

	/*!
	 * @brief ��ȡԶ��IP��ַ�Ͷ˿ں�
	 * @param const SOCKET socketͨ��������
	 * @param wstring& IP��ַ
	 * @param unsigned short& �˿�
	 * @return bool ��ȡ�ɹ�����true,���򷵻�false
	 */
	static bool GetPeerIPandPort(const SOCKET socket, std::wstring &wstrIP, unsigned short &usPort)
	{
		wchar_t wlpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		if (GetPeerIPandPort(socket, wlpszIP, usPort) == false)
		{
			return false;
		}

		wstrIP = wlpszIP;

		return true;
	}

	/*!
	 * @brief ��ȡ����IP��ַ�Ͷ˿ں�
	 * @param const SOCKET socketͨ��������
	 * @param char* IP��ַ
	 * @param unsigned short& �˿�
	 * @return bool ��ȡ�ɹ�����true,���򷵻�false
	*/
	static bool GetSockIPandPort(const SOCKET socket, char* lpszIP, unsigned short &usPort)
	{
		if (socket == INVALID_SOCKET)
		{
			return false;
		}

		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);

		//�����׽��ֻ�ȡ��ַ��Ϣ
		if (::getsockname(socket, (SOCKADDR*)&addrClient, &addrClientlen) != 0)
		{
			return false;
		}

		if (inet_ntop(AF_INET, (void*)&addrClient.sin_addr, lpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return false;
		}

		usPort = htons(addrClient.sin_port);

		return true;
	}

	/*!
	 * @brief ��ȡ����IP��ַ�Ͷ˿ں�
	 * @param const SOCKET socketͨ��������
	 * @param wchar_t* IP��ַ
	 * @param unsigned short& �˿�
	 * @return bool ��ȡ�ɹ�����true,���򷵻�false
	*/
	static bool GetSockIPandPort(const SOCKET socket, wchar_t* wlpszIP, unsigned short &usPort)
	{
		if (socket == INVALID_SOCKET)
		{
			return false;
		}

		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);

		//�����׽��ֻ�ȡ��ַ��Ϣ
		if (::getsockname(socket, (SOCKADDR*)&addrClient, &addrClientlen) != 0)
		{
			return false;
		}

		if (InetNtopW(AF_INET, (void*)&addrClient.sin_addr, wlpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return false;
		}

		usPort = htons(addrClient.sin_port);

		return true;
	}

	/*!
	 * @brief ��ȡ����IP��ַ�Ͷ˿ں�
	 * @param const SOCKET socketͨ��������
	 * @param string& IP��ַ
	 * @param unsigned short& �˿�
	 * @return bool ��ȡ�ɹ�����true,���򷵻�false
	*/
	static bool GetSockIPandPort(const SOCKET socket, std::string &strIP, unsigned short &usPort)
	{
		char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		if (GetSockIPandPort(socket, lpszIP, usPort) == false)
		{
			return false;
		}

		strIP = lpszIP;

		return true;
	}

	/*!
	 * @brief ��ȡ����IP��ַ�Ͷ˿ں�
	 * @param const SOCKET socketͨ��������
	 * @param wstring& IP��ַ
	 * @param unsigned short& �˿�
	 * @return bool ��ȡ�ɹ�����true,���򷵻�false
	*/
	static bool GetSockIPandPort(const SOCKET socket, std::wstring &wstrIP, unsigned short &usPort)
	{
		wchar_t wlpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		if (GetSockIPandPort(socket, wlpszIP, usPort) == false)
		{
			return false;
		}

		wstrIP = wlpszIP;

		return true;
	}

	/*!
	 * @brief ��ȡ���������IP
	 * @param char* ����IP��ַ�ַ���
	 * @param int �ƶ��˿ں� ֵΪ-1��ָ��
	 * @return bool �ɹ�����true,ʧ�ܷ���false
	*/
	static bool GetLocalIP(char* lpszIP, int nPort = -1)
	{
		//1.��ʼ��wsa  
		WSADATA wsaData;
		int ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (ret != 0)
		{
			return false;
		}

		//2.��ȡ������  
		char hostname[256];
		ret = ::gethostname(hostname, sizeof(hostname));
		if (ret == SOCKET_ERROR)
		{
			return false;
		}

		//3.��ȡ����ip  
		struct addrinfo hints;
		struct addrinfo *res, *cur;
		struct sockaddr_in *addr;
		// char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;		/* Allow IPv4 */
		hints.ai_flags = AI_PASSIVE;	/* For wildcard IP address */
		hints.ai_protocol = 0;			/* Any protocol */
		hints.ai_socktype = SOCK_STREAM;

		char lpszPort[6];
		if (nPort > -1)
		{
			sprintf_s(lpszPort, 6, "%d", nPort);
			getaddrinfo(hostname, lpszPort, &hints, &res);
		}
		else
		{
			getaddrinfo(hostname, NULL, &hints, &res);
		}

		if (ret == -1)
		{
			perror("getaddrinfo");
			return false;
		}

		for (cur = res; cur != NULL; cur = cur->ai_next)
		{
			addr = (struct sockaddr_in *)cur->ai_addr;

			sprintf_s(lpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH, "%d.%d.%d.%d",
				(*addr).sin_addr.S_un.S_un_b.s_b1,
				(*addr).sin_addr.S_un.S_un_b.s_b2,
				(*addr).sin_addr.S_un.S_un_b.s_b3,
				(*addr).sin_addr.S_un.S_un_b.s_b4);
		}

		freeaddrinfo(res);

		WSACleanup();

		return true;
	}

	/*!
	 * @brief ��ȡ���������IP
	 * @param char* ����IP��ַ�ַ���
	 * @param int ָ���˿ں� ֵΪ-1��ָ��
	 * @return bool �ɹ�����true,ʧ�ܷ���false
	*/
	static bool GetLocalIP(wchar_t* wlpszIP, int nPort = -1)
	{
		//1.��ʼ��wsa  
		WSADATA wsaData;
		int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (ret != 0)
		{
			return false;
		}

		//2.��ȡ������  
		char hostname[256];
		ret = gethostname(hostname, sizeof(hostname));
		if (ret == SOCKET_ERROR)
		{
			return false;
		}

		//3.��ȡ����ip  
		struct addrinfo hints;
		struct addrinfo *res, *cur;
		struct sockaddr_in *addr;
		// char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;		/* Allow IPv4 */
		hints.ai_flags = AI_PASSIVE;	/* For wildcard IP address */
		hints.ai_protocol = 0;			/* Any protocol */
		hints.ai_socktype = SOCK_STREAM;

		char lpszPort[6];
		if (nPort > -1)
		{
			sprintf_s(lpszPort, 6, "%d", nPort);
			getaddrinfo(hostname, lpszPort, &hints, &res);
		}
		else
		{
			getaddrinfo(hostname, NULL, &hints, &res);
		}

		if (ret == -1)
		{
			perror("getaddrinfo");
			return false;
		}

		for (cur = res; cur != NULL; cur = cur->ai_next)
		{
			addr = (struct sockaddr_in *)cur->ai_addr;

			swprintf_s(wlpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH, L"%d.%d.%d.%d",
				(*addr).sin_addr.S_un.S_un_b.s_b1,
				(*addr).sin_addr.S_un.S_un_b.s_b2,
				(*addr).sin_addr.S_un.S_un_b.s_b3,
				(*addr).sin_addr.S_un.S_un_b.s_b4);
		}

		freeaddrinfo(res);

		WSACleanup();

		return true;
	}

	/*!
	 * @brief ��ȡ���������IP
	 * @param string& ����IP��ַ�ַ���
	 * @param int ָ���˿ں� ֵΪ-1��ָ��
	 * @return bool �ɹ�����true,ʧ�ܷ���false
	*/
	static bool GetLocalIP(std::string& strIP, int nPort = -1)
	{
		char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];
		if (GetLocalIP(lpszIP, nPort) == false)
		{
			return false;
		}

		strIP = lpszIP;

		return true;
	}

	/*!
	 * @brief ��ȡ���������IP
	 * @param wstring& ����IP��ַ�ַ���
	 * @param int ָ���˿ں� ֵΪ-1��ָ��
	 * @return bool �ɹ�����true,ʧ�ܷ���false
	*/
	static bool GetLocalIP(std::wstring& wstrIP, int nPort = -1)
	{
		wchar_t wlpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];
		if (GetLocalIP(wlpszIP, nPort) == false)
		{
			return false;
		}

		wstrIP = wlpszIP;

		return true;
	}

	static const std::string DWORDToIPAddress(const DWORD dwIP)
	{
		SOCKADDR_IN addr;
		addr.sin_addr.s_addr = htonl(dwIP);

		char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];
		if (inet_ntop(AF_INET, (void*)& addr.sin_addr, lpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return "";
		}

		return lpszIP;
	}

	static const std::wstring DWORDToIPAddressW(const DWORD dwIP)
	{
		SOCKADDR_IN addr;
		addr.sin_addr.s_addr = htonl(dwIP);

		wchar_t wlpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];
		if (InetNtopW(AF_INET, (void*)& addr.sin_addr, wlpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return L"";
		}

		return wlpszIP;
	}

	static const DWORD IPAddressToDWORD(const char* strIP)
	{
		SOCKADDR_IN addr;
		if (strcmp(strIP, "") == 0)
		{
			inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr.s_addr);
		}
		else
		{
			inet_pton(AF_INET, strIP, &addr.sin_addr.s_addr);
		}

		return htonl(addr.sin_addr.s_addr);
	}

	static const DWORD IPAddressToDWORD(const wchar_t* wstrIP)
	{
		SOCKADDR_IN addr;
		if (wcscmp(wstrIP, L"") == 0)
		{
			InetPtonW(AF_INET,L"0.0.0.0", &addr.sin_addr.s_addr);
		}
		else
		{
			InetPtonW(AF_INET, wstrIP, &addr.sin_addr.s_addr);
		}

		return htonl(addr.sin_addr.s_addr);
	}
};

typedef void(__stdcall* UpdateServer)(void*, void*,SOCKET);

/*!
 * @class SocketClient
 * @brief ����SOCKETͨ�ŵ�TCP�ͻ��˶���
*/
typedef class SocketClient
{
public:
	SocketClient();
	SocketClient(const char* strSrvIP, const char* strLocalIP,int nSrvPort = -1, int nLocalPort = -1);
	SocketClient(const wchar_t* wstrSrvIP, const wchar_t* wstrLocalIP, int nSrvPort = -1, int nLocalPort = -1);

	SocketClient(const std::string strLocalIP, std::string strSrvIP = "", int nLocalPort = -1, int nSrvPort = -1);
	~SocketClient();

protected:
	SOCKET m_socket;					/*!< �ͻ��˰�SOCKETͨ�� */
	std::string m_strLocalIP;			/*!< �ͻ��˰�IP��ַ ֵΪ��ʱ�����IP;ͨ�������ɹ�ǰ�趨�ı���IP��ַ */
	int m_nLocalPort;					/*!< �ͻ��˰󶨶˿� ֵΪ-1ʱ������˿�;ͨ�������ɹ�ǰ�趨�ı��ض˿� */
	std::string m_strSrvIP;				/*!< ���������IP��ַ ֵΪ��ʱ�����IP;ͨ�������ɹ�ǰ�趨��Զ��IP��ַ */
	int m_nSrvPort;						/*!< ��������Ӷ˿� ֵΪ-1ʱ������˿�;ͨ�������ɹ�ǰ�趨��Զ�̶˿� */
	std::string m_strSockIP;			/*!< �ͻ��˱���IP��ַ ֵΪ��ʱ�����IP;ͨ�������ɹ��󷵻ص�IP��ַ */
	unsigned short m_usSockPort;		/*!< �ͻ��˱��ض˿� ֵΪ-1ʱ������˿�;ͨ�������ɹ��󷵻صĶ˿� */
	unsigned long m_ulThreadID;			/*!< �߳�ID */
	ULL m_ullReceiveTime;				/*!< ���ݽ��յ�ʱ�䣨ϵͳ����ʱ�䣩 */
	ULL m_ullSendTime;					/*!< ���ݷ��͵�ʱ�䣨ϵͳ����ʱ�䣩 */
	SOCKET m_socketTmp;

private:
	std::thread* m_pReceiveThread;		/*!< ���ݽ����߳�ָ�� */

public:
	/*!
	 * @brief ���ӿͻ���
	 * @param const SOCKET �ɷ�����Accept���յĿͻ���SOCKETͨ��
	 * @param const unsigned long �߳�ID
	 * @return bool ���ӳɹ�����true,���򷵻�false
	*/
	bool ConnectPeer(const SOCKET socketClien, const unsigned long ulThreadID = 0);

	/*!
	 * @brief ���ӷ����
	 * @param const char* �����IP��ַ
	 * @param const unsigned short ����˶˿�
	 * @param const unsigned long �߳�ID
	 * @return bool ���ӳɹ�����true,���򷵻�false
	*/
	bool Connect(const unsigned long ulThreadID = 0);

	bool Connect(const char* strSrvIP, const unsigned short usSrvPort,const unsigned long ulThreadID = 0);
	bool Connect(const wchar_t* wstrSrvIP, const unsigned short usSrvPort,const unsigned long ulThreadID = 0);

	/*!
	 * @brief �ж�����
	*/
	void Break();

	/*!
	 * @brief ��������
	 * @param const unsigned char* ���ݿ�ָ��
	 * @param const unsigned int ���ݿ��С
	 * @return bool ���ͳɹ�����true,���򷵻�false
	*/
	bool Send(const unsigned char* pData,const unsigned int unSize);

	/*!
	 * @brief �Ƿ�����
	 * @return bool �����ӷ���true,���򷵻�false
	*/
	bool IsConnected();

	/*!
	 * @brief ��ȡIP��ַ�Լ��˿�
	 * @param string& �ͻ���IP��ַ
	 * @param int& �ͻ��˶˿�
	 * @return bool �����ӷ���true,δ���ӷ���false
	*/
	bool GetIPandPort(std::string& strIP,int& nPort);

	/*!
	 * @brief ��ȡIP��ַ�Լ��˿�
	 * @param wstring& �ͻ���IP��ַ
	 * @param int& �ͻ��˶˿�
	 * @return bool �����ӷ���true,δ���ӷ���false
	*/
	bool GetIPandPort(std::wstring& wstrIP, int& nPort);

	/*!
	 * @brief ����IP��ַ�Լ��˿�
	 * @param const char* �ͻ���IP��ַ
	 * @param const int �ͻ��˶˿�
	*/
	void SetLocalIPandPort(const char* strIP, int nPort = -1);

	/*!
	 * @brief ����IP��ַ�Լ��˿�
	 * @param const wchar_t* �ͻ���IP��ַ
	 * @param const int �ͻ��˶˿�
	*/
	void SetLocalIPandPort(const wchar_t* wstrIP, int nPort = -1);

	void SetServerIPandPort(const char* strIP, int nPort = -1);
	void SetServerIPandPort(const wchar_t* wstrIP, int nPort = -1);
protected:
	/*!
	 * @brief ���ӿͻ���
	 * @param const SOCKET SOCKETͨ��
	 * @param const unsigned long �߳�ID
	 * @return bool ���ӳɹ�����true,���򷵻�false
	*/
	bool Connect(const SOCKET socketClien, const unsigned long ulThreadID = 0);

	/*!
	 * @brief ���ݽ����߳�����
	 * @return bool �����ɹ�����true,���򷵻�false
	*/
	bool StartReceiveThread();

	/*!
	 * @brief ���ݽ����߳���ֹ
	*/
	void EndReceiveThread();

	/*!
	 * @brief ���ݽ����߳�
	*/
	void ReceiveThread();

	/*!
	 * @brief ���ݴ���
	 * @param const unsigned char* �����������
	 * @param const unsigned int ����������ݴ�С
	*/
	virtual void ProcessData(const unsigned char* pData,const unsigned int unSize) = 0;
} TCPClient;

/*!
 * @class SockerServer
 * @brief ����SOCKETͨ�ŵ�TCP����˶���
*/
typedef class SocketServer
{
public:
	SocketServer();
	SocketServer(const unsigned short usPort, const char* strIP, UpdateServer funcPrt = nullptr, void* ptr = nullptr);
	SocketServer(const unsigned short usPort, const wchar_t* wstrIP, UpdateServer funcPrt = nullptr, void* ptr = nullptr);
	SocketServer(const unsigned short usPort, std::string strIP = "", UpdateServer funcPrt = nullptr, void* ptr = nullptr);
	SocketServer(const unsigned short usPort, std::wstring wstrIP, UpdateServer funcPrt = nullptr, void* ptr = nullptr);
	~SocketServer();

protected:
	SOCKET m_socket;					/*!< �����SOCKETͨ�� */
	std::string m_strIP;				/*!< ����˰�IP��ַ */
	unsigned short m_usPort;			/*!< ����˰󶨶˿� */
	std::thread *m_pAcceptThread;		/*!< ���ݽ����߳�ָ�� */
	unsigned long m_ulThreadID;			/*!< �߳�ID */
	UpdateServer m_funcPrt;
	void* m_funcParamPrt;

public:
	/*!
	 * @brief ��������˼���
	 * @param unsigned long �߳�ID
	 * @return bool �����ɹ�����true,���򷵻�false
	*/
	bool StartListen(UpdateServer funcPrt = nullptr, void* ptr = nullptr,const unsigned long ulThreadID = 0);

	/*!
	 * @brief ��������˼���
	 * @param const unsigned short ����˶˿�
	 * @param const char* ����˰�IP
	 * @param unsigned long �߳�ID
	 * @return bool �����ɹ�����true,���򷵻�false
	*/
	bool StartListen(const unsigned short usPort, const char* strIP, UpdateServer funcPrt = nullptr, void* ptr = nullptr, const unsigned long ulThreadID = 0);

	/*!
	 * @brief ��������˼���
	 * @param const unsigned short ����˶˿�
	 * @param const wchar_t* ����˰�IP
	 * @param unsigned long �߳�ID
	 * @return bool �����ɹ�����true,���򷵻�false
	*/
	bool StartListen(const unsigned short usPort, const wchar_t* wstrIP, UpdateServer funcPrt = nullptr, void* ptr = nullptr, const unsigned long ulThreadID = 0);

	/*!
	 * @brief ��������˼���
	 * @param const unsigned short ����˶˿�
	 * @param string ����˰�IP
	 * @param unsigned long �߳�ID
	 * @return bool �����ɹ�����true,���򷵻�false
	*/
	bool StartListen(const unsigned short usPort, std::string strIP = "", UpdateServer funcPrt = nullptr, void* ptr = nullptr, const unsigned long ulThreadID = 0);

	/*!
	 * @brief ��������˼���
	 * @param const unsigned short ����˶˿�
	 * @param wstring ����˰�IP
	 * @param unsigned long �߳�ID
	 * @return bool �����ɹ�����true,���򷵻�false
	*/
	bool StartListen(const unsigned short usPort, std::wstring wstrIP = L"", UpdateServer funcPrt = nullptr, void* ptr = nullptr, const unsigned long ulThreadID = 0);

	/*!
	 * @brief �رշ���˼���
	*/
	void CloseListen();

	/*!
	 * @brief �Ƿ��Ѽ���
	 * @return bool �Ѽ�������true,���򷵻�false
	*/
	bool IsListened();

	/*!
	 * @brief ��IP�Ͷ˿�
	 * @param unsigned short �˿�
	 * @param const char* IP��ַ
	*/
	void Bind(unsigned short usPort,const char* strIP);

	/*!
	 * @brief ��IP�Ͷ˿�
	 * @param unsigned short �˿�
	 * @param const wchar_t* IP��ַ
	*/
	void Bind(unsigned short usPort, const wchar_t* wstrIP);

	/*!
	 * @brief ��IP�Ͷ˿�
	 * @param unsigned short �˿�
	 * @param string IP��ַ
	*/
	void Bind(unsigned short usPort, std::string strIP = "");

	/*!
	 * @brief ��IP�Ͷ˿�
	 * @param unsigned short �˿�
	 * @param wstring IP��ַ
	*/
	void Bind(unsigned short usPort, std::wstring wstrIP);

	/*!
	 * @brief ��ȡIP��ַ�Ͷ˿�
	 * @param char* �����IP��ַ
	 * @param unsigned short& ����˶˿�
	*/
	void GetIPandPort(char* strIP,unsigned short& usPort) const;

	/*!
	 * @brief ��ȡIP��ַ�Ͷ˿�
	 * @param wchar_t* �����IP��ַ
	 * @param unsigned short& ����˶˿�
	*/
	void GetIPandPort(wchar_t* wstrIP, unsigned short& usPort) const;

	/*!
	 * @brief ��ȡIP��ַ�Ͷ˿�
	 * @param string& �����IP��ַ
	 * @param unsigned short& ����˶˿�
	*/
	void GetIPandPort(std::string& strIP, unsigned short& usPort) const;

	/*!
	 * @brief ��ȡIP��ַ�Ͷ˿�
	 * @param wstring& �����IP��ַ
	 * @param unsigned short& ����˶˿�
	*/
	void GetIPandPort(std::wstring& wstrIP, unsigned short& usPort) const;

protected:
	/*!
	 * @brief �����ͻ��˽����߳�
	 * @return bool �����ɹ�����true,���򷵻�false
	*/
	bool StartAcceptThread();

	/*!
	 * @brief �رտͻ��˽����߳�
	*/
	void EndAcceptThread();

	/*!
	 * @brief �ͻ��˽����߳�
	*/
	void AcceptThread();

	/*!
	 * @brief �ͻ��˽���
	 * @param const SOCKET �ͻ���SOCKETͨ��
	*/
	void AcceptClient(const SOCKET socket);

} TCPServer;

#endif //!_TCP_H