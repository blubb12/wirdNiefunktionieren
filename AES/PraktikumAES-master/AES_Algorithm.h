//
// Created by simon on 11.06.18.
//

#ifndef PRAKTIKUMAES_AES_H
#define PRAKTIKUMAES_AES_H


#include <sys/param.h>
#include "byte.h"
#include "GF256.h"
#include <vector>

using namespace std;

class AES_Algorithm{

public:
    /**
    * Konstruktor des AES Objektes.
    * @param verbose optionale Ausgabe der Zwischenschritte
    */
    AES_Algorithm(unsigned int verbose = 0);

    /**
     * Destruktor des AES Objektes
     */
    ~AES_Algorithm();

    /**
     * Verschlüsselungsmethode des AES Algorithmus. Der Klartext wird in passende Blöcke eingefügt, welche anschließend verarbeitet werden.
     * Die Anzahl der Runden sind dabei abhängig von der Schlüssellänge.
     * Ist der Verschlüsselungsvorgang abgeschlossen, werden die verarbeitete Texte in den cipherText Vektor eingefügt und anschließend zurückgegeben.
     * @param plainText der Klartext in einem Byte Vektor gespeichert
     * @param k der Schlüssel in einem Byte Vektor gespeichert
     * @param cipherText der Verschlüsselte Text in einem Byte Vektor gespeichert
     */
    void encrypt(vector<byte> plainText, vector<byte> key, vector<byte>& cipherText);

    /**
     * Entschlüsselugnsmethode des AES Algorithmus. Der Ciphertext wird in Blöcke eingefügt, welche anschließend verarbeitet werden.
     * Die Anzahl der Runden sind dabei abhängig von der Schlüssellänge.
     * Ist der Entschlüsselungsvorgang abgeschlossen, werden die verarbeitete Texte in den plainText Vektor eingefügt und anschließend zurückgegeben.
     * Zudem wird das padding entfernt und die Entschlüsselung zusätzlich auf Plausibilität geprüft.
     * @param plainText der Klartext in einem Byte Vektor gespeichert
     * @param k der Schlüssel in einem Byte Vektor gespeichert
     * @param cipherText der Verschlüsselte Text in einem Byte Vektor gespeichert
     */
    void decrypt(vector<byte> cipherText, vector<byte> key, vector<byte>& plainText);
    /**
     * Prüft zuerst ob Eingabe kleiner 0, sonst werden hexadecimale Eingaben in Bytes umgewandelt
     * und ausgegeben
     * @param xdigit Hexadezimaler Wert als Eingabe
     * @return Ausgabe erfolgt in Bytes
     */
    byte hexToByte(byte xdigit);

private:
    unsigned int verbose;

    /**
     * Anzahl der Runden wird abhängig von der Größe des Schlüssels definiert.
     * Zudem wird die Schlüsselänge überprüft und der Schlüsselvektor vorbereitet.
     * @param key Übergebener Schlüssel um Länge festzustellen
     */
    void init(const vector<byte>& key);

    /**
     * Expandiert den übergebenen Schlüssel auf die Menge der benötigten Schlüssel.
     * Diese werden in dem Byte Vektor kR gespeichert, damit sie später in der passenden Runde genutzt werden können.
     * @param key der zu expandierende Schlüssel
     * @param kR der Byte Vektor in dem die expandierten Schlüssel gespeichert werden
     */
    void keyExpansion(vector<byte> key, vector<byte>& kR);
  
    /**
     * Addiert Rundenschlüssel auf den internen Zustand. Hierzu wird jedes Byte des Zustand mit dem
     * entsprechenden Byte des Rundenschlüssels via XOR (Bitweise XOR) verknüpft.
     * @param s interner Zustand
     * @param k Rundenschlüssel
     */
    void addRoundKey(vector<byte>& s, vector<byte> k);

    /**
     * Soviele Runden wie es gibt, wird round ausgeführt. Dabei wird:
     * bytesub(s), shiftrow(s), mixcolumns(s) und addroundkey(s, Stelle von Rundenschlüssel kR)
     * @param i Rundenschlüssel an der Stelle i
     * @param s interner Zustand
     */
    void round(int i, vector<byte>&  s);
  
    /**
     * Vergleiche mit round(i, s), aber da es die letzte Runde ist, wird diesmal
     * mixColumn(s) nicht aufgerufen
     * @param i Rundenschlüssel an der Stelle i
     * @param s interner Zustand
     */
    void finalRound(int i, vector<byte>&  s);

    /**
     * Die S-Box wird auf jedes Byte des internen Zustands angewandt. Dies ist eine
     * nicht-lineare byte-weise Substitution. Die S-Box ist invertierbar und wird
     * mittels zwei bijektiven Abbildungen erzeugt: 1. Das zu berechnende Byte wird
     * als Element von GF(2^8) interpretiert und auf sein multiplikatives Inverses
     * abgebildet, wobei 0 auf sich selbst abgebildet wird (neutrales Element).
     * 2. eine affine Abbildung über GF(2) berechnen
     * @param s interner Zustand
     */
    void byteSub(vector<byte>&  s);
    /**
     * Zeilenweise Linksshift je nach Blocklänge (vgl. Folie 29/40). Dabei wird beim Überhang
     * darauf geachtet das aus dem Feld herausgeschobene Bytes nicht auf der anderen Seite "runterfallen".
     * Dieser Vorgang ist vollständig invertierbar (einfach soweit links shiften bis wieder in ursprünglicher Form,
     * vgl. invShiftColumns)
     * @param s interner Zustand vor dem Linksshift
     */
    void shiftRows(vector<byte>& s);
    /**
     * Es wird der interne Zustand spaltenweise transformiert. Dabei wird jede Spalte als Vektor
     * interpretiert und mit einer 4x4 Matrix multipliziert. Jedes Element pro Spalte wird zeilenweise
     * als Polynom über GF(2^8) interpretiert und mit statischen Werten multipliziert.
     * @param s interner Zustand
     */
    void mixColumn(vector<byte>&  s);

    /**
     * Ruft die benötigten Funktionen abhängig von der Runde i auf um den State s zu verarbeiten.
     * @param i Aktuelle Runde
     * @param s State (aktuell zu verarbeitender Block)
     */
    void invRound(int i, vector<byte>&  s);

    /**
     * Ruft die benötigten Funktionen der finalen Runde um den State s zu verarbeiten.
     * Dabei wird im Gegensatz zu einer "normalen" Runde die InvMixColumns Funktion nicht aufgerufen.
     * @param i Aktuelle Runde
     * @param s State (aktuell zu verarbeitender Block)
     */
    void invFinalRound(int i, vector<byte>&  s);

    /**
     * Die invertierte ByteSub Funktion wendet auf den internen Zustand die umgekehrte S-Box
     * an (reverse s-box)
     * @param s interner Zustand
     */
    void invByteSub(vector<byte>&  s);
    /**
     * Die invertierte ShiftRows Funktion schiebt den internen Zustand zurück zum Ursprung.
     * Während das shiften genau wie in ShiftRows funktioniert, reicht es in der invertierten
     * Funktion einzig zu shiftenden Entfernungen neu zu definieren
     * @param s interner Zustand
     */
    void invShiftRows(vector<byte>& s);
    /**
     * Vgl. MixColumns. In der invertierten MixColumns Funktion wird statt das GF zu
     * multiplizieren, feste Werte (vgl. Folie 31/40) multipliziert, um die Operation rückgängig zu machen.
     * @param s
     */
    void invMixColumn(vector<byte>&  s);

    /**
     * rcon Funktion. Gibt den für die Funktion F benötigten Wert zurück.
     * @param i Aktuelle Runde der Schlüssel Expansion
     * @return der errechnete Wert
     */
    u_int8_t rc(u_int8_t i);

    /**
     * Addition von zwei unsigned 32 Bit Integer durch die XOR Operation
     * @param a 32 Bit Integer
     * @param b 32 Bit Integer
     * @return Ergebnis 32 Bit Integer
     */
    u_int32_t vectorAddition(u_int32_t a, u_int32_t b);
    /**
     * Diese Methode erschwert die Kryptoanalyse. Auch triviale Eingaben (nur 0'en)
     * sind verschlüsselt nicht wiederzuerkennen. Dabei werden die Rundenschlüssel nocheinmal
     * miteinander verrechnet. Die Hilfsfunktion wird nur auf das erste und letzte angewendet.
     * @param i Stelle der aktuellen Runde der Expansion
     * @param x erster Vektor
     * @param y zweiter Vektor
     * @return Ergebnis
     */
    u_int32_t f(u_int8_t i, u_int32_t x, u_int32_t y);
    /**
     * Vergleiche f-Funktion.
     * @param x erster Vektor
     * @param y zweiter Vektor
     * @return Ergebnis
     */
    u_int32_t g(u_int32_t x, u_int32_t y);

    /**
     * Hilfsfunktion welche einen 32 Bit Integer a in den Byte Vektor b an der Position pos.
     * @param a 32 Bit Integer welcher in den Byte Vektor b gespeichert werden soll
     * @param b Byte Vektor
     * @param pos Position an der a gespeichert werden soll
     */
    void UINT32toBYTEVECTOR(u_int32_t a, vector<byte>& b, int pos);

    /**
     * Hilfsfunktion welche 4 8 Bit Integer in dem Byte Vektor b an der Position pos zu einem 32 Bit Integer konkateniert.
     * @param b Byte Vektor aus dem der 32 Bit Integer ausgelesen werden soll
     * @param pos Position im Vektor b
     * @return der konkatenierte 32 Bit Integer
     */
    u_int32_t BYTEVECTORtoUINT32(vector<byte> b, int pos);
    /**
     * Erstellt Rundenschlüssel. Zuerst wird ein Vektor der richtigen Größe (4x4) erstellt,
     * und mit dem Schlüssel aufgefüllt.
     * @param round Gibt Runde an
     * @return neuer Rundenschlüssel
     */
    vector<byte> getRoundKey(int round);
    /**
     * Debug Ausgabe des aktuellen States
     * @param s interner Zustand
     */
    void printSBlock(vector<byte> s);
    /**
     * Erzeuge s-box dynamisch statt statische S-Box zu benutzen.
     * @param number Stelle in S-Box
     * @return Stelle in S-Box
     */
    u_int8_t calculateSboxValue(u_int8_t number);
    /**
     * Hilfsfunktion um S-Box zu füllen
     */
    void generateSBox();

    /**
     * nk Länge des Schlüssels (128, 192 oder 256 Bit)
     * nR Rundenzahl
     * nB Anzahl der Spalten (Blocklänge) ist im AES Standard immer 128bit
     */
    int nK, nR, nB;

    /**
     * Vektor für alle Rundenschlüssel
     */
    vector<byte> kR;

    /**
     * Hilfsfelder um die S-Box abzuspeichern
     */
    u_int8_t sBox[256];
    u_int8_t rsBox[256];

    int kRSize[3][3] = {{10, 12, 14},{12, 12, 14},{14, 14, 14}};

};


#endif //PRAKTIKUMAES_AES_H
