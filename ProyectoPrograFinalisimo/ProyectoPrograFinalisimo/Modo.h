#ifndef MODO_H
#define MODO_H
#include "SistemaTransporte.h"
class Modo {
protected:
  PantallaLCD &lcd;
  LectorNFC &nfc;
  AlmacenSD &sd;
  Reloj &reloj;
  Buzzer &buzzer;
public:
  Modo(PantallaLCD &l, LectorNFC &n, AlmacenSD &s, Reloj &r, Buzzer &b) : lcd(l), nfc(n), sd(s), reloj(r), buzzer(b) {}
  virtual void ejecutar() = 0;
  virtual void alEntrar() = 0;
  virtual void alSalir() { nfc.reset();}
  virtual ~Modo() {}
};

#endif
