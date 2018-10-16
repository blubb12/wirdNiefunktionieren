#include <iostream>
#include <iomanip>
#include <cstring>
#include "GF256.h"
#include "AES_Algorithm.h"
#include "byte.h"


#include "filters.h" // StringSink
#include "hex.h" // HexEncoder
#include "aes.h" // AES_Algorithm
#include "modes.h" // ECB_Mode
#include "base64.h"
#include <assert.h>
#include <fstream>
#include <sstream>

void demo();

using namespace std;
using namespace CryptoPP;

int main(int argc, char**argv) {
    AES_Algorithm aes; //AES im Verbose Modus
    ifstream in_file;
    ofstream out_file;

    vector<byte> output;
    vector<byte> key;
    vector<byte>::iterator oi;

    if(argc==2 && strcmp(argv[1], "demo")==0){ //Demo Modus
        demo();
        return 0;
    }


    if(argc<4){ //Überprüfung der Parameter
        cerr << "Usage: " << argv[0] << " [d|e] <key> <in_file> [<out_file>]" << endl;
        cerr << "Info! For demonstration use: " << argv[0] << " demo" << endl;
        return 1;
    }

    //Überprüfung der Schlüsselänge
    if(strlen(argv[2])/2!=16 && strlen(argv[2])/2!=24 && strlen(argv[2])/2!=32){
        cerr << "Key must be an hexadecimal string of length 32 (128 Bit), 48 (192 Bit) or 64 (256 Bit)." << endl;
        return 1;
    }

    //Überprüfung ob Schlüssel Hex-codiert ist.
    for (int i = 0; i < strlen(argv[2]); i+=2){
        if (isxdigit(argv[2][i])==0 || isxdigit(argv[2][i+1])==0) {
            cerr << "Invalid key (no hex digit)." << endl;
            return 1;
        }
        else{
            key.push_back(aes.hexToByte(argv[2][i])*16+aes.hexToByte(argv[2][i+1]));
        }
    }

    //Die Datei, deren Inhalt verschlüsselt werden soll wird geöffnet.
    in_file.open(argv[3]);
    if (!in_file.is_open()) {
        cerr << "Could not open file >" << argv[3] << "<" << endl;
        return 1;
    }

    //Dateigrößer wird ermittelt und Bytevektor vorbereitet.
    in_file.seekg(0, ios::end);
    size_t fileSize = in_file.tellg();
    in_file.seekg(0, ios::beg);
    vector<byte> input(fileSize, 0);

    if (strcmp(argv[1], "e")==0) {
        in_file.read(reinterpret_cast<char *>(&input[0]), fileSize);//Datei wird in Byte Vektor gespeichert.
        input.pop_back();
    }
    else {
        string cipherText((std::istreambuf_iterator<char>(in_file)), //Zu entschlüssender Text wird in Bytevektor kopiert.
                        std::istreambuf_iterator<char>());
        int j = input.size();
        for (int i = 0; i< j; i++){
            if(i<j/2)
                input[i] = aes.hexToByte(cipherText[i*2])*16+aes.hexToByte(cipherText[i*2+1]);
            else
                input.pop_back();
        }
    }
    in_file.close();


    if (strcmp(argv[1], "e")==0) { //Ver- oder Entschlüsselung
        aes.encrypt(input, key, output);
    }
    else {
        aes.decrypt(input, key, output);
    }


    if (argc==5) { //Optional in eine Datei reinschreiben.
        out_file.open(argv[4]);
        if (!out_file.is_open()) {
            cerr << "Could not open output file >" << argv[4] << "<" << endl;
            return 1;
        } // if
        else {
            oi = output.begin();

            while (oi != output.end()){ //Ausgabe des Textes
                if(strcmp(argv[1], "e")==0)
                    out_file << uppercase << hex << setfill('0') << setw(2) << (int) *oi;
                else
                    out_file <<  (char) *oi;
                oi++;
            }
            out_file.close();
        }
    }

    oi = output.begin();
    while (oi != output.end()){ //Ausgabe des Textes
        if(strcmp(argv[1], "e")==0)
            cout << uppercase << hex << setfill('0') << setw(2) << (int) *oi;
        else
            cout <<  (char) *oi;
        oi++;
    }

    cout << endl;

    return 0;
}

void demo() {
    ECB_Mode<AES>::Encryption aes_enc;
    ECB_Mode<AES>::Decryption aes_dec;
    AES_Algorithm aes;

    byte key_cryptopp[16];
    string plain_text, cipher_text;
    vector<byte> key_aes, plainText, cipherText;
    vector<byte>::iterator oi;
    stringstream cipherTextBuffer;

    //Keys vorbereiten!
    string encoded_key = "08a8cbfe7a3d1262c8abc3d1197dfefe";
    StringSource(encoded_key, true, new HexDecoder(new ArraySink(key_cryptopp, 16)));
    for (int i = 0; i < encoded_key.size(); i+=2){
        key_aes.push_back(aes.hexToByte(encoded_key[i])*16+aes.hexToByte(encoded_key[i+1]));
    }

    cout << "=======Encrypted with CryptoPP and decrypted with own AES=========" << endl << endl;

    aes_enc.SetKey(key_cryptopp, sizeof(key_cryptopp)); //AES initialisieren
    plain_text = "Diese Botschaft ist sehr sehr geheim. Sie wurde mit CryptoPP verschlüsselt und mit dem eigenen AES Algorithmus entschlüsselt!";

    try {
        StringSource(plain_text, true,
                     new StreamTransformationFilter(aes_enc,
                                                    new HexEncoder(
                                                            new StringSink(cipher_text)
                                                    )
                     )
        );
    } catch (const Exception& e) {
        cerr << e.what() << endl;
        assert(false);
    }

    cout << cipher_text << endl; //Ausgabe des verschlüsselten Textes

    for (int i = 0; i< cipher_text.size(); i+=2)
        cipherText.push_back(aes.hexToByte(cipher_text[i])*16+aes.hexToByte(cipher_text[i+1]));

    aes.decrypt(cipherText, key_aes, plainText); //Entschlüsselung


    oi = plainText.begin();
    while (oi != plainText.end()){ //Ausgabe des Textes
        cout <<  (char) *oi;
        oi++;
    }
    cout << endl << endl;

    cout << "=======Encrypted with own AES and decrypted with CryptoPP=========" << endl << endl;

    plain_text="";//Variablen werden zurückgesetzt.
    cipher_text="";
    plainText.clear();
    cipherText.clear();

    const char *cstr = "Diese Botschaft ist sogar noch geheimer. Sie wurde mit dem eigenen AES Algorithmus verschlüsselt und mit CryptPP entschlüsselt!";
    plainText.assign( cstr, cstr+strlen(cstr));

    aes.encrypt(plainText, key_aes, cipherText); //Verschlüsselt

    oi = cipherText.begin();
    while (oi != cipherText.end()){ //Ausgabe des Textes
        cipherTextBuffer << uppercase << hex << setfill('0') << setw(2) << (int) *oi;
        cout << uppercase << hex << setfill('0') << setw(2) << (int) *oi;
        oi++;
    }

    cout << endl;

    aes_dec.SetKey(key_cryptopp, sizeof(key_cryptopp)); //AES initalisieren

    StringSource(cipherTextBuffer.str(), true, new HexDecoder(new StringSink(cipher_text)));

    try {
        StringSource(cipher_text, true,
                     new StreamTransformationFilter(aes_dec, new StringSink(plain_text)
                     )
        );
    } catch (const Exception& e) {
        cerr << e.what() << endl;
        assert(false);
    }

    cout << plain_text << endl; //Ausgabe des entschlüsselten Textes
}

