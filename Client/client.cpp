#include <iostream>
#include <sstream>
#include <string>

// ��� ���������� ������ freeaddrinfo � MinGW
// ���������: http://stackoverflow.com/a/20306451
#define _WIN32_WINNT 0x501

#include <WinSock2.h>
#include <WS2tcpip.h>

// ����������, ����� �������� ����������� � DLL-�����������
// ��� ������ � �������
#pragma comment(lib, "Ws2_32.lib")

using std::cerr;

int main()
{
	WSADATA wsaData; // ��������� ��������� ��� �������� ����������
					 // � ���������� Windows Sockets
					 // ����� ������������� ���������� ������� ���������
					 // (������������ Ws2_32.dll)
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// ���� ��������� ������ ��������� ����������
	if (result != 0) {
		cerr << "WSAStartup failed: " << result << "\n";
		return result;
	}

	struct addrinfo* addr = NULL; // ���������, �������� ����������
								  // �� IP-������  ���������� ������

								  // ������ ��� ������������� ��������� ������
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET; // AF_INET ����������, ��� �����
							   // �������������� ���� ��� ������ � �������
	hints.ai_socktype = SOCK_STREAM; // ������ ��������� ��� ������
	hints.ai_protocol = IPPROTO_TCP; // ���������� �������� TCP
	hints.ai_flags = AI_PASSIVE; // ����� ����� ��������� �� �����,
								 // ����� ��������� �������� ����������

								 // �������������� ���������, �������� ����� ������ - addr
								 // ��� HTTP-������ ����� ������ �� 8000-� ����� ����������
	result = getaddrinfo("127.0.0.1", "8000", &hints, &addr);

	// ���� ������������� ��������� ������ ����������� � �������,
	// ������� ���������� �� ���� � �������� ���������� ���������
	if (result != 0) {
		cerr << "getaddrinfo failed: " << result << "\n";
		WSACleanup(); // �������� ���������� Ws2_32.dll
		return 1;
	}

	// �������� ������
	int listen_socket = socket(addr->ai_family, addr->ai_socktype,
		addr->ai_protocol);
	// ���� �������� ������ ����������� � �������, ������� ���������,
	// ����������� ������, ���������� ��� ��������� addr,
	// ��������� dll-���������� � ��������� ���������
	if (listen_socket == INVALID_SOCKET) {
		cerr << "Error at socket: " << WSAGetLastError() << "\n";
		freeaddrinfo(addr);
		WSACleanup();
		return 1;
	}
/*
	// ����������� ����� � IP-������
	result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);

	// ���� ��������� ����� � ������ �� �������, �� ������� ���������
	// �� ������, ����������� ������, ���������� ��� ��������� addr.
	// � ��������� �������� �����.
	// ��������� DLL-���������� �� ������ � ��������� ���������.
	if (result == SOCKET_ERROR) {
		cerr << "bind failed with error: " << WSAGetLastError() << "\n";
		freeaddrinfo(addr);
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}
*/
	// �������������� ��������� �����
	result = connect(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);

	if (result == SOCKET_ERROR) {
		cerr << "connect failed with error: " << WSAGetLastError() << "\n";
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}

	std::stringstream response; // ���� ����� ������������ ����� �������
	std::stringstream response_body; // ���� ������
/*
	// ������ ������� ��������
	// ��������� ���� ������ (HTML)
	response_body << "<title>Test C++ HTTP Server</title>\n"
		<< "<h1>Test page</h1>\n"
		<< "<p>This is body of the test page...</p>\n"
		<< "<h2>Request headers</h2>\n"
		<< "<pre>" << "</pre>\n"
		<< "<em><small>Test C++ Http Server</small></em>\n";
*/
	// ��������� ���� ����� ������ � �����������
	response << "AAAAAAAAAA";

	// ���������� ����� ������� � ������� ������� send
	result = send(listen_socket, response.str().c_str(),
		response.str().length(), 0);

	if (result == SOCKET_ERROR) {
		// ��������� ������ ��� �������� ������
		cerr << "send failed: " << WSAGetLastError() << "\n";
	}

	// ������� �� �����
	closesocket(listen_socket);
	freeaddrinfo(addr);
	WSACleanup();
	return 0;
}