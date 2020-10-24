//P1 P2 P3 -> cascading output redirection
//P1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main()
{
  char ch[100];
  printf("Enter a number: ");
  scanf("%s", ch);
  printf("Number is: %s\n", ch);

  int pp[2];
  pipe(pp);


  dup2(pp[0], 0);
  write(pp[1], ch, strlen(ch));
  close(pp[1]);
  

  FILE *file = popen("./2-3-2", "r");
  dup2(fileno(file), 0);

  file = popen("./2-3-3", "r");
  dup2(fileno(file), 0);

  file = popen("./2-3-4", "r");
  dup2(fileno(file), 0);

  char data[100];
  //fgets(data, 100, stdin);
  scanf("%s", data);
  printf("%s", data);

  putchar('\n');
  return 0;
}
