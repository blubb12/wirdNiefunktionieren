//
// Created by simon on 11.06.18.
//

#include <iostream>
#include <iomanip>
#include <cmath>
#include <assert.h>
#include "AES_Algorithm.h"



/**
 * ak-aes-mazlum-philippi.tgz
 */


AES_Algorithm::AES_Algorithm(unsigned int verbose) {
    generateSBox();
    this->verbose = verbose;
}

AES_Algorithm::~AES_Algorithm() = default;

void AES_Algorithm::encrypt(vector<byte> plainText, vector<byte> k, vector<byte>& cipherText) {
    vector<byte> s;

    init(k);// Initaliserung mit Schlüssel

    for (int i = 0; i<nB*4; i++) //State wird mit der passenden Größe erstellt.
        s.push_back(0x00);

    int blocksToFill = ((nB*4)-(plainText.size()%(nB*4))); //Padding wird bestimmt
    for (int i = 0; i<blocksToFill;i++) //und durchgeführt
        plainText.push_back(0x00+blocksToFill);

    int numberBlocks = static_cast<int>((plainText.size() + (nB * 4) - 1) / (nB * 4)); //Anzahl an einzulesenden Blocks wird bestimmt

    for (int i = 0; i < numberBlocks;i++){



        for (int j = 0; j<nB; j++){  //Klartext wird in den State s kopiert
            s[j] = plainText[i*nB*4+j*4];
            s[j+nB] = plainText[i*nB*4+j*4+1];
            s[j+nB*2] = plainText[i*nB*4+j*4+2];
            s[j+nB*3] = plainText[i*nB*4+j*4+3];
        }

        if (verbose)
            printSBlock(s);

        addRoundKey(s, getRoundKey(0));

        for (int j = 1; j<nR; j++) //Runden werden aufgerufen
            round(j,s);

        finalRound(nR,s); //Final Round

        if (verbose)
            printSBlock(s);

        for (int i=0; i<nB*4;i++)
            cipherText.push_back(0x00);

        for(int j = 0; j<nB; j++){ //der State s wird in den Ciphertext Byte Vektor kopiert
            cipherText[i*nB*4+j*4] = s[j];
            cipherText[i*nB*4+j*4+1] = s[j+nB];
            cipherText[i*nB*4+j*4+2] = s[j+nB*2];
            cipherText[i*nB*4+j*4+3] = s[j+nB*3];
        }
    }
}



void AES_Algorithm::decrypt(vector<byte> cipherText, vector<byte> k, vector<byte> &plainText) {
    vector<byte> s;

    init(k);

    if(cipherText.size()%(nB*4)!=0){ //Ciphertext muss ein vielfaches der Blocklänge sein
        cerr << "Ciphertext ist kein vielfaches der Blocklänge!" << endl;
        throw exception();
    }

    for (int i = 0; i<nB*4; i++) //State wird mit der passenden Größe erstellt.
        s.push_back(0x00);

    int numberBlocks = static_cast<int>((cipherText.size() + (nB * 4) - 1) / (nB * 4)); //Anzahl an einzulesenden Blocks wird bestimmt

    for (int i = 0; i < numberBlocks;i++){


        for (int j = 0; j<nB; j++){ //Verschlüsselte Text wird in den State s kopiert
            s[j] = cipherText[i*nB*4+j*4];
            s[j+nB] = cipherText[i*nB*4+j*4+1];
            s[j+nB*2] = cipherText[i*nB*4+j*4+2];
            s[j+nB*3] = cipherText[i*nB*4+j*4+3];
        }

        if (verbose)
            printSBlock(s);

        addRoundKey(s, getRoundKey(nR));

        for (int j = 1; j<nR; j++)
            invRound(j,s);

        invFinalRound(nR,s); //inverse Finalround

        if (verbose)
            printSBlock(s);

        for (int i=0; i<nB*4;i++) //Plaintext Byte Vektor wird um die benötigten Bytes vergrößert
            plainText.push_back(0x00);

        for(int j = 0; j<nB; j++){ //der State s wird in den Plaintext Byte Vektor kopiert
            plainText[i*nB*4+j*4] = s[j];
            plainText[i*nB*4+j*4+1] = s[j+nB];
            plainText[i*nB*4+j*4+2] = s[j+nB*2];
            plainText[i*nB*4+j*4+3] = s[j+nB*3];
        }

        if(i==numberBlocks-1){ //wenn der letzte Block erreicht ist
            int paddedBytes = plainText[plainText.size()-1];
            for (int j = 0; j< paddedBytes;j++){ //wird solange wie das padding vorhanden ist dieses entfernt
                if (paddedBytes != plainText[plainText.size()-1]){ //dabei wird auch die Plausibilität der Entschlüsselung geprüft werden.
                    cerr << "Wrong key entered." << endl;
                    throw exception();
                }
                plainText.pop_back();
            }

        }
    }
}



void AES_Algorithm::init(const vector<byte> &k){
    if (k.size()==16) //Schlüsselänge wird überprüft
        nK=4;
    else if(k.size()==24)
        nK=6;
    else if(k.size()==32)
        nK=8;
    else {
        cerr << "Unpassende Schlüssellänge gewählt!" << endl;
        throw exception();
    }

    nB = 4;//Blocklänge ist immer 4!
    nR = kRSize[nK/2-2][(nB/2-2)];

    for (int i=0; i<((nR+1)*16);i++)//Vektor wird in benötigte Größe definiert
        kR.push_back(0x00);

    keyExpansion(k, kR);
}


void AES_Algorithm::round(int i, vector<byte>& s) {
    byteSub(s);
    shiftRows(s);
    mixColumn(s);
    addRoundKey(s, getRoundKey(i));
}


void AES_Algorithm::invRound(int i, vector<byte>& s) {
    invShiftRows(s);
    invByteSub(s);
    addRoundKey(s,getRoundKey(nR-i));
    invMixColumn(s);
}



void AES_Algorithm::finalRound(int i, vector<byte>&  s) {
    byteSub(s);
    shiftRows(s);
    addRoundKey(s, getRoundKey(i));
}


void AES_Algorithm::invFinalRound(int i, vector<byte>&  s) {
    invShiftRows(s);
    invByteSub(s);
    addRoundKey(s,getRoundKey(nR-i));
}


void AES_Algorithm::keyExpansion(vector<byte> key, vector<byte>& kR) {
    for (int i = 0; i < nK*4; i++){ //Schlüssel wird in die ersten Felder des Byte Vektors kopiert.
        kR[i] = key[i];
    }

    for (int i = nK; i < nB*(nR+1); i++) { //Erstellung der restlichen Rundenschlüssel
        if (i% nK == 0)
            UINT32toBYTEVECTOR(f((u_int8_t) i/nK, BYTEVECTORtoUINT32(kR,i-nK), BYTEVECTORtoUINT32(kR,i-1)), kR, i);
        else if (i% nK == 4 && nK > 6) //Wenn Schlüssellänge über 6 ist, kann auch dieser Fall eintreten
            UINT32toBYTEVECTOR(g(BYTEVECTORtoUINT32(kR,i-nK), BYTEVECTORtoUINT32(kR,i-1)), kR, i);
        else
            UINT32toBYTEVECTOR(BYTEVECTORtoUINT32(kR,i-nK) ^ BYTEVECTORtoUINT32(kR,i-1), kR, i);
    }

}


u_int8_t AES_Algorithm::rc(u_int8_t i){
    if (i == 0)
        return 1;
    else if (i == 1)
        return 2;
    else{
        GF256 temp(2);
        for (int j= 1; j<i; j++){//Potenzierung von 2
            temp = temp * 2;
        }
        return temp.getValue();
    }
}


u_int32_t AES_Algorithm::vectorAddition(u_int32_t a, u_int32_t b){
    return a ^ b;
}


u_int32_t AES_Algorithm::BYTEVECTORtoUINT32(vector<byte> b, int pos){
    u_int32_t temp = b[pos*4+0]; //Erstellugn des unsigned Integers mit 32 bit.
    temp <<= 8;
    temp += b[pos*4+1];
    temp <<= 8;
    temp += b[pos*4+2];
    temp <<= 8;
    temp +=  b[pos*4+3];
    return temp;
}


void AES_Algorithm::UINT32toBYTEVECTOR(u_int32_t a, vector<byte>& b, int pos){
    b[pos*4+0] = static_cast<unsigned char>(a >> 24 & 0xFF); //Speichern des unsigned Integers innerhalb des Byte Vektors
    b[pos*4+1] = static_cast<unsigned char>(a >> 16 & 0xFF);
    b[pos*4+2] = static_cast<unsigned char>(a >> 8 & 0xFF);
    b[pos*4+3] = static_cast<unsigned char>(a & 0xFF);

}

vector<byte> AES_Algorithm::getRoundKey(int round) {
    vector<byte>roundKey;
    for (int i=0; i<4*4;i++)
        roundKey.push_back(0x00); //Roundkey mit der passenden Größe wird erstellt
    for (int i=0; i<4; i++){
        roundKey[i]=kR[round*4*4+i*4]; //und gespeichert.
        roundKey[i+4]=kR[round*4*4+i*4+1];
        roundKey[i+4*2]=kR[round*4*4+i*4+2];
        roundKey[i+4*3]=kR[round*4*4+i*4+3];
    }
    return roundKey;
}

u_int32_t AES_Algorithm::f(u_int8_t i, u_int32_t x, u_int32_t y){
    u_int32_t temp = sBox[(y>>16)%256];
    temp <<= 8;
    temp += sBox[(y>>8)%256];
    temp <<= 8;
    temp += sBox[(y%256)];
    temp <<= 8;
    temp +=  sBox[(y>>24)%256];
    u_int32_t temp2 = rc(i-1);
    temp2 <<= 24;
    return vectorAddition(vectorAddition(x,temp),temp2);
}

u_int32_t AES_Algorithm::g(u_int32_t x, u_int32_t y){
    u_int32_t temp = sBox[(y>>24)%256];
    temp <<= 8;
    temp += sBox[(y>>16)%256];
    temp <<= 8;
    temp += sBox[((y>>8)%256)];
    temp <<= 8;
    temp +=  sBox[y%256];
    return vectorAddition(x,temp);
}

void AES_Algorithm::addRoundKey(vector<byte>& s, vector<byte> k) {
    for (int i = 0; i < s.size(); i++)
        s[i] = s[i] ^ k[i]; //State XORed mit dem Rundenschlüssel
}

void AES_Algorithm::byteSub(vector<byte>&  s) {
    for (unsigned char &i : s)// State wird mit Sbox ersetzt
        i = sBox[i];
}

void AES_Algorithm::invByteSub(vector<byte>&  s) {
    for (unsigned char &i : s)//State wird mit invertierten SBox ersetzt
        i = rsBox[i];
}

void AES_Algorithm::shiftRows(vector<byte>& s) {
    int c[3];

    switch(nB) { //Es wird zwischen Blöcklängen unterschieden obwohl diese festgelegt ist
        case 4:
        case 6:
            c[0] = 1;
            c[1] = 2;
            c[2] = 3;
            break;
        case 8:
            c[0] = 1;
            c[1] = 3;
            c[2] = 4;
            break;
        default:
            std::cerr << "Block length doesnt fit." << std::endl;
            break;
    }

    for (int i = 1; i<4; i++){
        int k = 0;
        byte temp[c[i-1]];

        for(int j = 0; j<c[i-1]; j++){ //Werte der Reihen werden gespeichert und verschoben
            temp[j] = s[j+i*nB];
        }

        for (int j = 0; j<nB-c[i-1]; j++){
            s[j+i*nB] = s[j+i*nB+c[i-1]];
        }

        for (int j = nB*(i+1)-c[i-1]; j < nB*(i+1); j++, k++){
            s[j] = temp[k];
        }
    }
}

void AES_Algorithm::invShiftRows(vector<byte>& s) {
    int c[3];

    switch(nB) {
        case 4:
        case 6:
            c[0] = nB-1;
            c[1] = nB-2;
            c[2] = nB-3;
            break;
        case 8:
            c[0] = nB-1;
            c[1] = nB-3;
            c[2] = nB-4;
            break;
        default:
            std::cerr << "Block length doesnt fit." << std::endl;
            break;
    }

    for (int i = 1; i<4; i++){
        int k = 0;
        byte temp[c[i-1]];

        for(int j = 0; j<c[i-1]; j++){
            temp[j] = s[j+i*nB];
        }

        for (int j = 0; j<nB-c[i-1]; j++){
            s[j+i*nB] = s[j+i*nB+c[i-1]];
        }

        for (int j = nB*(i+1)-c[i-1]; j < nB*(i+1); j++, k++){
            s[j] = temp[k];
        }
    }
}

void AES_Algorithm::mixColumn(vector<byte>& s) {
    u_int8_t column[4];

    for (int i = 0; i < nB; i++) {
        u_int8_t temp[4];

        for (int j = 0; j < 4; j++) {
            column[j] = s[(j * nB) + i];
            temp[j] = column[j];
        }

        column[0] = (GF256(temp[0]) * GF256(2) + //Werte der Spalten werden addiert und Multipliziert
                     (GF256(temp[1]) * GF256(3)) +
                     (GF256(temp[2]) * GF256(1)) +
                     GF256(temp[3]) * GF256(1)).getValue();

        column[1] = (GF256(temp[0]) * GF256(1) +
                     (GF256(temp[1]) * GF256(2)) +
                     (GF256(temp[2]) * GF256(3)) +
                     GF256(temp[3]) * GF256(1)).getValue();

        column[2] = (GF256(temp[0]) * GF256(1) +
                     (GF256(temp[1]) * GF256(1)) +
                     (GF256(temp[2]) * GF256(2)) +
                     GF256(temp[3]) * GF256(3)).getValue();

        column[3] = (GF256(temp[0]) * GF256(3) +
                     (GF256(temp[1]) * GF256(1)) +
                     (GF256(temp[2]) * GF256(1)) +
                     GF256(temp[3]) * GF256(2)).getValue();


        for (int j = 0; j < nB; j++) {
            s[(j * 4) + i] = column[j];
        }
    }
}

void AES_Algorithm::invMixColumn(vector<byte>& s) {
    u_int8_t column[4];

    for (int i = 0; i < nB; i++) {
        u_int8_t temp[4];

        for (int j = 0; j < 4; j++) {
            column[j] = s[(j * nB) + i];
            temp[j] = column[j];
        }

        column[0] = (GF256(temp[0]) * GF256(0x0E) + //Werte der Spalten werden addiert und Multipliziert
                     (GF256(temp[1]) * GF256(0x0B)) +
                     (GF256(temp[2]) * GF256(0x0D)) +
                     GF256(temp[3]) * GF256(0x09)).getValue();

        column[1] = (GF256(temp[0]) * GF256(0x09) +
                     (GF256(temp[1]) * GF256(0x0E)) +
                     (GF256(temp[2]) * GF256(0x0B)) +
                     GF256(temp[3]) * GF256(0x0D)).getValue();

        column[2] = (GF256(temp[0]) * GF256(0x0D) +
                     (GF256(temp[1]) * GF256(0x09)) +
                     (GF256(temp[2]) * GF256(0x0E)) +
                     GF256(temp[3]) * GF256(0x0B)).getValue();

        column[3] = (GF256(temp[0]) * GF256(0x0B) +
                     (GF256(temp[1]) * GF256(0x0D)) +
                     (GF256(temp[2]) * GF256(0x09)) +
                     GF256(temp[3]) * GF256(0x0E)).getValue();

        for (int j = 0; j < nB; j++) {
            s[(j * 4) + i] = column[j];
        }
    }
}

void AES_Algorithm::printSBlock(vector<byte> s){ //Debug Ausgabe des aktuellen States
    for(int i = 0; i<4*nB; i++){
        if (i%4==0 and i!=0)
            cout << endl;
        cout << uppercase << hex << setfill('0') << setw(2) << (int) s[i]; // << " ";
    }
    cout << endl << endl;
}

byte AES_Algorithm::hexToByte(byte xdigit) { //Hex to Byte Methode
    if(isxdigit(xdigit)<0){
        cerr << "Text seems to be non-hexadecimal." << endl;
        throw exception();
    }
    else{
        if (isdigit(xdigit)>0) {
            return (xdigit-'0');
        } // if
        else {
            return (tolower(xdigit)-'a')+10;
        } // else
    } // if
}

u_int8_t AES_Algorithm::calculateSboxValue(u_int8_t value) {
    uint8_t tempOne, tempTwo;

    if (value == 0)
        return 0x63;

    tempOne = GF256(value).inverse().getValue();
    tempTwo = GF256(value).inverse().getValue();

    for (int i = 0; i < 4; i++) {
        tempOne = (tempOne << 1) | (tempOne >> 7);
        tempTwo = (GF256(tempTwo) +  GF256(tempOne)).getValue();
    }

    tempTwo = (GF256(tempTwo) +  GF256(0x63)).getValue();
    return tempTwo;
}

void AES_Algorithm::generateSBox() {
    for (int i=0; i<256;i++)
        sBox[i] = calculateSboxValue(i);

    for (int i=0; i<256;i++)
        rsBox[sBox[i]] = i;
}