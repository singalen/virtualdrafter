#include "MyStrUtil.h"

string ExtractWord(string& s, string Delimiters)
{
  string Result;
  int i = s.find_first_not_of(Delimiters);
  if (i > 0) s.erase(0, i);
  i = s.find_first_of(Delimiters);
  if (i < 0)
  {
    Result = s;
    s.resize(0);
  }
  else
  {
    Result = s.substr(0, i); // i cannot be 0
    s.erase(0, i+1);
    i = s.find_first_not_of(Delimiters);
    if (i > 0) s.erase(0, i);
  }
  return Result;
}
