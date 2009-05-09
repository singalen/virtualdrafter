#ifndef NetPlayerH
#define NetPlayerH

#include "Player.h"
#include <sysdefs.h> // для dstring.h
#include <vcl/dstring.h>

/*

  тут заюзаем LocalPlayer

class TClientNetPlayer : public TDraftPlayer
{
public:
  TClientNetPlayer(string aName, bool aLog);
  virtual void Pick(TCardPile& Booster);
};*/

namespace Scktcomp {

class TCustomWinSocket;

}

// время для реконнекта
#define TIMEOUT 30

class TServerNetPlayer : public TDraftPlayer
{
public:
  TServerNetPlayer(
    string aName,
    bool aLog,
    Scktcomp::TCustomWinSocket *aSocket);
  ~TServerNetPlayer();
  virtual void Pick(TCardPile& Booster);

  // аксесоры сокета. Нулевой сокет - значит, клиент отвалился.
  Scktcomp::TCustomWinSocket* GetSocket() const { return Socket; };
  void SetSocket(Scktcomp::TCustomWinSocket*);

  // сокетные функции
  AnsiString Buffer;
  void OnReceiveText(AnsiString Text);

  bool UsedByFactory;

private:
  Scktcomp::TCustomWinSocket *Socket;
  TCardPile Booster;
  bool PickReceived;
  int PickIndex;
};

#endif
