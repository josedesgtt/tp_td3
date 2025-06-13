#include <vector>

#include "lib.h"
#include "calendario.h"
#include "billetera.h"
#include "blockchain.h"

using namespace std;

Billetera::Billetera(const id_billetera id, Blockchain* blockchain)
  : _id(id)
  , _blockchain(blockchain) {
}

id_billetera Billetera::id() const {
  return _id;
}

void Billetera::notificar_transaccion(Transaccion t) {
  _transacciones.push_back(&t);
  if (t.origen == _id){
    _saldo -= t.monto;
    nodo* destinatario;
    nodo* resultado;
    nodo* ref_inicial;
    destinatario->id_bille = t.destino;
    if (_primero ==  nullptr) {
      _primero = destinatario;
      destinatario->cant++;
    } else {
      resultado = buscar_nodo(_primero, t.destino);
      if (resultado != nullptr) {
        resultado->cant++;
        ref_inicial = resultado->anterior;
        while (ref_inicial->cant < resultado->cant && ref_inicial != nullptr) {
          ref_inicial = ref_inicial->anterior;
        }
        if (ref_inicial == nullptr) {
          resultado->siguiente = _primero;
          resultado->anterior = nullptr;
          _primero = resultado;
          _primero->siguiente->anterior = resultado;
          resultado->anterior->siguiente = resultado->siguiente;
          resultado->siguiente->anterior = resultado->anterior;
        }
        ref_inicial->
      }
    }
  }else{
    _saldo += t.monto;
  }
  
}

Billetera::nodo* buscar_nodo(Billetera::nodo* inicio, id_billetera id) {
  Billetera::nodo* actual = inicio;
  while (actual != nullptr) {
    if (actual->id_bille == id) return actual;
    actual = actual->siguiente;
  }
  return nullptr;
}

monto Billetera::saldo() const {
  return _saldo;
}

monto Billetera::saldo_al_fin_del_dia(timestamp t) const {
  const list<Transaccion> transacciones = _blockchain->transacciones();
  timestamp fin_del_dia = Calendario::fin_del_dia(t);

  monto ret = 0;

  auto it = transacciones.begin();
  while (it != transacciones.end() && it->_timestamp < fin_del_dia) {
    if (it->origen == _id) {
      ret -= it->monto;
    } else if (it->destino == _id) {
      ret += it->monto;
    }

    ++it;
  }

  return ret;
}

vector<Transaccion> Billetera::ultimas_transacciones(int k) const {

  vector<Transaccion> ret;

  // Notar que `rbegin` y `rend` recorren la lista en orden inverso.
  auto it = _transacciones.rbegin();
  while(it != _transacciones.rend() && ret.size() < k) {
      ret.push_back(**it);
      ++it;
    }
    
  return ret;
}

vector<id_billetera> Billetera::detinatarios_mas_frecuentes(int k) const {
  const list<Transaccion> transacciones = _blockchain->transacciones();

  // cuento la cantidad de transacciones salientes por cada billetera de destino
  map<id_billetera, int> transacciones_por_billetera;
  auto it = transacciones.begin();
  while (it != transacciones.end()) {
    if (it->origen == _id) {
      // notar que el map devuelve 0 por default!
      transacciones_por_billetera[it->destino]++;
    }
    ++it;
  }

  // invierto el map de forma que puedo acceder a las billeteras según su
  // cantidad de transacciones.
  map<int, vector<id_billetera>> billeteras_por_cantidad_de_transacciones;
  auto it2 = transacciones_por_billetera.begin();
  while (it2 != transacciones_por_billetera.end()) {
    billeteras_por_cantidad_de_transacciones[it2->second].push_back(it2->first);
    ++it2;
  }

  // recorro el map usando un iterador en orden inverso, que me lleva por todos
  // los pares de entradas desde la mayor clave hasta la menor.
  vector<id_billetera> ret = {};
  auto it3 = billeteras_por_cantidad_de_transacciones.rbegin();
  while (it3 != billeteras_por_cantidad_de_transacciones.rend() && ret.size() < k) {
    vector<id_billetera> siguiente_grupo = it3->second;
    int i = 0;
    while (i < siguiente_grupo.size() && ret.size() < k) {
      ret.push_back(siguiente_grupo[i]);
      i++;
    }
    ++it3;
  }

  return ret;
}
