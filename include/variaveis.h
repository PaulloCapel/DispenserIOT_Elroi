#ifndef Variaveis_h
#define Variaveis_h
#include <main.h>

// task handles

TaskHandle_t
    xTask_StatusLedHandle,
    xTask_ControlDispenserHandle,
    xTask_DataControl,
    xTask_SelectComunicationModeHandle,
    xTask_ModeMasterHandle,
    xTask_ModeSlaveHandle;

EventGroupHandle_t
    xEventGroupStatusHandle;

SemaphoreHandle_t
    xMutexMyDataHandle;
//
QueueHandle_t
    xQueueAppToMydata,
    xQueueMyDataToServer;

struct CardRFID
{
    uint8_t succes;
    uint8_t uidlength;
    uint8_t uid_uint8_t[7];
    uint8_t uid_datablock4[16];
    uint32_t uid_uint32_t;
};

struct ApiStatus{
    bool req;
    uint32_t timestamp;
};

bool AtualizaTime = false;

const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDfDCCAwKgAwIBAgISA5UU35gvfosubNvLTyXmvM/DMAoGCCqGSM49BAMDMDIx\n" \
"CzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQDEwJF\n" \
"NTAeFw0yNDEyMjExNzU2MDRaFw0yNTAzMjExNzU2MDNaMBgxFjAUBgNVBAMTDWJh\n" \
"Y3Byby5jb20uYnIwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARKNvpcd7YcnZt4\n" \
"V18mK3cBfozlbmiGg2HL5nDHqwPup63N5+1hE73bxgd1ucoE8r9jilvekS1Oel89\n" \
"a7JQy+Ybo4ICEDCCAgwwDgYDVR0PAQH/BAQDAgeAMB0GA1UdJQQWMBQGCCsGAQUF\n" \
"BwMBBggrBgEFBQcDAjAMBgNVHRMBAf8EAjAAMB0GA1UdDgQWBBSBtk6jgbHYiVuW\n" \
"KVt9bVulg7n4tDAfBgNVHSMEGDAWgBSfK1/PPCFPnQS37SssxMZwi9LXDTBVBggr\n" \
"BgEFBQcBAQRJMEcwIQYIKwYBBQUHMAGGFWh0dHA6Ly9lNS5vLmxlbmNyLm9yZzAi\n" \
"BggrBgEFBQcwAoYWaHR0cDovL2U1LmkubGVuY3Iub3JnLzAYBgNVHREEETAPgg1i\n" \
"YWNwcm8uY29tLmJyMBMGA1UdIAQMMAowCAYGZ4EMAQIBMIIBBQYKKwYBBAHWeQIE\n" \
"AgSB9gSB8wDxAHYAzPsPaoVxCWX+lZtTzumyfCLphVwNl422qX5UwP5MDbAAAAGT\n" \
"6pJC9AAABAMARzBFAiEAwslRydMeKXX4NHmZnkS4vU6hmPRyNFUCSu75UXAsI0wC\n" \
"IAqJE4CfgXy4q//D+jNSTBIYBx55Gs8tk73watAzAedNAHcAE0rfGrWYQgl4DG/v\n" \
"THqRpBa3I0nOWFdq367ap8Kr4CIAAAGT6pJDpQAABAMASDBGAiEAmjC7CqEWrqMg\n" \
"t2vEVpo+50bBZcJaowBQ8x6QmQ4pA9QCIQDxzzBJdBa5yw4lVFxSl8o5G1woHE/w\n" \
"zvwF5EyNAwzXyjAKBggqhkjOPQQDAwNoADBlAjAN19Kn1YOh4G9bJLnKfa91pwV6\n" \
"tj128OrtNPWb1DjEj4Kht8eNSOGiNUSDc+mwWgICMQC73ecdSPmwdynOweja7gZV\n" \
"GRoZFn6PkzYOkse7PuDj1tDmg06wznW7cl2tTrBipSg=\n" \
"-----END CERTIFICATE-----\n";




// event group bits

#define xEvG_ClockLedVM1Hz 0x01    // 0000 0001 // rename when fuction is defined
#define xEvG_ClockLedVD1Hz 0x02    // 0000 0010 // rename when fuction is defined
#define xEvG_ClockLedVM_VD1Hz 0x04 // 0000 0100 // rename when fuction is defined
#define xEvG_OnLedVM 0x08          // 0000 1000 // rename when fuction is defined
#define xEvG_OnLedVM_VD 0x10       // 0001 0000 // rename when fuction is defined
#define xEvG_OffLedVM_VD 0x20      // 0010 0000 // rename when fuction is defined
#define xEvG_OnLedEsp32 0x40       // 0100 0000 // rename when fuction is defined
#define xEvG_OffLedEsp32 0x80      // 1000 0000 // rename when fuction is defined

#endif