#ifndef MODO_KIOSKO_H
#define MODO_KIOSKO_H

#include "Modo.h" 

class ModoKiosko : public Modo {
private:
  Teclado& teclado;
  enum Estado{ESPERANDO_MONTO, ESPERANDO_TARJETA, ESPERANDO_TIPO, MOSTRANDO_OK};
  Estado estado;
  float monto;
  char tipo;
  String uidPendiente;
  unsigned long tMensaje;
  void registrarYFinalizar(String uid, Tarjeta* t);
public:
  ModoKiosko(PantallaLCD& l, LectorNFC& n, AlmacenSD &s, Reloj &r, Buzzer &b, Teclado& t);  
  void ejecutar() override;
  void alEntrar() override;
};

#endif
