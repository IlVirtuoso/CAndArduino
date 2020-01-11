#ifndef PARAMETERS_H
#include "parameters.h"
#endif

int equals(char *string, char *tocompare);

char options[128][128];
char buffer[100];
char token[28];
void ParseFile(FILE *config)
{
  char c;
  int i = 0, b = 0, x = 0, r = 0;
  SO_ALTEZZA = -1;
  SO_BASE = -1;
  SO_FLAG_MAX = -1;
  SO_FLAG_MIN = -1;
  SO_MAX_TIME = -1;
  SO_MIN_HOLD_NSEC = -1;
  SO_N_MOVES = -1;
  SO_NUM_G = -1;
  SO_NUM_P = -1;
  SO_ROUND_SCORE = -1;

  fseek(config, 0, 0);
  while ((c = fgetc(config)) != EOF)
  {
    if (c == '\n')
    {
      i++;
      b = 0;
    }
    else if (i > 127)
      error("Too many Arguments", E2BIG);
    else if (b > 127)
      error("Config buffer Too Big", E2BIG);
    else
    {
      options[i][b] = c;
      b++;
    }
  }
  b = 0;

  for (x = 0; x <= i; x++)
  {
    while (options[x][b] != '=')
    {
      buffer[b] = options[x][b];
      b++;
    }
    b++;
    while (options[x][b] != '\000')
    {
      token[r] = options[x][b];
      b++;
      r++;
    }
    printf("%s", buffer);
    if (equals("SO_BASE", buffer))
      SO_BASE = atol(token);
    else if (equals("SO_ALTEZZA", buffer))
      SO_ALTEZZA = atol(token);
    else if (equals("SO_NUM_G", buffer))
      SO_NUM_G = atol(token);
    else if (equals("SO_NUM_P", buffer))
      SO_NUM_P = atol(token);
    else if (equals("SO_FLAG_MIN", buffer))
      SO_FLAG_MIN = atol(token);
    else if (equals("SO_FLAG_MAX", buffer))
      SO_FLAG_MAX = atol(token);
    else if (equals("SO_ROUND_SCORE", buffer))
      SO_ROUND_SCORE = atol(token);
    else if (equals("SO_MAX_TIME", buffer))
      SO_MAX_TIME = atol(token);
    else if (equals("SO_MIN_HOLD_NSEC", buffer))
      SO_MIN_HOLD_NSEC = atol(token);
    else if (equals("SO_N_MOVES", buffer))
      SO_N_MOVES = atol(token);
    else
    {
      error("stringa non appartenente allo standard di configurazione", EBADR);
    }

    printf("Variabile: %s impostata a %d", buffer, atoi(token));
    bzero(buffer, sizeof(buffer));
    bzero(token, sizeof(token));
    b = 0;
    r = 0;
  }
}

int equals(char *string, char *tocompare)
{
  int b = 0;
  int r = 0;
  while (string[b] == tocompare[b])
  {
    b++;
  }
  if (tocompare[b] == '\000')
  {
    r = 1;
  }
  return r;
}