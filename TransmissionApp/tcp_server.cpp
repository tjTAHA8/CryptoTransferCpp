#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <string>

#pragma comment(lib, "ws2_32.lib")

std::atomic<bool> stopListening(false);

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
    cout << "Envoi du fichier : " << filePath << endl;
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

// Fonction pour gérer la communication avec un client
void handleClient(SOCKET clientSocket) {
    char action[10];

    int result = recv(clientSocket, action, sizeof(action), 0);
    if (result > 0) {
        action[result] = '\0'; // Terminer la chaîne

        if (string(action) == "send") {
            cout << "Le client veut envoyer un fichier." << endl;
            receiveFile(clientSocket);
        } else if (string(action) == "receive") {
            cout << "Le client veut recevoir un fichier." << endl;
            sendFile(clientSocket);
        } else {
            cerr << "Action inconnue recue : " << action << endl;
        }
    } else {
        cerr << "Erreur lors de la reception de l'action : " << WSAGetLastError() << endl;
    }

    closesocket(clientSocket);
    cout << "Client deconnecte." << endl;
}

// Fonction principale
int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup a echoue." << endl;
        return 1;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        cerr << "Echec de la creation du socket : " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Echec du bind : " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Echec de l'ecoute : " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Le serveur ecoute sur le port 8080..." << endl;

    while (!stopListening) {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Echec de l'acceptation : " << WSAGetLastError() << endl;
            break;
        }

        cout << "Client connecte." << endl;
        thread(handleClient, clientSocket).detach();
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
