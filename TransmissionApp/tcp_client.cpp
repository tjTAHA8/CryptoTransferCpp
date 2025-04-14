#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Fonction pour envoyer un fichier
void sendFile(SOCKET socket) {
    string filePath;
    cout << "Entrez le chemin absolu du fichier a envoyer : ";
    getline(cin, filePath);

    ifstream file(filePath, ios::binary);
    if (!file) {
        cerr << "Erreur : Impossible d'ouvrir le fichier : " << filePath << endl;
        return;
    }

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        send(socket, buffer, file.gcount(), 0);
    }

    if (file.gcount() > 0) {
        send(socket, buffer, file.gcount(), 0);
    }

    file.close();
    cout << "Fichier envoye avec succes !" << endl;
}

// Fonction pour recevoir un fichier
void receiveFile(SOCKET socket) {
    string filePath;
    cout << "Entrez le chemin ou enregistrer le fichier recu : ";
    getline(cin, filePath);

    ofstream file(filePath, ios::binary);
    if (!file) {
        cerr << "Erreur : Impossible de creer le fichier : " << filePath << endl;
        return;
    }

    char buffer[1024];
    int bytesRead;
    cout << "Reception du fichier..." << endl;
    while ((bytesRead = recv(socket, buffer, sizeof(buffer), 0)) > 0) {
        file.write(buffer, bytesRead);
    }

    file.close();
    cout << "Fichier recu avec succes !" << endl;
}

// Fonction principale
int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup a echoue." << endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Echec de la creation du socket : " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = htons(8080);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Echec de la connexion au serveur : " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connecte au serveur. Choisissez une action (send/receive) : ";
    string action;
    cin >> action;
    cin.ignore();

    send(clientSocket, action.c_str(), action.size(), 0);

    if (action == "send") {
        sendFile(clientSocket);
    } else if (action == "receive") {
        receiveFile(clientSocket);
    } else {
        cerr << "Action inconnue." << endl;
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
