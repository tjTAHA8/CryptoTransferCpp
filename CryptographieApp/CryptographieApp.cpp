#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <fstream>
#include <numeric>
#include <limits>

using namespace std;

#include <stdexcept>

int mod_exp(int base, int exp, int mod) {
    if (mod <= 0) {
        throw std::invalid_argument("Modulo must be greater than 0");
    }

    long long result = 1; 
    long long b = base % mod; // Réduction initiale dans le domaine modulo
    if (b < 0) b += mod; 

    while (exp > 0) {
        if (exp % 2 == 1) { // Si exp est impair
            result = (result * b) % mod;
        }
        exp = exp >> 1; // Divise exp par 2
        b = (b * b) % mod; // Mise à jour de base
    }
    return (int)result;
}

// Génération des nombres premiers
bool estPremier(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}
// Tester si e et r sont premiers entre eux
bool EestValide(int e, int r) {
    return gcd(e, r) == 1;
}
// Générer p et q
int genererNombrePremier(int borneMin, int borneMax) {
    int nombre;
    do {
        nombre = rand() % (borneMax - borneMin + 1) + borneMin;
    } while (!estPremier(nombre));
    return nombre;
}
int mod_inverse(int e, int phi) {
    int t = 0, new_t = 1;
    int r = phi, new_r = e;
    while (new_r != 0) {
        int quotient = r / new_r;
        int temp_t = t;
        t = new_t;
        new_t = temp_t - quotient * new_t;

        int temp_r = r;
        r = new_r;
        new_r = temp_r - quotient * new_r;
    }
    if (r > 1) return -1; // Pas d'inverse
    if (t < 0) t += phi; // Assurer que t est positif
    return t;
}

int main() {
    string EncryptedMessage;
    string message;
    vector<int> v;
    vector<int> m;
    char mode;
    char keys;
    string chaine;
    int p;
    int q;
    int a;
    int n, r, d;
    int e = 65537;
    int borneMin = 200, borneMax = 1000;
    int x;
    int number;
    int PrivateK1;
    int PrivateK2;
    string Path;
    srand(time(NULL));
    cout << "To Generate keys tap (y) else press (n) "<<endl;
    cin>> keys;
    if (keys == 'y' || keys == 'Y') {
    do {
            p = genererNombrePremier(borneMin, borneMax);
            q = genererNombrePremier(borneMin, borneMax);
            r = (p - 1) * (q - 1);
        } while (p == q || !EestValide(e, r));  
    n = p * q;
    d = mod_inverse(e, r);
     ofstream PublicKEY;
        ofstream PrivateKEY;
          PublicKEY.open("PublicKEY.txt",ios::out);
        PrivateKEY.open("PrivateKey.txt",ios::out);
     PublicKEY << "Your Public Key is: ( Public Key 1 :" << e << ",Public Key 2 :" << n << ")" << std::endl;
        PrivateKEY<< "Your Private Key is: ( Private Key 1 :" << d << ", Private Key 2 :" << n << ")" << std::endl;
        PrivateKEY.close(); 
        PublicKEY.close(); 
    }
    
    cout << "To Encrypt tap (e) or press (d) to Decrypt : "<<endl;
    cin >> mode;
       while (mode != 'e' && mode != 'E' && mode != 'd' && mode != 'D') {
        cout << "Please choose either <E/e> for encryption or <D/d> for decryption: "<<endl;
        cin >> mode;
        
    } 
    // Encryption
    if (mode == 'e' || mode == 'E') {
         
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Tap your Public Key : "<<endl;
        cout << "key 1 ="<<endl;
        cin >> e;
        cout << "key 2 ="<<endl;
        cin >> n;
        cout << "Tap your message : "<<endl;
        ofstream CryptoProject;
        CryptoProject.open("Cypher.txt");  
        getline(cin, message);  
        for (auto c : message) {
            x = mod_exp(c, e, n);
            v.push_back(x);
        }
        cout << "Encryption completed successfully." << endl;
        for (auto val : v) {
            CryptoProject << val << " ";
        } 
        CryptoProject.close();
    }
    // Decryption
    if (mode == 'd' || mode == 'D') {

        cout<< "Enter the path of the file you want to Decrypte : "<<endl;
        cin>> Path;
        ifstream CryptoProject(Path, ios::in); 
        ofstream DecryptedFile("Decrypted_message.txt", ios::out); 
        if (CryptoProject.is_open() && DecryptedFile.is_open()) {
            vector<int> m; 
            int number;
            cout << "To Decrypt Enter your Private Key : "<<endl;
            cout << " Private Key 1 : "<<endl;
            cin>>PrivateK1 ;
            cout << " Private Key 2 : "<<endl;
            cin>>PrivateK2 ;
            // Lire les nombres du fichier 
            while (CryptoProject >> number) {
                a = mod_exp(number, PrivateK1, PrivateK2);
                m.push_back(a);
            }
            // Convertir les valeurs déchiffrées 
            for (const auto& num : m) {
                DecryptedFile <<(char) num;
            }
             cout << "Decryption completed successfully." << endl;
            // Fermer les fichiers
            CryptoProject.close();
            DecryptedFile.close();
        } else {
            cerr << "Error: Could not open one or both files!" << endl;
        }
    }
    return 0;
}
