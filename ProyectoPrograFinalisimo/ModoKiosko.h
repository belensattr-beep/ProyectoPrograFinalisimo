#ifndef MODO_KIOSKO_H
#define MODO_KIOSKO_H

#include "Modo.h" 
#include "SistemaTransporte.h"

class ModoKiosko : public Modo {
private:
  PantallaLCD& lcd;
  Buzzer& buzzer;
  Reloj& reloj;
  AlmacenSD& sd;
  LectorNFC& nfc;
  Teclado& teclado;
  enum Estado{ESPERANDO_MONTO, ESPERANDO_TARJETA, ESPERANDO_TIPO, MOSTRANDO_OK};
  Estado estado;
  float monto;
  char tipo;
  String uidPendiente;
  unsigned long tMensaje;
  void registrarYFinalizar(String uid, Tarjeta* t);
public:
  ModoKiosko(PantallaLCD& l, Buzzer& b, Reloj& r, AlmacenSD& s, LectorNFC& n, Teclado& t);  
  void ejecutar() override;
  void alEntrar() override;
  void alSalir() override;
};

#endif
