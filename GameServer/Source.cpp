#include "Header.h"

int main(int argc, char* argv[])
{
	srand(time(NULL));
	getMap();
	char buff[1024]; //системный буфер
	printf("TCP SERVER DEMO\n");
	//ѕроверка наличи€ инструментов (библиотеки)
	if (WSAStartup(0x0202, (WSADATA *)&buff[0])){
		printf("Error WSAStartup %d\n", WSAGetLastError());
		return 1;
	}
	//серверный сокет
	int mysocket;
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error Socket%d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	//информаци€ о расположении сервера 
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);
	local_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//прикрепл€ем к сокету адрес
	if (bind(mysocket, (sockaddr *)&local_addr, sizeof(local_addr))){
		printf("Error bind\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	//монтируем сокет дл€ прослушивани€
	if (listen(mysocket, 0x100)){
		printf("Error Listen\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	printf("Connection...\n");
	//ждем клиентов
	SOCKET client_socket;
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);

	while ((client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size)))
	{
		nclients++;
		HOSTENT *hst;
		hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, 4, AF_INET); //gлучени инфы о пользователе
		printf("+%s [%s] new connect!\n", (hst) ? hst->h_name : "", inet_ntoa(client_addr.sin_addr));
		PRINTNUSERS;
		DWORD thID;
		//поток дл€ каждого пользовател€
		CreateThread(NULL, NULL, SexToClient, &client_socket, NULL, &thID);
	}
	return 0;
}