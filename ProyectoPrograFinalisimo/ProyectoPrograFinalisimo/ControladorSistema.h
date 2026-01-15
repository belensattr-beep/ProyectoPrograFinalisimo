#ifndef CONTROLADOR_SISTEMA_H
#define CONTROLADOR_SISTEMA_H

#include "ModoKiosko.h"
#include "ModoCobro.h"

class ControladorSistema {
private:
  PantallaLCD lcd;
  Buzzer     buzzer;
  Reloj      reloj;
  AlmacenSD  sd;
  LectorNFC  nfc;
  Teclado    teclado;
  Switch selector;

  ModoKiosko kiosko;
  ModoCobro  cobro;
  Modo* modoActual;
public:
  ControladorSistema(byte pinSelector);
  void iniciar();
  void ejecutar();
};

#endif