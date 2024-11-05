// web_page.h
#ifndef WEB_PAGE_H
#define WEB_PAGE_H

const char* htmlPage = R"rawliteral(
<html>
<head>
    <title>Config Portal</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background-color: #f4f4f4;
        }
        h2 {
            color: #333;
        }
        form {
            margin-top: 20px;
        }
        input[type="text"], input[type="password"], select {
            width: 100%;
            padding: 10px;
            margin: 5px 0;
            border: 1px solid #ccc;
            border-radius: 5px;
        }
        input[type="submit"] {
            background-color: #4CAF50;
            color: white;
            padding: 10px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }
        input[type="submit"]:hover {
            background-color: #45a049;
        }
    </style>
</head>
<body>
    <h2>Escolha sua rede Wi-Fi</h2>
    <form action='/save' method='POST'>
        Rede Wi-Fi: <select name='ssid'>)rawliteral";

const char* htmlEnd = R"rawliteral(
        </select><br>
        Senha: <input type='password' name='password' required><br>
        Porta: <input type='text' name='porta' required><br>
        ID do Hardware: <input type='text' name='hwid' required><br>
        <input type='submit' value='Salvar'>
    </form>
</body>
</html>
)rawliteral";

#endif // WEB_PAGE_H
