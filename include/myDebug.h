#ifndef DEBUGLIB_H
#define DEBUGLIB_H

#include <Arduino.h>

class myDebug {
public:
    explicit myDebug(bool debugMode = false);

    void begin(int level, unsigned long baudRate = 115200);
    void Print(const String& functionName, const String& message, const String& type);
    void Println(const String& functionName, const String& message, const String& type);

    // Métodos para configurar dinamicamente o modo de debug e o nível
    void setDebugMode(bool debugMode);
    void setDebugLevel(int level);

private:
    bool DEBUG_MODE_;
    int debugLevel;
    String currentHeader; // Armazena o cabeçalho atual
    bool isNewLine;       // Indica se é o início de uma nova linha

    String buildOutput(const String& functionName, const String& message, const String& type);
    bool shouldPrint(const String& type);
};

#endif // DEBUGLIB_H
