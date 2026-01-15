#include "ControladorSistema.h"

ControladorSistema::ControladorSistema(byte pinSelector)
  : nfc(34, 26,&Wire), sd(5), buzzer(32), selector(pinSelector),
    kiosko(lcd, nfc, sd, reloj, buzzer, teclado),
    cobro(lcd, nfc, sd, reloj, buzzer),
    modoActual(nullptr) {}

void ControladorSistema::iniciar() {
  lcd.iniciar();
  buzzer.iniciar();
  reloj.iniciar();
  sd.iniciar(reloj);
  nfc.iniciar();
}

void ControladorSistema::ejecutar() {
  bool esKiosko = selector.esKiosko();
  Modo* nuevoModo = nullptr;
  if (esKiosko) {
    nuevoModo = &kiosko;
  } else {
    nuevoModo = &cobro;
  }

  if (modoActual != nuevoModo) {
    if (modoActual) {
      modoActual->alSalir();
    }
    nuevoModo->alEntrar();
    modoActual = nuevoModo;
  }

  if (modoActual) {
    modoActual->ejecutar();
  }
}