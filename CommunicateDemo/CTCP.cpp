#include "pch.h"
#include "CTCP.h"
#include "../String/DebugPrint.h"
#include "../String/StringEx.h"

#ifdef _TCP_H

SocketClient::SocketClient()
	:m_socket(INVALID_SOCKET)
	, m_strLocalIP("")
	, m_nLocalPort(-1)
	, m_strSrvIP("")
	, m_nSrvPort(-1)
	, m_strSockIP("")
	, m_usSockPort(0)
	, m_pReceiveThread(nullptr)
	, m_ulThreadID(0)
	, m_ullReceiveTime(0)
	, m_ullSendTime(0)
	, m_socketTmp(INVALID_SOCKET)
{
	
}

SocketClient::SocketClient(const char* strSrvIP, const char* strLocalIP, const int nSrvPort, const int nLocalPort)
	:m_socket(INVALID_SOCKET)
	, m_strSockIP("")
	, m_usSockPort(0)
	, m_pReceiveThread(nullptr)
	, m_ulThreadID(0)
	, m_ullReceiveTime(0)
	, m_ullSendTime(0)
	, m_socketTmp(INVALID_SOCKET)
{
	m_strLocalIP = strLocalIP;
	m_nLocalPort = nLocalPort;
	m_strSrvIP = strSrvIP;
	m_nSrvPort = nSrvPort;
}

SocketClient::SocketClient(const wchar_t* wstrSrvIP, const wchar_t* wstrLocalIP, int nSrvPort, int nLocalPort)
	:m_socket(INVALID_SOCKET)
	, m_strSockIP("")
	, m_usSockPort(0)
	, m_pReceiveThread(nullptr)
	, m_ulThreadID(0)
	, m_ullReceiveTime(0)
	, m_ullSendTime(0)
	, m_socketTmp(INVALID_SOCKET)
{
	m_strLocalIP = StringEx::wstring_to_string(wstrLocalIP);
	m_nLocalPort = nLocalPort;
	m_strSrvIP = StringEx::wstring_to_string(wstrSrvIP);
	m_nSrvPort = nSrvPort;
}

SocketClient::SocketClient(const string strLocalIP, string strSrvIP, int nLocalPort, int nSrvPort)
	:m_socket(INVALID_SOCKET)
	, m_strSockIP("")
	, m_usSockPort(0)
	, m_pReceiveThread(nullptr)
	, m_ulThreadID(0)
	, m_ullReceiveTime(0)
	, m_ullSendTime(0)
	, m_socketTmp(INVALID_SOCKET)
{
	m_strLocalIP = strLocalIP;
	m_nLocalPort = nLocalPort;
	m_strSrvIP = strSrvIP;
	m_nSrvPort = nSrvPort;
}

SocketClient::~SocketClient()
{
	Break();
}

bool SocketClient::ConnectPeer(const SOCKET socketClien, const unsigned long ulThreadID)
{
	// ��ЧSOCKETͨ��
	if (socketClien == INVALID_SOCKET)
	{
		return false;
	}

	std::string strClientIP = "";		/*!< �ͻ���IP */
	unsigned short usClientPort = 0;	/*!< �ͻ��˶˿� */

	// �޷���ȡ�ͻ���IP��˿�
	if (TCP::GetPeerIPandPort(socketClien, strClientIP, usClientPort) == false)
	{
		return false;
	}

	// �ް󶨵�IP��ַ
	if (m_strLocalIP == "")
	{
		return false;
	}

	// IP��˿ڲ�����
	if (m_strLocalIP != strClientIP || (m_nLocalPort != -1 && m_nLocalPort != usClientPort))
	{
		return false;
	}

	m_strSockIP = strClientIP;
	m_usSockPort = usClientPort;

	return Connect(socketClien, ulThreadID);
}

bool SocketClient::Connect(const SOCKET socketClien, const unsigned long ulThreadID)
{
	if (socketClien == INVALID_SOCKET)
	{
		return false;
	}

	// ͨ������ͬ
	if (m_socket != INVALID_SOCKET)
	{
		if (m_socket != socketClien)
		{
			Break();
		}
	}

	EndReceiveThread();

	m_socket = socketClien;
	m_ulThreadID = ulThreadID;

	if (StartReceiveThread() == false)
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;

		shutdown(m_socketTmp, SD_BOTH);
		closesocket(m_socketTmp);
		m_socketTmp = INVALID_SOCKET;

		Break();

		return false;
	}

	std::string strSockIP = "", strPeerClient = "";	/*!< IP��ַ */
	unsigned short usSockPort = 0, usPeerPort = 0;	/*!< �˿� */

	TCP::GetSockIPandPort(m_socket, strSockIP, usSockPort);
	TCP::GetPeerIPandPort(m_socket, strPeerClient, usPeerPort);

	DebugPrint::Printf("���ص�ַ%s:%d����Զ�̵�ַ%s:%d�ɹ�", strSockIP.c_str(), usSockPort, strPeerClient.c_str(), usPeerPort);

	m_socketTmp = INVALID_SOCKET;

	m_ullReceiveTime = 0;
	m_ullSendTime = 0;

	return true;
}

bool SocketClient::Connect(const unsigned long ulThreadID)
{
	if (m_socketTmp != INVALID_SOCKET)
	{
		return false;
	}

	// ��ʼ���׽���
	WORD sockVersion = MAKEWORD(2, 2);

	WSADATA datawsa;

	// WSAStartup�첽�׽�����������
	if (WSAStartup(sockVersion, &datawsa) != 0)
	{
		WSACleanup();
		return false;
	}

	// �����׽���
	m_socketTmp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // �ͻ���SOCKET������

	if (m_socketTmp == INVALID_SOCKET)
	{
		WSACleanup();
		return false;
	}

	SOCKADDR_IN addrLocal; // ���ص�ַ
	addrLocal.sin_family = AF_INET;

	// �󶨶˿�
	if (m_nLocalPort != -1)
	{
		addrLocal.sin_port = htons(m_nLocalPort);
	}
	else
	{
		// ��portָ��Ϊ0,�����bindʱ��ϵͳ��Ϊ��ָ��һ�����õĶ˿ں�
		addrLocal.sin_port = 0;
	}

	// ��IP
	if (m_strLocalIP == "")
	{
		addrLocal.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		inet_pton(AF_INET, m_strLocalIP.c_str(), &addrLocal.sin_addr.s_addr);
	}
	
	//���׽��� 
	int retVal; // ����ֵ
	retVal = ::bind(m_socketTmp, (LPSOCKADDR)&addrLocal, sizeof(SOCKADDR_IN));

	if (SOCKET_ERROR == retVal)
	{
		//�ر��׽���
		shutdown(m_socketTmp, SD_BOTH);
		closesocket(m_socketTmp);
		m_socketTmp = INVALID_SOCKET;

		//�ͷ��׽�����Դ;
		WSACleanup();

		return false;
	}

	// ��ʼ��sockaddr_in�ṹ��
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(m_nSrvPort);
	// serAddr.sin_addr.S_un.S_addr = inet_addr(strIP.c_str());

	inet_pton(AF_INET, m_strSrvIP.c_str(), &serAddr.sin_addr.s_addr);

	// ���ӷ�����
	if (connect(m_socketTmp, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		WSACleanup();

		shutdown(m_socketTmp, SD_BOTH);
		closesocket(m_socketTmp);
		m_socketTmp = INVALID_SOCKET;

		return false;
	}

	// �޷���ȡ����IP��˿�
	if (TCP::GetPeerIPandPort(m_socketTmp, m_strSockIP, m_usSockPort) == false)
	{
		WSACleanup();

		shutdown(m_socketTmp, SD_BOTH);
		closesocket(m_socketTmp);
		m_socketTmp = INVALID_SOCKET;

		return false;
	}
	
	return Connect(m_socketTmp, ulThreadID);
}

bool SocketClient::Connect(const char* strSrvIP, const unsigned short usSrvPort, const unsigned long ulThreadID)
{
	m_strSrvIP = strSrvIP;
	m_nSrvPort = usSrvPort;

	return Connect(ulThreadID);
}

bool SocketClient::Connect(const wchar_t* wstrSrvIP, const unsigned short usSrvPort, const unsigned long ulThreadID)
{
	m_strSrvIP = StringEx::wstring_to_string(wstrSrvIP);
	m_nSrvPort = usSrvPort;

	return Connect(ulThreadID);
}

void SocketClient::Break()
{
	EndReceiveThread();

	m_ullReceiveTime = 0;
	m_ullSendTime = 0;

	return;
}

bool SocketClient::Send(const unsigned char * pData, const unsigned int unSize)
{
	if (m_socket == INVALID_SOCKET)
	{
		return false;
	}

	int nIndex = 0; /*!< �ɹ����͵��ֽ��� */

	nIndex = send(m_socket, (char*)pData, unSize, 0);

	if (nIndex == SOCKET_ERROR)
	{
		DebugPrint::Printf("�ͻ���%s:%d��������ʧ��", m_strSockIP.c_str(), m_usSockPort);

		return false;
	}

	m_ullSendTime = GetTickCountEX();

	/*
	// ��¼���͵�����
	stringstream ssHex; // 16��������
	for (int nI = 0; nI < nIndex; ++nI)
	{
		ssHex << StringEx::string_format("%02X ", (*(pData + nI) & 0xFF));
	}
	char* lpszStr = new char[nIndex + 1]; // ASCII�ַ�����
	memset(lpszStr, 0, nIndex + 1);
	memcpy_s(lpszStr, nIndex, pData, nIndex);

	DebugPrint::Printf("�ͻ���%s:%d�������ݳɹ�!\n16����:%s", m_strSockIP.c_str(), m_usSockPort, ssHex.str().c_str());
	DebugPrint::Printf("ASCII�ַ���:%s", lpszStr);

	delete[] lpszStr;
	*/

	return true;
}

bool SocketClient::IsConnected()
{
	if (m_socket == INVALID_SOCKET)
	{
		return false;
	}

	return true;
}

bool SocketClient::GetIPandPort(std::string & strIP, int & nPort)
{
	if (m_strSockIP != "")
	{
		strIP = m_strSockIP;
	}
	else
	{
		TCP::GetLocalIP(strIP, m_nLocalPort);
	}

	nPort = m_usSockPort;

	return IsConnected();
}

bool SocketClient::GetIPandPort(std::wstring & wstrIP, int & nPort)
{
	if (m_strSockIP != "")
	{
		wstrIP = StringEx::string_to_wstring(m_strSockIP);
	}
	else
	{
		TCP::GetLocalIP(wstrIP, m_nLocalPort);
	}

	nPort = m_usSockPort;

	return IsConnected();
}

void SocketClient::SetLocalIPandPort(const char * strIP, int nPort)
{
	if (IsConnected())
	{
		Break();
	}
	
	m_strLocalIP = strIP;
	m_nLocalPort = nPort;

	return;
}

void SocketClient::SetLocalIPandPort(const wchar_t * wstrIP, int nPort)
{
	if (IsConnected())
	{
		Break();
	}

	m_strSockIP = StringEx::wstring_to_string(wstrIP);
	m_nLocalPort = nPort;

	return;
}

void SocketClient::SetServerIPandPort(const char* strIP, int nPort)
{
	if (IsConnected())
	{
		Break();
	}

	m_strSrvIP = strIP;
	m_nSrvPort = nPort;

	return;
}

void SocketClient::SetServerIPandPort(const wchar_t* wstrIP, int nPort)
{
	if (IsConnected())
	{
		Break();
	}

	m_strSrvIP = StringEx::wstring_to_string(wstrIP);
	m_nSrvPort = nPort;

	return;
}

bool SocketClient::StartReceiveThread()
{
	m_pReceiveThread = new std::thread(&SocketClient::ReceiveThread, this);

	if (m_pReceiveThread)
	{
		return true;
	}

	return false;
}

void SocketClient::EndReceiveThread()
{
	if (m_pReceiveThread)
	{
		if (m_pReceiveThread->joinable())
		{
			if (m_socket != INVALID_SOCKET)
			{
				// �ر�SOCKET�׽���
				shutdown(m_socket, SD_BOTH);
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;

				DebugPrint::Printf("�ͻ���%s:%d�ر����ݽ����߳�", m_strSockIP.c_str(), m_usSockPort);
			}

			m_pReceiveThread->join();
		}

		delete m_pReceiveThread;
		m_pReceiveThread = nullptr;
	}
	
	return;
}

void SocketClient::ReceiveThread()
{
	char achDataBuffer[1500];		/*!< ���ݽ�����*/
	int nBufferLen = 0;				/*!< ���������ݳ���*/

	fd_set fds;

	DebugPrint::Printf("�ͻ���%s:%d���ݽ����߳̿���", m_strSockIP.c_str(), m_usSockPort);

	while (m_socket != INVALID_SOCKET)
	{
		nBufferLen = 0;

		// ��ʼ��������
		memset(achDataBuffer, 0, 1500);

		FD_ZERO(&fds);
		FD_SET(m_socket, &fds);

		switch (select((int)m_socket + 1, &fds, NULL, NULL, NULL))
		{
		case  SOCKET_ERROR:
		{
			if (errno == WSAEINTR)
			{
				continue;
			}

			shutdown(m_socket, SD_BOTH);
			m_socket = INVALID_SOCKET;

			DebugPrint::Printf("�ͻ���%s:%dͨ���쳣�ر�", m_strSockIP.c_str(), m_usSockPort);

			break;
		}
		case 0:
		{
			continue;
		}
		default:
		{
			if (FD_ISSET(m_socket, &fds))
			{
				// ��������
				nBufferLen = recv(m_socket, achDataBuffer, 1500, 0);

				if (SOCKET_ERROR == nBufferLen)
				{
					shutdown(m_socket, SD_BOTH);
					m_socket = INVALID_SOCKET;

					DebugPrint::Printf("�ͻ���%s:%d���ݽ����쳣��ͨ���ر�", m_strSockIP.c_str(), m_usSockPort);

					break;
				} // if (SOCKET_ERROR == nBufferLen)

				if (nBufferLen > 0)
				{
					/*
					// ��¼���յ�����
					stringstream ssHex; // 16��������
					for (int nI = 0; nI < nBufferLen; ++nI)
					{
						ssHex << StringEx::string_format("%02X ", (*(achDataBuffer + nI) & 0xFF));
					}
					char* lpszStr = new char[nBufferLen + 1]; // ASCII�ַ�����
					memset(lpszStr, 0, nBufferLen + 1);
					memcpy_s(lpszStr, nBufferLen, achDataBuffer, nBufferLen);

					DebugPrint::Printf("�ͻ���%s:%d�������ݳɹ�!\n16����:%s",m_strSockIP.c_str(), m_usSockPort, ssHex.str().c_str());
					DebugPrint::Printf("ASCII�ַ���:%s", lpszStr);

					delete[] lpszStr;
					*/

					m_ullReceiveTime = GetTickCountEX();

					ProcessData((unsigned char*)achDataBuffer, nBufferLen);
				} // if (nBufferLen > 0)
				else
				{
					shutdown(m_socket, SD_BOTH);
					m_socket = INVALID_SOCKET;

					DebugPrint::Printf("�ͻ���%s:%d���ݽ����쳣��ͨ���ر�", m_strSockIP.c_str(), m_usSockPort);

					break;
				}
			} // if (FD_ISSET(m_socket, &fds))
			break;
		}
		}

		Sleep(1);
	}

	DebugPrint::Printf("�ͻ���%s:%d���ݽ����߳̽���", m_strSockIP.c_str(), m_usSockPort);

	return;
}

SocketServer::SocketServer()
{
	m_socket = INVALID_SOCKET;
	m_strIP = "";
	m_usPort = 0;
	m_pAcceptThread = nullptr;
	m_ulThreadID = 0;
	m_funcPrt = nullptr;
	m_funcParamPrt = nullptr;
}

SocketServer::SocketServer(const unsigned short usPort, const char * strIP, UpdateServer funcPrt, void* ptr)
{
	m_socket = INVALID_SOCKET;
	m_strIP = strIP;
	m_usPort = usPort;
	m_pAcceptThread = nullptr;
	m_ulThreadID = 0;
	m_funcPrt = funcPrt;
	m_funcParamPrt = ptr;
}

SocketServer::SocketServer(const unsigned short usPort, const wchar_t * wstrIP, UpdateServer funcPrt, void* ptr)
{
	m_socket = INVALID_SOCKET;
	m_strIP = StringEx::wstring_to_string(wstrIP);
	m_usPort = usPort;
	m_pAcceptThread = nullptr;
	m_ulThreadID = 0;
	m_funcPrt = funcPrt;
	m_funcParamPrt = ptr;
}

SocketServer::SocketServer(const unsigned short usPort, std::string strIP, UpdateServer funcPrt, void* ptr)
{
	m_socket = INVALID_SOCKET;
	m_strIP = strIP;
	m_usPort = usPort;
	m_pAcceptThread = nullptr;
	m_ulThreadID = 0;
	m_funcPrt = funcPrt;
	m_funcParamPrt = ptr;
}

SocketServer::SocketServer(const unsigned short usPort, std::wstring wstrIP, UpdateServer funcPrt, void* ptr)
{
	m_socket = INVALID_SOCKET;
	m_strIP = StringEx::wstring_to_string(wstrIP);
	m_usPort = usPort;
	m_pAcceptThread = nullptr;
	m_ulThreadID = 0;
	m_funcPrt = funcPrt;
	m_funcParamPrt = ptr;
}

SocketServer::~SocketServer()
{
	CloseListen();
}

bool SocketServer::StartListen(UpdateServer funcPrt, void* ptr,const unsigned long ulThreadID)
{
	CloseListen();

	WSADATA wsd;

	SOCKADDR_IN addrServ; /*!< ��������ַ*/

	int retVal; /*!< ����ֵ*/

	// ��ʼ���׽��ֶ�̬��  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		return false;
	}

	//�����׽���  
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == m_socket)
	{
		//�ͷ��׽�����Դ
		WSACleanup();

		return false;
	}

	//������׽��ֵ�ַ   
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(m_usPort);
	if (m_strIP == "")
	{
		addrServ.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		// addrServ.sin_addr.s_addr = inet_addr(m_strIP.c_str());

		inet_pton(AF_INET, m_strIP.c_str(), &addrServ.sin_addr.s_addr);
	}

	//���׽���  
	retVal = ::bind(m_socket, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));

	if (SOCKET_ERROR == retVal)
	{
		//�ر��׽���
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;

		//�ͷ��׽�����Դ;
		WSACleanup();

		return false;
	}

	m_ulThreadID = ulThreadID;
	m_funcPrt = funcPrt;
	m_funcParamPrt = ptr;

	StartAcceptThread();

	return true;
}

bool SocketServer::StartListen(const unsigned short usPort,  const char * strIP, UpdateServer funcPrt, void* ptr, const unsigned long ulThreadID)
{
	m_usPort = usPort;
	m_strIP = strIP;

	return StartListen(funcPrt, ptr, ulThreadID);
}

bool SocketServer::StartListen(const unsigned short usPort,  const wchar_t * wstrIP, UpdateServer funcPrt, void* ptr, const unsigned long ulThreadID)
{
	m_usPort = usPort;
	m_socket = INVALID_SOCKET;

	m_strIP = StringEx::wstring_to_string(wstrIP);

	return StartListen(funcPrt, ptr, ulThreadID);
}

bool SocketServer::StartListen(const unsigned short usPort, std::string strIP, UpdateServer funcPrt, void* ptr, const unsigned long ulThreadID)
{
	return StartListen(usPort, strIP.c_str(), funcPrt, ptr, ulThreadID);
}

bool SocketServer::StartListen(const unsigned short usPort, std::wstring wstrIP, UpdateServer funcPrt, void* ptr, const unsigned long ulThreadID)
{
	return StartListen(usPort, wstrIP.c_str(), funcPrt, ptr,ulThreadID);
}

void SocketServer::CloseListen()
{
	m_funcPrt = nullptr;
	m_funcParamPrt = nullptr;

	EndAcceptThread();

	return;
}

bool SocketServer::IsListened()
{
	if (m_socket == INVALID_SOCKET)
	{
		return false;
	}

	if (m_pAcceptThread == nullptr)
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;

		return false;
	}

	if (m_pAcceptThread->joinable())
	{
		return true;
	}

	shutdown(m_socket, SD_BOTH);
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;

	return false;
}

void SocketServer::Bind(unsigned short usPort, const char * strIP)
{
	if (IsListened())
	{
		CloseListen();
	}

	m_usPort = usPort;
	m_strIP = strIP;

	return;
}

void SocketServer::Bind(unsigned short usPort, const wchar_t * wstrIP)
{
	if (IsListened())
	{
		CloseListen();
	}

	m_usPort = usPort;
	m_strIP = StringEx::wstring_to_string(wstrIP);

	return;
}

void SocketServer::Bind(unsigned short usPort, std::string strIP)
{
	Bind(usPort, strIP.c_str());

	return;
}

void SocketServer::Bind(unsigned short usPort, std::wstring wstrIP)
{
	Bind(usPort, wstrIP.c_str());

	return;
}

void SocketServer::GetIPandPort(char * strIP, unsigned short & usPort) const
{
	memset(strIP, 0, IP4_ADDRESS_STRING_BUFFER_LENGTH);

	if (m_strIP == "")
	{
		if (m_socket != INVALID_SOCKET)
		{
			TCP::GetLocalIP(strIP, m_usPort);
		}
		else
		{
			TCP::GetLocalIP(strIP, -1);
		}
	}
	else
	{
		memcpy_s(strIP, IP4_ADDRESS_STRING_BUFFER_LENGTH, m_strIP.c_str(), m_strIP.length());
	}

	usPort = m_usPort;

	return;
}

void SocketServer::GetIPandPort(wchar_t * wstrIP, unsigned short & usPort) const
{
	wmemset(wstrIP, 0, IP4_ADDRESS_STRING_BUFFER_LENGTH);

	if (m_strIP == "")
	{
		if (m_socket != INVALID_SOCKET)
		{
			TCP::GetLocalIP(wstrIP, m_usPort);
		}
		else
		{
			TCP::GetLocalIP(wstrIP, -1);
		}
	}
	else
	{
		wmemcpy_s(wstrIP, IP4_ADDRESS_STRING_BUFFER_LENGTH, StringEx::string_to_wstring(m_strIP).c_str(), StringEx::string_to_wstring(m_strIP).length());
	}

	usPort = m_usPort;

	return;
}

void SocketServer::GetIPandPort(std::string & strIP, unsigned short & usPort) const
{
	char strLocalIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

	if (m_strIP == "")
	{
		if (m_socket != INVALID_SOCKET)
		{
			TCP::GetLocalIP(strLocalIP, m_usPort);
		}
		else
		{
			TCP::GetLocalIP(strLocalIP, -1);
		}
	}
	else
	{
		strIP = m_strIP;
	}

	if (strcmp(strLocalIP, "") == 0)
	{
		strIP = m_strIP;
	}
	else
	{
		strIP = strLocalIP;
	}

	usPort = m_usPort;

	return;
}

void SocketServer::GetIPandPort(std::wstring & wstrIP, unsigned short & usPort) const
{
	if (m_socket != INVALID_SOCKET)
	{
		TCP::GetSockIPandPort(m_socket, wstrIP, usPort);

		return;
	}

	if (m_strIP == "")
	{
		char strLocalIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		if (TCP::GetLocalIP(strLocalIP, -1))
		{
			wstrIP = StringEx::string_to_wstring(strLocalIP);
		}
		else
		{
			wstrIP = StringEx::string_to_wstring(m_strIP);
		}
	}
	else
	{
		wstrIP = StringEx::string_to_wstring(m_strIP);
	}

	usPort = m_usPort;

	return;
}

bool SocketServer::StartAcceptThread()
{
	EndAcceptThread();

	m_pAcceptThread = new std::thread(&SocketServer::AcceptThread, this);

	if (m_pAcceptThread)
	{
		return true;
	}

	return false;
}

void SocketServer::EndAcceptThread()
{
	if (m_pAcceptThread == nullptr)
	{
		return;
	}

	if (m_pAcceptThread->joinable())
	{
		if (m_socket != INVALID_SOCKET)
		{
			std::string strSockIP = "";	/*!< IP��ַ */
			unsigned short usSockPort = 0;	/*!< �˿� */
			TCP::GetSockIPandPort(m_socket, strSockIP, usSockPort);
			DebugPrint::Printf("�����:%s:%dͨ���ر�", strSockIP.c_str(), usSockPort);

			// �ر�SOCKET�׽���
			shutdown(m_socket, SD_BOTH);
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}

		m_pAcceptThread->join();
	}

	delete m_pAcceptThread;
	m_pAcceptThread = nullptr;

	return;
}

void SocketServer::AcceptThread()
{
	std::string strSockIP = "";	/*!< IP��ַ */
	unsigned short usSockPort = 0;	/*!< �˿� */
	TCP::GetSockIPandPort(m_socket, strSockIP, usSockPort);
	DebugPrint::Printf("�����:%s:%d�����߳̿���", strSockIP.c_str(), usSockPort);

	while (true)
	{
		//��ʼ����   
		int retVal = listen(m_socket, 1); /*!< ����ֵ */

		if (SOCKET_ERROR == retVal)
		{
			// �����쳣

			if (m_socket != INVALID_SOCKET)
			{
				//�ر��׽���
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;

				DebugPrint::Printf("�����:%s:%d�����쳣��ͨ���ر�", strSockIP.c_str(), usSockPort);

				if (m_funcPrt)
				{
					m_funcPrt(this, m_funcParamPrt, m_socket);
				}
			}
			
			//�ͷ��׽�����Դ;
			WSACleanup();

			return;
		}

		//���ܿͻ�������  
		sockaddr_in addrClient; /*!< �ͻ��˵�ַ*/

		int addrClientlen = sizeof(addrClient); /*!< �ͻ��˳���*/

		SOCKET sClient = accept(m_socket, (sockaddr FAR*)&addrClient, &addrClientlen); /*!< �ͻ���SOCKET������*/

		if (INVALID_SOCKET == sClient)
		{
			//�ر��׽���
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;

			//�ͷ��׽�����Դ;
			WSACleanup();

			DebugPrint::Printf("�����:%s:%d���տͻ����쳣��ͨ���ر�", strSockIP.c_str(), usSockPort);

			if (m_funcPrt)
			{
				m_funcPrt(this, m_funcParamPrt, m_socket);
			}

			return;
		}
		else
		{
			AcceptClient(sClient);
		}

		Sleep(1);
	}

	DebugPrint::Printf("�����:%s:%d�����߳̽���", strSockIP.c_str(), usSockPort);

	return;
}

void SocketServer::AcceptClient(const SOCKET socket)
{
	std::string strSockIP = "",strPeerClient = "";	/*!< IP��ַ */
	unsigned short usSockPort = 0,usPeerPort = 0;	/*!< �˿� */
	TCP::GetSockIPandPort(socket, strSockIP, usSockPort);
	TCP::GetPeerIPandPort(socket, strPeerClient, usPeerPort);
	DebugPrint::Printf("�����%s:%d���ӿͻ���%s:%d�ɹ�", strSockIP.c_str(), usSockPort, strPeerClient.c_str(), usPeerPort);

	if (m_funcPrt == nullptr)
	{
		// �ر�SOCKET�׽���
		shutdown(socket, SD_BOTH);
		closesocket(socket);

		DebugPrint::Printf("�����%s:%d���ӿͻ���%s:%d�ж�", strSockIP.c_str(), usSockPort, strPeerClient.c_str(), usPeerPort);

		return;
	}

	m_funcPrt(this, m_funcParamPrt, socket);

	if (m_ulThreadID == 0)
	{
		// �ر�SOCKET�׽���
		shutdown(socket, SD_BOTH);
		closesocket(socket);

		DebugPrint::Printf("�����%s:%d���ӿͻ���%s:%d�ж�", strSockIP.c_str(), usSockPort, strPeerClient.c_str(), usPeerPort);

		return;
	}

	PostThreadMessage(m_ulThreadID, TM_TCPSERVER_ACCEPT, socket, 0);

	return;
}

#endif //_TCP_H
