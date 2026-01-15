#ifndef SISTEMA_TRANSPORTE_H
#define SISTEMA_TRANSPORTE_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_PN532.h>
#include <Keypad.h>

class PantallaLCD {
private:
  LiquidCrystal_I2C lcd;
public:
  PantallaLCD();
  void iniciar();
  void mensaje(const String& l1, const String& l2 = "");
};

class Buzzer {
private:
  byte pin;
  byte canal;
  bool iniciado;
public:
  Buzzer(byte p, byte c = 0);
  void iniciar();
  void  ok();
  void error();
};

class Reloj {
private:
  RTC_DS3231 rtc;
  bool iniciado; 
public:
  Reloj();
  void iniciar();
  void ahora(String& fecha, String& hora);
  DateTime ahoraDT();
};

class Tarjeta {
protected:
  String uid;
  float saldo;
public:
  Tarjeta(const String& id, float s);
  virtual ~Tarjeta();
  String getUID() const;
  float getSaldo() const;
  void recargar(float monto);
  bool cobrar(float monto);
  virtual String tipo() const = 0;
};

class TarjetaComun : public Tarjeta {
public:
  TarjetaComun(const String& id, float s);
  String tipo() const override;
};

class TarjetaEstudiante : public Tarjeta {
public:
  TarjetaEstudiante(const String& id, float s);
  String tipo() const override;
};

class TarjetaAdultoMayor : public Tarjeta {
public:
  TarjetaAdultoMayor(const String& id, float s);
  String tipo() const override;
};

class AlmacenSD {
private:
  byte cs;
  bool iniciado;
  SdFat sd;
  Reloj* reloj;
  static AlmacenSD* instancia;
  static void dateTimeCallback(uint16_t *date, uint16_t *time);
public:
  AlmacenSD(byte pinCS);
  void iniciar(Reloj &reloj);
  Tarjeta* cargarTarjeta(const String& uid);
  void guardarTarjeta(Tarjeta* t);
  void guardarOperacion(const String& linea);
};

class LectorNFC {
private:
  byte irq;
  byte rst;
  Adafruit_PN532 nfc;
  bool iniciado;
  String ultimoUID;
public:
  LectorNFC(byte IRQ, byte RST, TwoWire *theWire = &Wire);
  void iniciar();
  bool leerUID(String& uid);
  void reset();
};

class Teclado {
private:
  static const byte FILAS = 4;
  static const byte COLS  = 4;
  String buffer;
  char teclas[FILAS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };
  byte pinesFilas[FILAS] = {4, 12, 13, 14};
  byte pinesCols[COLS]  = {16, 17, 25, 15};
  Keypad keypad;
public:
  Teclado();
  bool leerMontoNoBloqueante(float& monto, PantallaLCD& lcd);
  bool leerTipoNoBloqueante(char& tipo);
};

class Switch {
private:
  byte pin;

public:
  Switch(byte p);
  bool esKiosko() const;
};

#endif
