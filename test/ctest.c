#include <stdio.h>

typedef struct sss{
  int x;
  int y;
} SPE;

int main(){
  SPE s;
  s.x = 123;
  s.y = 2222;
  printf("x %d, y %d", s.x, s.y);
  return 0;
}
