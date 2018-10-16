//
// Created by simon on 03.06.18.
//

#ifndef PRAKTIKUMAES_GF256_H
#define PRAKTIKUMAES_GF256_H


#include <sys/param.h>


class GF256 {
public:
    /**
     * Konstruktor ruft init Funktion auf
     * @param value übergebener Wert an init()
     */
    GF256(u_int8_t value);
    /**
     * Standard Destruktor
     */
    ~GF256();

    void init(u_int8_t g);

    GF256 inverse();
    /**
     * Addition über das Galois Feld. Dabei wird this mit other ge-XOR
     * @param other zweiter Parameter der Addition
     * @return Ergebnis der Addition
     */
    GF256 operator+( GF256 other );
    /**
     * Subtraktion über das Galois Feld. Dabei wird this mit other ge-XOR
     * @param other zweiter Pramater der Subtraktion
     * @return Ergebnis der Subtraktion
     */
    GF256 operator-( GF256 other );
    /**
     * Beim Multiplizieren muss darauf geachtet werden dass, das Ergebnis
     * kein Element von GF sein kann. Man reduziert das Ergbnis indem
     * man modulo einem irreduziblem Polynom mit Grad k rechnet (-> Polynom-
     * division, shift dann xor)
     * @param other
     * @return
     */
    GF256 operator*( GF256 other );
    /**
     * Gibt aktuellen Wert vom gefragten Objekt zurück
     * @return value
     */
    u_int8_t getValue() const;
    /**
     * Gibt Exponenten Tabelle zurück
     * @return
     */
    const u_int8_t *getExpTable() const;
    /**
     * Gibt Logarithmus Tabelle zurück
     * @return
     */
    const u_int8_t *getLogTable() const;

private:
    /**
     * Interna des Galois Feld
     * Die Russische Multiplikation kann nicht vollständig ersetzt werden,
     * da der überladende Operator (*) auf den Konstruktor mit value angewiesen ist,
     * aber noch nicht definiert wurde. Deswegen brauchen wir eine zweite
     * Funktion zur Multiplikation, unabhängig von this->value
     */
    u_int8_t expTable[256], logTable[256];
    u_int8_t value;

    u_int8_t russianMultiplication(u_int8_t multiplicandOne, u_int8_t multiplicandTwo);


};


#endif //PRAKTIKUMAES_GF256_H