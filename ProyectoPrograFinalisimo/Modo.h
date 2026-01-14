#ifndef MODO_H
#define MODO_H

class Modo {
public:
  virtual void ejecutar() = 0;
  virtual void alEntrar() {}
  virtual void alSalir() {}
  virtual ~Modo() {}
};

#endif
