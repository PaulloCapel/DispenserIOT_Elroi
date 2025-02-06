#include "myDebug.h"

myDebug::myDebug(bool debugMode) : DEBUG_MODE_(debugMode), debugLevel(1), isNewLine(true) {}


void myDebug::begin(int level, unsigned long baudRate) {
    debugLevel = constrain(level, 1, 3); // Limita o nível de debug entre 1 e 3
    if (DEBUG_MODE_) {
        Serial.begin(baudRate);
        Serial.println("[DEBUG] Debug mode ativado.");
    }
}

void myDebug::Print(const String& functionName, const String& message, const String& type) {
    if (shouldPrint(type)) {
        if (isNewLine) {
            currentHeader = buildOutput(functionName, "", type);
            Serial.print(currentHeader); // Imprime o cabeçalho apenas uma vez
            isNewLine = false;
        }
        Serial.print(message); // Adiciona a mensagem sem quebrar a linha
    }
}


void myDebug::Println(const String& functionName, const String& message, const String& type) {
    if (shouldPrint(type)) {
        if (isNewLine) {
            currentHeader = buildOutput(functionName, "", type);
            Serial.print(currentHeader); // Imprime o cabeçalho apenas uma vez
        }
        Serial.println(message); // Imprime a mensagem com quebra de linha
        isNewLine = true;        // Marca que a próxima chamada será uma nova linha
    }
}


void myDebug::setDebugMode(bool debugMode) {
    DEBUG_MODE_ = debugMode;
}

void myDebug::setDebugLevel(int level) {
    debugLevel = constrain(level, 1, 3);
}

String myDebug::buildOutput(const String& functionName, const String& message, const String& type) {
    unsigned long timestamp = millis(); // Timestamp atual em milissegundos
    return "[" + String(timestamp) + "] - [" + functionName + "] - [" + type + "] : " + message;
}

bool myDebug::shouldPrint(const String& type) {
    if (!DEBUG_MODE_) return false;

    if (type == "ERROR") return debugLevel >= 1;
    if (type == "WARN") return debugLevel >= 2;
    if (type == "INFO") return debugLevel >= 3;

    return false; // Tipo desconhecido, não imprime
}
