#include <vcl.h>
#include "DrawMTGC.h"
#include "JpgImg.hpp"
#include <memory>
#include <string>
#pragma hdrstop

#include "CardData.h"
#include "MyStrUtil.h"

#pragma package(smart_init)

// также воспринимает символы тапа
// возвращает ширину строки
int DrawMana(
  AnsiString Mana,
  TCanvas* Target,
  int X, int Y,
  bool RightToLeft = false)
{
  if (Mana.IsEmpty()) return 0;
  int Result;

  int i;
  if (((i = Mana.Pos(":")) > 0) || ((i = Mana.Pos(",")) > 0) || ((i = Mana.Pos("\"")) > 0))
  {
    Result = DrawMana(Mana.SubString(1, i-1), Target, X, Y, RightToLeft);
    Mana.Delete(1, i-1);
    if (Mana.IsEmpty()) return Result;
    Target->TextOut(X + Result, Y, Mana[1]);
    Result += Target->TextWidth(Mana[1]);
    Mana.Delete(1, 1);
    Result += DrawMana(Mana, Target, X+Result, Y, RightToLeft);
    return Result;
  }

  Target->Font->Name = "MagicSymbols";

  AnsiString Digits = "";
  while (!Mana.IsEmpty() && (Mana[1] <= '9'))
  {
    Digits += Mana[1];
    Mana.Delete(1, 1);
  }

  Target->Font->Color = clWhite;
  Target->Font->Size = 11;
  AnsiString o = AnsiString::StringOfChar('O', Mana.Length() + (Digits.IsEmpty() ? 0 : 1));
  Result = Target->TextWidth(o);
  if (RightToLeft)
  {
    X -= Result;
  }
  Target->TextOut(X, Y-1, o);

  int OWidth = Target->TextWidth("O");

  //if (!Digits.IsEmpty())

  Target->Font->Color = clBlack;
  Target->Font->Size = 10;
  if (!Digits.IsEmpty())
  {
    Target->TextOut(X+(OWidth-Target->TextWidth(Digits))/2, Y, Digits);
    X += OWidth;
  }

  for (int i=1; i<=Mana.Length(); i++)
  {
    switch (Mana[i])
    {
    case 'U':
      Target->Font->Color = clBlue;
      break;
    case 'B':
    default:
      Target->Font->Color = clBlack;
      break;
    case 'R':
      Target->Font->Color = clRed;
      break;
    case 'G':
      Target->Font->Color = clGreen;
      break;
    case 'W':
      Target->Font->Color = clBlack;
      break;
    }
    Target->TextOut(X, Y, Mana[i]);
    X += OWidth;
  }

  return Result;
}

void DrawText(
  AnsiString Text,
  int X, int Y,
  int Width,
  TCanvas* Target)
{
  std::auto_ptr<TStringList> Lines (new TStringList);
  int i;
  while ((i = Text.Pos("#^")) > 0)
  {
    Text.Delete(i, 2);
  }
  while ((i = Text.Pos("#P")) > 0)
  {
    Lines->Add(Text.SubString(1, i-1));
    Text.Delete(1, i+2);
  }
  while ((i = Text.Pos("\n")) > 0)
  {
    Lines->Add(Text.SubString(1, i-1));
    Text.Delete(1, i+1); // Наш \n тоже удаляем.
  }
  Lines->Add(Text);

  Target->Font->Size = 9;
  /*if (Target->TextWidth(Text) > Width*9)
  {
    Target->Font->Size = 8;
  }
  else if (Target->TextWidth(Text) > Width*5)
  {
    Target->Font->Size = 9;
  }*/

  for (int l = 0; l<Lines->Count; l++)
  {
    // откусываем струко по ширине
    if (Target->TextWidth(Lines->Strings[l]) > Width)
    {
      string CutLine;
      string Word;
      string Line = Lines->Strings[l].c_str();
      do
      {
        Word = ExtractWord(Line, " ");
        CutLine += " " + Word;
        if (Word.find("<") < Word.size())
        {
          do
          {
            if (Line.empty()) break;
            Word = ExtractWord(Line, " ");
            CutLine += " " + Word;
          }
          while (Word.find(">") >= Word.size());
        }
      }
      while (Target->TextWidth(CutLine.c_str()) < Width - 5);

      CutLine.erase(0, 1); // тут получается лишний пробел
      Lines->Strings[l] = CutLine.c_str();
      Lines->Insert(l+1, Line.c_str());
    }
    // откусывание закончили, перешли к отрисовке строки

//Abandoned Outpost comes into play tapped. #P
// < T: > Add < W > to your mana pool. #P
// < T, G, > Sacrifice Abandoned Outpost: Add one mana of any color to your mana pool.

    AnsiString Line = Lines->Strings[l];
    int CurX = X;
    bool InSpecials = false;
    AnsiString ToPrint;
    while (!Line.IsEmpty())
    {
      int i;
      while ((i = Line.Pos('<')) > 0)
      {
        AnsiString s = Line.SubString(1, i-1);
        Line.Delete(1, i);
        Target->Font->Name = "Times New Roman";
        Target->Font->Size = 9;
        Target->Font->Color = clBlack;
        Target->TextOut(CurX, Y, s);
        CurX += Target->TextWidth(s);

        i = Line.Pos('>');
        if (i <= 0) i = Line.Length();

        s = Line.SubString(1, i-1);
        Line.Delete(1, i);
        s = s.Trim();
        CurX += DrawMana(s, Target, CurX, Y);
      }
      Target->Font->Name = "Times New Roman";
      Target->Font->Size = 9;
      Target->Font->Color = clBlack;
      Target->TextOut(CurX, Y, Line);
      Line = "";
      //CurX += Target->TextWidth(Line);
    }
    Y += Target->TextHeight("Wg")+2;
  }
}

void DrawCard(
  AnsiString Name,
  AnsiString Edition,
  AnsiString No,
  TCardPile& Cards,
  TCanvas* Target)
{
  // не хаваем мы stl...
  unsigned int CardIndex = 0;
  while (CardIndex < Cards.size())
  {
    if ((Cards[CardIndex]->Name == Name.c_str())
        && (Cards[CardIndex]->Edition == Edition.c_str())) break;
    CardIndex++;
  }
  if (CardIndex == Cards.size()) return;
  //if (CardIndex == Cards.end()) return;

  extern string PicturesPath;
  extern string MaglibPath;

  AnsiString PicPath = PicturesPath.c_str();
    //ExtractFilePath(Application->ExeName)+"pics\\";
  AnsiString DataPath = (MaglibPath + "data\\").c_str();
    //ExtractFilePath(Application->ExeName)+"data\\";

  //int CardCount = std::count(Cards.begin(), Cards.end(), Name);

  TCardData* Card = Cards[CardIndex];//*CardIndex;
  AnsiString Background;
  switch (Card->GetColor())
  {
  case cGreen:
    Background = "g";
    break;
  case cRed:
    Background = "r";
    break;
  case cBlack:
    Background = "b";
    break;
  case cBlue:
    Background = "u";
    break;
  case cWhite:
    Background = "w";
    break;
  case cColorless:
    if (Card->Type == "Land")
      Background = "lnd";
    else
      Background = "art";
    break;
  case cColorCount:
    Background = "gld";
    break;
  }

  int CardWidth = Target->ClipRect.Right;
  int CardHeight = Target->ClipRect.Bottom;
  int ArtHeight = 
    218*CardHeight/431;//SourceHeight;
  int BorderWidth = 20 * CardWidth/294;//SourceWidth;
  int BorderHeight = 24*CardHeight/400;//SourceHeight;
 
  TJPEGImage* Img = new TJPEGImage(Application);
  try
  {
    // background
    Img->LoadFromFile(DataPath + Background + ".jpg");
    //int SourceWidth = Img->Picture->Height;
    //int SourceHeight = Img->Picture->Width;
    Target->StretchDraw(Target->ClipRect, Img->Picture->Graphic);

    // art
    Img->LoadFromFile(PicPath + Card->Edition.c_str() + "\\" +
      Card->Name.c_str() + ".jpg");
    Target->StretchDraw(
      Rect(BorderWidth, BorderHeight, CardWidth-BorderWidth, ArtHeight+BorderHeight),
      Img->Picture->Graphic);
  }
  catch(...)
  {
    delete Img;
  }

  // name
  Target->Font->Name = "Times New Roman";
  Target->Font->Size = 9;
  Target->Font->Style = TFontStyles() << fsBold;
  Target->Brush->Style = bsClear;

  Target->Font->Color = clBlack;
  Target->TextOut(BorderWidth, 2, Card->Name.c_str());
  Target->Font->Color = clWhite;
  Target->TextOut(BorderWidth-1, 2-1, Card->Name.c_str());

  // p/t
  string PowerToughness = Card->Power + "/" + Card->Toughness;
  if (PowerToughness == "/") PowerToughness = "";
  Target->Font->Color = clBlack;
  Target->TextOut(
    CardWidth-BorderWidth+2-Target->TextWidth(
      PowerToughness.c_str())+1,
    CardHeight-BorderHeight+1,
    PowerToughness.c_str());
  Target->Font->Color = clWhite;
  Target->TextOut(
    CardWidth-BorderWidth+2-Target->TextWidth(
    PowerToughness.c_str()),
    CardHeight-BorderHeight,
    PowerToughness.c_str());

  // type
  Target->Font->Style = TFontStyles();// << fsBold;
  AnsiString CardType = (Card->Type == "Creature") ?
    AnsiString("Creature - ") + Card->SubTypes.c_str() :
    AnsiString(Card->Type.c_str());

  Target->Font->Color = clBlack;
  Target->TextOut(BorderWidth+1, ArtHeight+BorderHeight+1, CardType);
  Target->Font->Color = clWhite;
  Target->TextOut(BorderWidth, ArtHeight+BorderHeight, CardType);

  // манакост
  DrawMana(Card->ManaCost.c_str(), Target, CardWidth-BorderWidth+2, 2, true);

  // текст, однако
  DrawText(Card->Text.c_str(),
    BorderWidth+1, ArtHeight+BorderHeight*2+4, CardWidth-BorderWidth*2, Target);
}

class TMagicFontKeeper
{
  bool MagicFontOwned;
  bool TextFontOwned;
  AnsiString MagicFontPath;
  AnsiString TextFontPath;
public:
  TMagicFontKeeper() : MagicFontOwned(false), TextFontOwned(false)
  {
    MagicFontOwned = true;
    MagicFontPath = ExtractFilePath(Application->ExeName)+"font\\Magis___.ttf";
    try
    {
      AddFontResource(MagicFontPath.c_str());
    }
    catch (...)
    {
      MagicFontOwned = false;
    }

    TextFontOwned = true;
    TextFontPath = ExtractFilePath(Application->ExeName)+"font\\Tt0298m_.ttf";
    try
    {
      AddFontResource(TextFontPath.c_str());
    }
    catch (...)
    {
      TextFontOwned = false;
    }
  }

  ~TMagicFontKeeper()
  {
    if (MagicFontOwned)
    {
      try
      {
        RemoveFontResource(MagicFontPath.c_str());
      }
      catch (...)
      {
      }
    }
    if (TextFontOwned)
    {
      try
      {
        RemoveFontResource(TextFontPath.c_str());
      }
      catch (...)
      {
      } // 100 строк закрывающих скобок?..
    }
  }
};

TMagicFontKeeper FontKeeper;
