#define WIFI_SSID "RoboCode"
#define WIFI_PASS "1029384756"
#define BOT_TOKEN "5899080725:AAFz87G8U2NbEVY10vmUfAG0F2jr3yx83nY"

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <DHT.h>
DHT dht(26, DHT11);

#include <FastBot.h>
FastBot bot(BOT_TOKEN);

#include<EEPROM.h>

int _min = 26;
int _max = 28;
int _kotelMode = 0; // 0-выкл, 1-поддерживаем темп. , 2-одиночный нагрев

float t = 0;
float h = 0;

bool minMode = 0;
bool maxMode = 0;

bool temper = 0;

void setup() {
  lcd.begin();
  lcd.backlight();

  Serial.begin(115200);
  dht.begin();
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() > 15000) ESP.restart();
  }

  EEPROM.begin(500);
  Serial.println("Connected");

  bot.attach(newMsg);
  pinMode(27, OUTPUT);

}




void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  bot.tick();

  delay(50);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Hum=");
  lcd.setCursor(5, 0);
  lcd.print(h);
  lcd.setCursor(1, 1);
  lcd.print("Tem=");
  lcd.setCursor(5, 1);
  lcd.print(t);

  if (_kotelMode == 0)
    digitalWrite(27, LOW);


  if (t < _min and _kotelMode == 1)
    digitalWrite(27, HIGH);


  if (t >= _max and _kotelMode == 1)
    digitalWrite(27, LOW);

  if (t < _max and _kotelMode == 2)
    digitalWrite(27, HIGH);

  if (t >= _max and _kotelMode == 2)
  {
    digitalWrite(27, LOW);
    _kotelMode = 0;
  }
}


void newMsg(FB_msg& msg) {

  Serial.println(msg.text);
  if (msg.text == "/start") {
    bot.showMenu("Температура\t авто тем\t настр тем\n  влажность\t  изменить температуру\t выкл", msg.chatID);
  }
  if (msg.text == "настр тем") {
    bot.showMenu("минимальная температура\t макс для авто тем , для один изменения\t/start", msg.chatID);
  }


  if (msg.text == "Температура")
  {

    t = dht.readTemperature();
    bot.sendMessage("температура в помещении: " + (String)t, msg.chatID);

  }
  if (msg.text == "влажность")
  {
    h = dht.readHumidity();
    bot.sendMessage("влажность в помещении: " + (String)h, msg.chatID);

  }

  if (minMode)
  {
    if (msg.text.toInt() != 0)
      _min = msg.text.toInt();

    bot.sendMessage("Теперь минимальная температура=" + String(_min), msg.chatID);
    minMode = 0;
  }

  if (msg.text == "минимальная температура")
  {
    bot.sendMessage("Отправьте минимальную температуру", msg.chatID);
    minMode = 1;
  }


  if (maxMode)
  {
    if (msg.text.toInt() != 0)
      _max = msg.text.toInt();

    bot.sendMessage("Теперь макс для авто тем , для один изменения=" + String(_max), msg.chatID);
    maxMode = 0;
  }

  if (msg.text == "макс для авто тем , для один изменения")
  {
    bot.sendMessage("Отправьте макс для авто тем , для один изменения", msg.chatID);
    maxMode = 1;
  }

  if (msg.text == "выкл")
  {
    bot.sendMessage("Котел выключен", msg.chatID);
    _kotelMode = 0;
  }


  if (msg.text == "авто тем")
  {
    bot.sendMessage("Авто температура подерживается", msg.chatID);
    _kotelMode = 1;
  }


  if (msg.text == "изменить температуру")
  {
    bot.sendMessage("Однакратное изменение температуры включено", msg.chatID);
    _kotelMode = 2;
  }
}
