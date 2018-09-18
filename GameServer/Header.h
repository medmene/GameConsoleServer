#pragma once

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <fstream>
#include <time.h>

#pragma comment(lib, "ws2_32")

#define MY_PORT 666
#define PRINTNUSERS if ( nclients ) {printf ( "%d user on-line\n", nclients);} else printf("no user on line\n")
#define sHELLO "Hello, Sailor\r\n"

int nclients = 0;
int mapSize = 20 * 40;
char mapChar[20 * 40 + 200];

//сравнение принятой карты и карты которая на сервере
char* Compare(char *buff, int &len) {
	int k = 0;
	char *res = new char[mapSize];
	for (int i = 0; i < mapSize; i += 1) {
		if (mapChar[i] != buff[i]) {
			res[k] = buff[i]; k++;
			res[k] = i / 39; k++;//res[k++]= i/39;
			res[k] = i % 39; //res[k++]=i%39;
		}
	}
	len = k++;
}

//функция взаимодействия с клиентом
DWORD WINAPI SexToClient(LPVOID client_socket)
{
	int current_client = nclients;
	srand(time(NULL));

	char buff[20 * 40 + 200];
	for (int i = 0; i < 1000; ++i)
		buff[i] = mapChar[i];

	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];
	//-6 char code for empty place
	COORD pos;
	pos.X = rand() % 36 + 3;
	pos.Y = rand() % 17 + 3;
	while (mapChar[pos.Y * 39 + pos.X] != -6) {
		pos.X = rand() % 36 + 3;
		pos.Y = rand() % 17 + 3;
	}
	mapChar[800] = 100 + nclients;
	mapChar[801 + (nclients - 1) * 6] = 100 + nclients; //номер
	mapChar[802 + (nclients - 1) * 6] = (nclients < 25) ? 65 + (nclients - 1) : 97 + (nclients - 1); //буква
	mapChar[803 + (nclients - 1) * 6] = 0; //позиция если больше 255
	mapChar[804 + (nclients - 1) * 6] = pos.X; //позиция если меньше 255
	mapChar[805 + (nclients - 1) * 6] = 0; //позиция если больше 255
	mapChar[806 + (nclients - 1) * 6] = pos.Y; //позиция если меньше 255

	send(my_sock, mapChar, sizeof(mapChar), 0);
	send(my_sock, mapChar, sizeof(mapChar), 0);
	int bytes_recv;
	while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && (bytes_recv != SOCKET_ERROR)) {
		for (int i = 0; i < 800; ++i)
			mapChar[i] = buff[i];
		Sleep(20);
		mapChar[999] = 100 + current_client;
		mapChar[800] = 100 + nclients;
		mapChar[801 + (nclients - 1) * 6] = 100 + current_client; //номер
		mapChar[802 + (nclients - 1) * 6] = (current_client < 25) ? 65 + (current_client - 1) : 97 + (current_client - 1); //буква
		pos.X = (int)(buff[802]) * 255 + (int)(buff[803]);
		pos.Y = (int)(buff[804]) * 255 + (int)(buff[805]);
		mapChar[803 + (nclients - 1) * 6] = 0; //позиция если больше 255
		mapChar[804 + (nclients - 1) * 6] = pos.X; //позиция если меньше 255
		mapChar[805 + (nclients - 1) * 6] = 0; //позиция если больше 255
		mapChar[806 + (nclients - 1) * 6] = pos.Y; //позиция если меньше 255
		send(my_sock, &mapChar[0], sizeof(mapChar), 0);
	}
	nclients--;
	printf("-disconnect\n");
	PRINTNUSERS;
	closesocket(my_sock);
	return 0;
}

void getMap() {
	char map[20][40];
	std::ifstream f("map.txt");
	char t = 32;
	int i = 0, j = 0;
	while (!f.eof()) {
		if (i == 20)break;
		f >> t;
		if (t == -73)
			t = 250;
		if (t == -80)
			t = -8;
		map[i][j] = t;
		j++;
		if (j == 40) { j = 0; i++; }
	}
	f.close();

	for (int i = 0; i < 20; ++i) {
		for (int j = 0; j < 40; ++j) {
			if (map[i][j] == '\0')mapChar[i * 40 + j] = '0';
			else mapChar[i * 40 + j] = map[i][j];
		}
	}
	return;
}
