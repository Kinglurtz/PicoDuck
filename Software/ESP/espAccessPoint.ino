#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>


// Set web server port number to 80
AsyncWebServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String LED_STATE = "OFF";
String led2State = "off";

// Assign output variables to GPIO pins
const int led1 = 5;
const int led2 = 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

String textInput = "Enter your code here. Each line is a key stroke. To combine multiple key strokes use the '+'. Example: CTRL + C";
String buttonText = "OFF";

const char* textID = "picoBeeTextArea";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html> 
  <head>
  <title>Pico USB</title>
    <meta charset="utf-8" name="viewport" content="width=device-width, initial-scale=1"> <link rel="icon" href="data:,"> 
    <style>html { font-family: Georgia, 'Times New Roman', Times, serif; display: inline-block; margin: 0px auto; text-align: center; background-color: rgb(58, 58, 58);} 
    .button { background-color: #195B6A; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; border-radius: 10px;} 
    table, th, td{
      border: 1px solid azure;
      color: azure;
    }
    </style>
  </head>
  <body>
    <h1 style="color:azure">ESP8266 Web Server(Bad Bee)</h1>
    <p><label for="picoBeeTextArea" style="color:azure">Bad USB interaction Page</label></p>
    <form action="/get">
    <textarea id="picoBeeTextArea" name="picoBeeTextArea" rows="15" cols="50" style="border-radius: 10px; min-width: 10%; max-width: 75%;">
%TextBox% 
    </textarea>
    <br>
    <input type="submit" value="Submit">
    </form>
    <p style="color:azure">Click the buttons to load example scripts</p>
    <p><button id="exampleButton1" onclick="LoadExamples(this.id)" class="button">Example 1</button></p>
    <p><button id="button2" onclick="LoadExamples(this.id)" class="button">Example 2</button></p>
  <table style="width: 75%; margin-left: auto; margin-right: auto; table-layout: fixed;">
    <tr>
        <th>Script</th>
        <th>Description</th>
    </tr>
    <tr>
      <td>WINDOWS</td>
      <td>The windows key ⊞</td>
    </tr>
    <tr>
      <td>SHIFT</td>
      <td>The shift key</td>
    </tr>
    <tr>
      <td>ALT</td>
      <td>The ALT key</td>
    </tr>
       <tr>
      <td>CTRL</td>
      <td>The CTRL key</td>
    </tr>
    <tr>
      <td>DOWN</td>
      <td>The down arrow key</td>
    </tr>
    <tr>
      <td>UP</td>
      <td>The up arrow key</td>
    </tr>
    <tr>
      <td>LEFT</td>
      <td>The left arrow key</td>
    </tr>
    <tr>
      <td>RIGHT</td>
      <td>The right arrow key</td>
    </tr>
    <tr>
      <td>CAPSLOCK</td>
      <td>The Caps Lock key</td>
    </tr>
    <tr>
      <td>DELETE</td>
      <td>The Delete key</td>
    </tr>
    <tr>
      <td>ESC</td>
      <td>The escape key</td>
    </tr>
    <tr>
      <td>HOME</td>
      <td>The Home key</td>
    </tr>
    <tr>
      <td>PRINTSCREEN</td>
      <td>The Print Screen key</td>
    </tr>
    <tr>
      <td>DELETE</td>
      <td>The Delete Key</td>
    </tr>
    <tr>
      <td>TAB</td>
      <td>The tab key</td>
    </tr>
    <tr>
      <td>BACKSPACE</td>
      <td>The backspace key</td>
    </tr>
    <tr>
      <td>SPACE</td>
      <td>The space bar</td>
    </tr>
    <tr>
      <td>A - Z</td>
      <td>A single alpabetical character A - Z</td>
    </tr>
    <tr>
      <td>0 - 9</td>
      <td>A single number key stroke 0 - 9</td>
    </tr>
    <tr>
      <td>DELAY:[time ms]</td>
      <td>Delay the execution for a set of ms. Example(100 ms delay): DELAY:100</td>
    </tr>
        <tr>
      <td>STRING <String> </td>
      <td>Attempts to type all characters until the end of the line is reached Example: STRING Hello World!</td>
    </tr>
    </table>


  <script>
  function LoadExamples(id)
  {
    if(id === 'exampleButton1')
    {
      document.getElementById("picoBeeTextArea").value = "WINDOWS\nDELAY:2000\nN\nO\nT\nE\nP\nA\nD\nENTER\nDELAY:2000\nH\nE\nL\nL\nO\nSPACE\nW\nO\nR\nL\nD\n" ;
    }
    else
    {
      document.getElementById("picoBeeTextArea").value = "WINDOWS\nDELAY:2000\nSHIFT + C\nSHIFT + M\nSHIFT + D\nDELAY:1000\nENTER\nDELAY:3000\nE\nX\nP\nL\nO\nR\nE\nR\nSPACE\nSTRING \"https://www.youtube.com/watch?v=dQw4w9WgXcQ\"\nDELAY:1000\nENTER\n" ;
    }
      
  }
  </script>
  </body>
</html>
)rawliteral";


void notFound(AsyncWebServerRequest *request) 
{
  request->send(404, "text/plain", "Not found");
}


String processor(const String& var)
{
  if(var == "TextBox")
  {
    return textInput;
  }
  return String();
}



void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP("ESPsoftAP_01", "1234567890");
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
    exit(1);
  }
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  // Set outputs to LOW
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/html", index_html, processor);
  });
  
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) 
  {
    String inputMessage;
    // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam(textID)) 
    {
    inputMessage = request->getParam(textID)->value();
    Serial.println(inputMessage.c_str());
    textInput = inputMessage.c_str();
    }
    request->send_P(200, "text/html", index_html, processor);
  });
 

  server.onNotFound(notFound);
}

void loop()
{

}
