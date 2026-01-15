#include "ModoCobro.h"
ModoCobro::ModoCobro(PantallaLCD& l, LectorNFC& n, AlmacenSD& s, Reloj& r, Buzzer& b) : Modo(l, n, s, r, b) , estado(ESPERANDO_TARJETA), tMensaje(0) {}

float ModoCobro::tarifaPara(const Tarjeta* t) const {
  if (t->tipo() == "ESTUDIANTE")   return 1.00;
  if (t->tipo() == "ADULTO_MAYOR") return 1.50;
  return 2.30;
}

void ModoCobro::alEntrar() {
  estado = ESPERANDO_TARJETA;
  lcd.mensaje("MODO COBRO", "Acerque tarjeta");
  nfc.reset();
}

void ModoCobro::ejecutar() {
  if (estado == MOSTRANDO_OK || estado == MOSTRANDO_ERROR) {
    if (millis() - tMensaje >= 2500) {
      alEntrar();
    }
    return;
  }

  if (estado == ESPERANDO_TARJETA) {
    String uid;
    if (!nfc.leerUID(uid)) {
      return;
    }

    Tarjeta* t = sd.cargarTarjeta(uid);
    if (!t) {
      buzzer.error();
      lcd.mensaje("ERROR", "No registrada");
      estado = MOSTRANDO_ERROR;
      tMensaje = millis();
      return;
    }

    float tarifa = tarifaPara(t);
    if (!t->cobrar(tarifa)) {
      buzzer.error();
      lcd.mensaje("SALDO INSUF.", "Saldo: " + String(t->getSaldo(), 2));
      delete t;
      estado = MOSTRANDO_ERROR;
      tMensaje = millis();
      return;
    }

    sd.guardarTarjeta(t);
    String f, h;
    reloj.ahora(f, h);
    sd.guardarOperacion(f + "," + h + "," + uid + "," + t->tipo() + ",PASAJE," + String(tarifa, 2) + "," + String(t->getSaldo(), 2));
    buzzer.ok();
    lcd.mensaje("PASAJE OK", "Saldo: " + String(t->getSaldo(), 2));
    delete t;
    estado = MOSTRANDO_OK;
    tMensaje = millis();
    nfc.reset();
  }
}