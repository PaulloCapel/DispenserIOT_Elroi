#ifndef WIFI_PORTAL_H
#define WIFI_PORTAL_H

#include <Arduino.h>
#include <Wifi.h>
#include <WebServer.h>
#include <myDebug.h>
#include <DispenserData.h>

class WifiPortal
{
public:
    // função que recebe ponteiro da instance da função debug
    //explicit WifiPortal(myDebug &debugInstance) : debug(debugInstance) {}
    explicit WifiPortal(myDebug &debugInstance, DispenserData &dataInstance)
      : debug(debugInstance), dispenserData(dataInstance) {}
    // função que inicializa do esp em modoap e inicia o portal de configurações
    void ApMode();
    // função que espera pela resposta do usuario
    bool HandleClient();    
    // função que faz o scan da rede
    void performScan();

private:
    // instancia do server
    String networksJson; // Armazena o resultado do escaneamento em JSON
    String networksHtml; // Armazena o resultado do escaneamento em HTML

    WebServer server; // ponteiro do server
    
    String AP_password = "elroimedical";
    bool PortalConfigurado = false;
    // prototipo de função de scan de rede
    void handleScan();
    // prototipo de função que cria o HTML / portal
    void handleRoot();
    // funçãoq eu salva os dados vindo do portal
    void handleSave();

    myDebug &debug; // Referência ao objeto myDebug
    DispenserData &dispenserData; // referencia do objeto Dispenser data

    // função de classificação de intensidade de sinal
    String classifySignal(int dBm);

    // html ddo portal
    const char *htmlPage = R"rawliteral(
<html>
<head>
    <meta charset="UTF-8">
    <title>Config Portal</title>
    <style>
    body {
        font-family: Arial, sans-serif;
        margin: 0;
        padding: 20px;
        background-color: #f4f4f4;
        display: flex;
        justify-content: center;
        align-items: center;
        height: 100vh;
    }
    .container {
        width: 100%;
        max-width: 400px;
        background-color: #fff;
        padding: 20px;
        border-radius: 10px;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        text-align: center;
    }
    h2 {
        margin-bottom: 20px;
        color: #333;
    }
    form {
        display: flex;
        flex-direction: column;
        gap: 15px;
    }
    label {
        font-weight: bold;
        margin-bottom: 5px;
        text-align: left;
    }
    input[type="text"], input[type="password"], select {
        width: 100%;
        padding: 10px;
        margin: 0;
        border: 1px solid #ccc;
        border-radius: 5px;
        box-sizing: border-box;
    }
    input[type="submit"], button {
        background-color: #4CAF50;
        color: white;
        padding: 10px;
        border: none;
        border-radius: 5px;
        cursor: pointer;
    }
    input[type="submit"]:hover, button:hover {
        background-color: #45a049;
    }
    footer {
        margin-top: 20px;
        font-size: 0.9em;
        color: #777;
    }
    </style>
</head>
<body>
    <div class="container">
        <h2>Configuração Wi-Fi</h2>
        <form action='/save' method='POST'>
            <label for="ssid">Rede Wi-Fi:</label>
            <select name='ssid' id='ssid' required>
                <option value=''>Nenhuma rede encontrada</option>
            </select>
            <button type="button" onclick="scanNetworks()">Buscar Redes</button>

            <label for="password">Senha:</label>
            <input type='password' id='password' name='password' required>

            <label for="communicationMode">Modo de Comunicação:</label>
            <select name='communicationMode' id='communicationMode' required onchange="updateFieldValidation()">
                <option value='1'>Master</option>
                <option value='2'>Slave</option>
            </select>
            
            <input type='submit' value='Salvar'>
        </form>
        <footer>
            Elroi Tecnologia Hospitalar - Todos os direitos reservados
        </footer>
    </div>
    <script>
        // Função para atualizar a validação dos campos de SSID e senha
        function updateFieldValidation() {
            const mode = document.getElementById('communicationMode').value;
            const passwordField = document.getElementById('password');
            const ssidField = document.getElementById('ssid');

            if (mode === 'slave') {
                // Desabilitar e remover 'required' do SSID e senha
                ssidField.removeAttribute('required');
                ssidField.disabled = true;
                passwordField.removeAttribute('required');
                passwordField.disabled = true;
                passwordField.value = ''; // Limpar senha
            } else {
                // Habilitar e adicionar 'required' ao SSID e senha
                ssidField.setAttribute('required', '');
                ssidField.disabled = false;
                passwordField.setAttribute('required', '');
                passwordField.disabled = false;
            }
        }

        // Função para buscar redes Wi-Fi
        function scanNetworks() {
            fetch('/scan')
                .then(response => response.json())
                .then(data => {
                    const select = document.querySelector('select[name="ssid"]');
                    select.innerHTML = '';
                    data.networks.forEach(net => {
                        const opt = document.createElement('option');
                        opt.value = net.ssid;
                        opt.textContent = `${net.ssid} (Sinal: ${net.signal} dBm)`;
                        select.appendChild(opt);
                    });
                })
                .catch(error => {
                    alert("Erro ao buscar redes: " + error);
                });
        }
    </script>
</body>
</html>
)rawliteral";
};

#endif