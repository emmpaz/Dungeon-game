#include <cstdlib>

#include "dice.h"


int32_t dice::roll(void) const
{
  int32_t total;
  uint32_t i;
  srand(time(0));
  total = base;

  if (sides) {
    for (i = 0; i < number; i++) {
      total += (rand()%sides)+1;
    }
  }

  return total;
}

std::ostream &dice::print(std::ostream &o)
{
  return o << base << '+' << number << 'd' << sides;
}

std::ostream &operator<<(std::ostream &o, dice &d)
{
  return d.print(o);
}
