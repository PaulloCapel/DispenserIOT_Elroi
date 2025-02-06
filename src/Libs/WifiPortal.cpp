#include <WifiPortal.h>


void WifiPortal::setPreferences(Preferences &prefs)
{
    this->preferences = &prefs; // Armazena o endereço
};

// modo de inicialização em modo ap
void WifiPortal::ApMode()
{

    // Array para armazenar o endereço MAC
    uint8_t baseMac[6];
    // Lê o MAC Address da interface STA
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    // Cria uma String para armazenar o MAC Address formatado
    String macAddress = "";
    // Concatena os bytes em formato "XX:XX:XX:XX:XX:XX"
    for (int i = 0; i < 6; i++)
    {
        if (i > 0)
        {
            macAddress += ":"; // Adiciona ":" entre os bytes
        }
        macAddress += String(baseMac[i], HEX); // Converte o byte para HEX
    }
    // Converte para maiúsculas
    macAddress.toUpperCase();

    String AP_Name = "";

    AP_Name = "Dispenser_" + macAddress;

    debug.Println("WifiPortal.ApMode()", "AP SSID : " + AP_Name, "WARN");

    WiFi.mode(WIFI_AP);

    IPAddress ip(192, 168, 10, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPConfig(ip, ip, NMask);

    delay(250);
    WiFi.softAP(AP_Name, AP_password);
    debug.Println("WifiPortal.ApMode()", "AP Password : " + AP_password, "WARN");
    IPAddress myIP = WiFi.softAPIP();
    debug.Println("WifiPortal.ApMode()", "AP IP address: " + String(myIP), "WARN");

    // cria handle de funções
    server.on("/", [this]()
              { this->handleRoot(); });
    server.on("/save", [this]()
              { this->handleSave(); });
    server.on("/scan", [this]()
              { this->handleScan(); });

    server.begin();

    vTaskDelay(pdMS_TO_TICKS(1000));

    performScan();

    debug.Println("WifiPortal.ApMode()", "Dispenser em modo AP esperando por config ", "INFO");
};

void WifiPortal::handleRoot()
{
    String html = htmlPage; // A página inteira já está em htmlPage
    // Substitui o placeholder no HTML com a lista de redes
    html.replace("<option value=''>Nenhuma rede encontrada</option>", networksHtml);
    // Envia a resposta ao cliente
    server.send(200, "text/html", html);
};
void WifiPortal::handleScan()
{
    debug.Println("WifiPortal.handleScan()", "Solicitação de escaneamento de rede", "INFO");
    server.send(200, "application/json", networksJson);
};
void WifiPortal::handleSave()
{
    debug.Println("WifiPortal.handleSave()", "Dados recebidos do portal", "INFO");

    String ssid = server.arg("ssid");
    String password = server.arg("password");
    String modeStr = server.arg("communicationMode");

    bool mode = (modeStr == "slave");
    debug.Println("WifiPortal.handleSave()", "Metodo de comunicação : " + modeStr, "INFO");
    debug.Println("WifiPortal.handleSave()", "ssid : " + ssid, "INFO");
    debug.Println("WifiPortal.handleSave()", "password : " + password, "INFO");

    if (preferences->begin("teste", false))
    {
        preferences->putString("ssid_wifi", ssid);
        preferences->putString("password_wifi", password);
        preferences->putBool("mode", mode);
        preferences->putBool("configured", true);
        ssid = "";
        password = "";

        preferences->getString("ssid_wifi", ssid);
        preferences->getString("password_wifi", password);

        debug.Println("WifiPortal.handleSave()", "ssid : " + ssid, "INFO");
        debug.Println("WifiPortal.handleSave()", "password : " + password, "INFO");

        preferences->end();

        debug.Println("WifiPortal.handleSave()", "Dados preferences salvos", "INFO");
    }
    else
    {
        debug.Println("WifiPortal.handleSave()", "Falha ao inicializar preferences", "ERROR");
    }

    server.send(200, "text/html; charset=UTF-8", "<html><body><h2>Configurações Salvas!</h2></body></html>");
    PortalConfigurado = true;

    debug.Println("WifiPortal.handleSave()", "Dados do portal salvo na preferences", "INFO");
};

void WifiPortal::performScan()
{
    debug.Println("WifiPortal.handleSave()", "Escaneando redes wifi", "WARN");
    int n = WiFi.scanNetworks();

    // Inicializa os resultados
    networksJson = "{\"networks\":[";
    networksHtml = "";

    bool firstEntry = true;

    for (int i = 0; i < n; i++)
    {
        int signalStrength = WiFi.RSSI(i);

        // Ignora sinais abaixo de -70 dBm
        if (signalStrength < -80)
        {
            continue;
        }

        // Classifica o sinal
        String signalClass = classifySignal(signalStrength);

        // Adiciona ao JSON
        if (!firstEntry)
        {
            networksJson += ",";
        }
        firstEntry = false;

        networksJson += "{\"ssid\":\"" + String(WiFi.SSID(i)) + "\",\"signal\":\"" + signalClass + "\"}";

        // Adiciona ao HTML
        networksHtml += "<option value='" + String(WiFi.SSID(i)) + "'>" +
                        String(WiFi.SSID(i)) + " (" + signalClass + ")</option>";
    }

    networksJson += "]}";
    debug.Println("WifiPortal.handleSave()", "Redes escaneadas", "WARN");
    debug.Println("WifiPortal.handleSave()", networksJson, "WARN");

    // Caso nenhuma rede seja válida
    if (networksHtml.isEmpty())
    {
        networksHtml = "<option value=''>Nenhuma rede encontrada</option>";
    }
};

String WifiPortal::classifySignal(int dBm)
{
    if (dBm >= -40)
    {
        return "Excelente";
    }
    else if (dBm >= -60)
    {
        return "Bom";
    }
    else if (dBm >= -70)
    {
        return "Fraco";
    }
    else if (dBm >= -80)
    {
        return "Ruim";
    }
    else
    {
        return ""; // Não deve ser usada, pois sinal abaixo de -70 será filtrado
    }
};

bool WifiPortal::HandleClient()
{
    server.handleClient();

    return PortalConfigurado;
};