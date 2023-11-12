// Kutubxonalarni yuklab olamiz
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);


// WI FI SSID va Paroli
const char* ssid = "SSID";
const char* password = "Parol";

#define DHTPIN 15     // DHT PIN
#define DHTTYPE    DHT11     // DHT TYPE
DHT dht(DHTPIN, DHTTYPE);

AsyncWebServer server(80);
String readTuproqNamlik(){
  int namlik= analogRead(32);   
  namlik = map(namlik, 1060, 2800, 100, 0);
  if(namlik>100){
    namlik = 100;
  }
  if (namlik < 0){
    namlik=0;
  }   
  if (isnan(namlik)) {    
    Serial.println("Namlik sensor Xatosi!");
    return "--";
  }
  else {
    Serial.println(namlik);
    return String(namlik);
  }
}


String readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous" color="#20d309">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2 style="color:#20d309, font-family:cooper black">D.B.AGRO</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-solid fa-wind" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i>  
    <span class="dht-labels">Tuproq namligi</span>
    <span id="tuproqnamlik">%TUPROQNAMLIK%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <span class="dht-labels">Nasos holati</span>
    <span id="fotorezistor">O'chgan</span>
    
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("tuproqnamlik").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/tuproqnamlik", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("fotorezistor").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/fotorezistor", true);
  xhttp.send();
}, 1000 ) ;

</script>
</html>)rawliteral";

String processor(const String& var){
  if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  else if(var == "TUPROQNAMLIK"){
    return readTuproqNamlik();
  }
  return String();
}



void setup(){
  // Serial Portga ulanamiz
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  pinMode(35,INPUT);
  pinMode(32,INPUT);
  pinMode(27, OUTPUT);
  dht.begin();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());



  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
  server.on("/tuproqnamlik", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readTuproqNamlik().c_str());
  });
  server.begin();
}
 
unsigned long lStepLcd = 0, rStepLcd = 0, lMLcd = 0, currentTime = 0;

void loop(){ 
  float h =  dht.readHumidity();
  float t = dht.readTemperature();
  int namlik= analogRead(32);   
     // tuproq namligi sensori
  namlik = map(namlik, 1060, 2800, 100, 0);
  if(namlik>100){
    namlik = 100;
  }
  if (namlik < 0){
    namlik=0;
  }   
  
  currentTime = millis();
  rStepLcd = currentTime / 1000;
  // Serial.println(currentTime);
  if((rStepLcd - lStepLcd) % 2 && lMLcd != rStepLcd){
    lcd.setCursor(0,0);   
    lcd.print("Havo namligi:");
    lcd.print(h);
    lcd.print("%           ");
    
    lcd.setCursor(0,1);  
    lcd.print("IP: ");
    lcd.print(WiFi.localIP());

    lMLcd = rStepLcd;
    Serial.println("111");
  }
  
  if((rStepLcd - lStepLcd) % 2 == 0 && rStepLcd != lStepLcd){
    lcd.setCursor(0,0);   
    lcd.print("Harorat: ");
    lcd.print(t);
    lcd.print("C          ");
    
    lcd.setCursor(0,1); 
    lcd.print("Namlik T. : ");
    lcd.print(namlik);
    lcd.print("%           ");
    lStepLcd = rStepLcd;
    Serial.println("222");
  }
}
