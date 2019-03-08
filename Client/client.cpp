#include <iostream>
#include <sstream>
#include <string>

// Для корректной работы freeaddrinfo в MinGW
// Подробнее: http://stackoverflow.com/a/20306451
#define _WIN32_WINNT 0x501

#include <WinSock2.h>
#include <WS2tcpip.h>

// Необходимо, чтобы линковка происходила с DLL-библиотекой
// Для работы с сокетам
#pragma comment(lib, "Ws2_32.lib")

using std::cerr;

int main()
{
	WSADATA wsaData; // служебная структура для хранение информации
					 // о реализации Windows Sockets
					 // старт использования библиотеки сокетов процессом
					 // (подгружается Ws2_32.dll)
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Если произошла ошибка подгрузки библиотеки
	if (result != 0) {
		cerr << "WSAStartup failed: " << result << "\n";
		return result;
	}

	struct addrinfo* addr = NULL; // структура, хранящая информацию
								  // об IP-адресе  слущающего сокета

								  // Шаблон для инициализации структуры адреса
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET; // AF_INET определяет, что будет
							   // использоваться сеть для работы с сокетом
	hints.ai_socktype = SOCK_STREAM; // Задаем потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP; // Используем протокол TCP
	hints.ai_flags = AI_PASSIVE; // Сокет будет биндиться на адрес,
								 // чтобы принимать входящие соединения

								 // Инициализируем структуру, хранящую адрес сокета - addr
								 // Наш HTTP-сервер будет висеть на 8000-м порту локалхоста
	result = getaddrinfo("127.0.0.1", "8000", &hints, &addr);

	// Если инициализация структуры адреса завершилась с ошибкой,
	// выведем сообщением об этом и завершим выполнение программы
	if (result != 0) {
		cerr << "getaddrinfo failed: " << result << "\n";
		WSACleanup(); // выгрузка библиотеки Ws2_32.dll
		return 1;
	}

	// Создание сокета
	int listen_socket = socket(addr->ai_family, addr->ai_socktype,
		addr->ai_protocol);
	// Если создание сокета завершилось с ошибкой, выводим сообщение,
	// освобождаем память, выделенную под структуру addr,
	// выгружаем dll-библиотеку и закрываем программу
	if (listen_socket == INVALID_SOCKET) {
		cerr << "Error at socket: " << WSAGetLastError() << "\n";
		freeaddrinfo(addr);
		WSACleanup();
		return 1;
	}
/*
	// Привязываем сокет к IP-адресу
	result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);

	// Если привязать адрес к сокету не удалось, то выводим сообщение
	// об ошибке, освобождаем память, выделенную под структуру addr.
	// и закрываем открытый сокет.
	// Выгружаем DLL-библиотеку из памяти и закрываем программу.
	if (result == SOCKET_ERROR) {
		cerr << "bind failed with error: " << WSAGetLastError() << "\n";
		freeaddrinfo(addr);
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}
*/
	// Инициализируем слушающий сокет
	result = connect(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);

	if (result == SOCKET_ERROR) {
		cerr << "connect failed with error: " << WSAGetLastError() << "\n";
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}

	std::stringstream response; // сюда будет записываться ответ клиенту
	std::stringstream response_body; // тело ответа
/*
	// Данные успешно получены
	// формируем тело ответа (HTML)
	response_body << "<title>Test C++ HTTP Server</title>\n"
		<< "<h1>Test page</h1>\n"
		<< "<p>This is body of the test page...</p>\n"
		<< "<h2>Request headers</h2>\n"
		<< "<pre>" << "</pre>\n"
		<< "<em><small>Test C++ Http Server</small></em>\n";
*/
	// Формируем весь ответ вместе с заголовками
	response << "AAAAAAAAAA";

	// Отправляем ответ клиенту с помощью функции send
	result = send(listen_socket, response.str().c_str(),
		response.str().length(), 0);

	if (result == SOCKET_ERROR) {
		// произошла ошибка при отправле данных
		cerr << "send failed: " << WSAGetLastError() << "\n";
	}

	// Убираем за собой
	closesocket(listen_socket);
	freeaddrinfo(addr);
	WSACleanup();
	return 0;
}