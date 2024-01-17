#include <stdio.h>
#include <stdlib.h>

int state_machine()
{
  enum states
  {
    RUN,
    READY,
    BLOCKED,
    IDLE
  };

  enum states state = IDLE;

  int value = 0;

  while (1)
  {
    switch (state)
    {
      case BLOCKED:
        {
          printf("I am in BLOCKED state\n");
          state = IDLE;
        }
        break;
      case READY:
        {
          printf("I am in the READY state\n");
          state = IDLE;
          break;
        }
      case RUN:
        {
          printf("I am in the RUN state\n");
          state = IDLE;
          break;
        }
      case IDLE:
        {
          printf("Enter state selection: 1 (BLOCKED), 2 (READY), 3 (RUN) ");
          scanf("%d", &value);
          switch (value)
          {
            case 1:
              state = BLOCKED;
              break;
            case 2:
              state = READY;
              break;
            case 3:
              state = RUN;
              break;
            default:
              exit(0);
              break;
          }
        }
        break;
    }
  }
  return 1;
}

int main()
{
  state_machine();
  return 0;
}
