#ifndef MODO_COBRO_H
#define MODO_COBRO_H

#include "Modo.h"
#include "SistemaTransporte.h"

class ModoCobro : public Modo {
private:
  PantallaLCD& lcd;
  LectorNFC& nfc;
  AlmacenSD& sd;
  Reloj& reloj;
  Buzzer& buzzer;

  enum Estado {ESPERANDO_TARJETA, MOSTRANDO_OK, MOSTRANDO_ERROR};
  Estado estado;
  unsigned long tMensaje;
  float tarifaPara(const Tarjeta* t) const;
public:
  ModoCobro(PantallaLCD& l, LectorNFC& n, AlmacenSD& s, Reloj& r, Buzzer& b); 
  void ejecutar() override;
  void alEntrar() override;
  void alSalir() override;
};

#endif
