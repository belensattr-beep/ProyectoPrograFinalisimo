#include "SistemaTransporte.h"

//LCD
PantallaLCD::PantallaLCD(): lcd(0x27, 16, 2) {}
void PantallaLCD::iniciar() {
  lcd.init();
  delay(50);
  lcd.backlight();
  lcd.clear();
}
void PantallaLCD::mensaje(const String& l1, const String& l2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(l1);
  lcd.setCursor(0, 1);
  lcd.print(l2);
}

//Buzzer
Buzzer::Buzzer(byte p, byte c): pin(p), canal(c), iniciado(false) {}
void Buzzer::iniciar() {
  if (iniciado) return;
  ledcSetup(canal, 2000, 8);
  ledcAttachPin(pin, canal);
  ledcWrite(canal, 0);
  iniciado = true;
}
void Buzzer::ok() {
  if (!iniciado) return;
  ledcWrite(canal, 128);
  delay(120);
  ledcWrite(canal, 0);
}
void Buzzer::error() {
  if (!iniciado) return;
  for (int i = 0; i < 3; i++) {
    ledcWrite(canal, 200);
    delay(80);
    ledcWrite(canal, 0);
    delay(80);
  }
}

//Reloj
Reloj::Reloj() : iniciado(false) {}
void Reloj::iniciar() {
  if (iniciado) return;

  if (!rtc.begin()) {
    Serial.println("ERROR RTC");
    return;
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  iniciado = true;
}
void Reloj::ahora(String& fecha, String& hora) {
  if (!iniciado) {
    fecha = "";
    hora  = "";
    return;
  }
  DateTime t = rtc.now();
  char f[11];  // YYYY-MM-DD
  char h[9];   // HH:MM
  sprintf(f, "%04d-%02d-%02d", t.year(), t.month(), t.day());
  sprintf(h, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
  fecha = f;
  hora  = h;
}
DateTime Reloj::ahoraDT() {
  if (!iniciado) {
    return DateTime(2000, 1, 1, 0, 0, 0);
  }
  return rtc.now();
}

//Tarjeta
Tarjeta::Tarjeta(const String& id, float s)
  : uid(id), saldo(s) {}

Tarjeta::~Tarjeta() {}

String Tarjeta::getUID() const { return uid; }
float Tarjeta::getSaldo() const { return saldo; }

void Tarjeta::recargar(float monto) { saldo += monto; }

bool Tarjeta::cobrar(float monto) {
  if (saldo < monto) return false;
  saldo -= monto;
  return true;
}

//Tarjetas hijas
TarjetaComun::TarjetaComun(const String& id, float s)
  : Tarjeta(id, s) {}
String TarjetaComun::tipo() const { return "COMUN"; }

TarjetaEstudiante::TarjetaEstudiante(const String& id, float s)
  : Tarjeta(id, s) {}
String TarjetaEstudiante::tipo() const { return "ESTUDIANTE"; }

TarjetaAdultoMayor::TarjetaAdultoMayor(const String& id, float s)
  : Tarjeta(id, s) {}
String TarjetaAdultoMayor::tipo() const { return "ADULTO_MAYOR"; }

//SD
AlmacenSD* AlmacenSD::instancia = nullptr;
AlmacenSD::AlmacenSD(byte pinCS): cs(pinCS), iniciado(false), reloj(nullptr) { instancia = this;}
void AlmacenSD::dateTimeCallback(uint16_t* date, uint16_t* time) {
  if (!instancia || !instancia->reloj) return;

  DateTime t = instancia->reloj->ahoraDT(); 

  *date = FAT_DATE(t.year(), t.month(), t.day());
  *time = FAT_TIME(t.hour(), t.minute(), t.second());
}
void AlmacenSD::iniciar(Reloj &r) {
  if (iniciado) return;
  reloj = &r;
  SdFile::dateTimeCallback(dateTimeCallback);
  SPI.begin(18, 19, 23, cs);
  if (!sd.begin(cs, SD_SCK_MHZ(4))) {
    Serial.println("ERROR SD");
    return;
  }
  if (!sd.exists("/tarjetas.txt")) {
    File f = sd.open("/tarjetas.txt", FILE_WRITE);
    f.close();
  }
  if (!sd.exists("/historial.txt")) {
    File f = sd.open("/historial.txt", FILE_WRITE);
    f.close();
  }
  iniciado = true;
}
Tarjeta* AlmacenSD::cargarTarjeta(const String& uid) {
  if (!iniciado) return nullptr;
  File f = sd.open("/tarjetas.txt", FILE_READ);
  if (!f) return nullptr;
  while (f.available()) {
    String linea = f.readStringUntil('\n');
    linea.trim();
    int p1 = linea.indexOf(',');
    int p2 = linea.lastIndexOf(',');
    if (p1 < 0 || p2 < 0) continue;
    if (linea.substring(0, p1) != uid) continue;
    String tipo = linea.substring(p1 + 1, p2);
    float saldo = linea.substring(p2 + 1).toFloat();
    f.close();

    if (tipo == "ESTUDIANTE") return new TarjetaEstudiante(uid, saldo);
    if (tipo == "ADULTO_MAYOR") return new TarjetaAdultoMayor(uid, saldo);
    return new TarjetaComun(uid, saldo);
  }
  f.close();
  return nullptr;
}
void AlmacenSD::guardarTarjeta(Tarjeta* t) {
  if (!iniciado) return;
  File fin = sd.open("/tarjetas.txt", FILE_READ);
  File fout = sd.open("/tmp.txt", FILE_WRITE);
  while (fin.available()) {
    String linea = fin.readStringUntil('\n');
    linea.trim();
    if (!linea.startsWith(t->getUID() + ",")) {
      fout.println(linea);
    }
  }
  fout.println(t->getUID() + "," + t->tipo() + "," + String(t->getSaldo(), 2));
  fin.close();
  fout.close();
  sd.remove("/tarjetas.txt");
  sd.rename("/tmp.txt", "/tarjetas.txt");
}
void AlmacenSD::guardarOperacion(const String& linea) {
  if (!iniciado) return;
  File log = sd.open("/historial.txt", FILE_WRITE);
  log.println(linea);
  log.close();
  Serial.println(linea);
}

//NFC
LectorNFC::LectorNFC(byte IRQ, byte RST,TwoWire *theWire): irq(IRQ), rst(RST), nfc(IRQ, RST, theWire), iniciado(false), ultimoUID("") {}
void LectorNFC::iniciar() {
  if (iniciado) return;
  if (rst <= 33) {
    pinMode(rst, OUTPUT);
    digitalWrite(rst, HIGH);
  }
  nfc.begin();
  nfc.SAMConfig();
  iniciado = true;
}
bool LectorNFC::leerUID(String& uid) {
  if (!iniciado) return false;
  uint8_t id[7];
  uint8_t len;
  if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A,id,&len,50)){
    ultimoUID = "";
    return false;
  }
  uid = "";
  for (byte i = 0; i < len; i++) {
    if (id[i] < 0x10) uid += "0";
    uid += String(id[i], HEX);
  }
  uid.toUpperCase();
  if (uid == ultimoUID) return false;
  ultimoUID = uid;
  return true;
}
void LectorNFC::reset(){
  ultimoUID = "";
}

//Keypad
Teclado::Teclado() : keypad(makeKeymap(teclas), pinesFilas, pinesCols, FILAS, COLS), buffer("") {}
bool Teclado::leerMontoNoBloqueante(float &monto, PantallaLCD &lcd){
  char k = keypad.getKey();
  if (!k) return false;
  if (k >= '0' && k <= '9') {
    buffer += k;
    lcd.mensaje("Monto:", buffer);
  }
  else if (k == '*') {
    buffer = "";
    lcd.mensaje("Monto:", buffer);
  }
  else if (k == '#') {
    if (buffer.length() > 0) {
      monto = buffer.toFloat();
      buffer = "";
      return true;
    }
  }
  return false;
}
bool Teclado::leerTipoNoBloqueante(char& tipo) {
  char k = keypad.getKey();
  if (!k) return false;
  if (k == '1' || k == '2' || k == '3') {
    tipo = k;
    return true;
  }
  return false;
}

//Switch
Switch::Switch(byte p) : pin(p) {
  pinMode(pin, INPUT_PULLUP);
}

bool Switch::esKiosko() const {
  return digitalRead(pin) == HIGH;
}
