#include <stdio.h>
#include <stdlib.h>

// v mojom priklade pouzivam 0 ako vratanu hodnotu s chybou
// 1 ako uspesu vratnu hodnotu

// kontroluje, ci subor bol otvoreny / uspesne otoreny
int skontrolovat_subor(FILE* file) {
  if (file == NULL || !file) {
    printf("Neotvoreny subor.\n");
    return 0;
  }
  printf("Subor bol uspesne otvoreny\n");
  return 1;
}

// otvorit subor
int command_v(FILE** file) {
  *file = fopen("dataloger.txt", "r");
  int file_status = skontrolovat_subor(*file);
  if (!file_status) return 0;

  return 1;
}

int main() {
  char command;
  FILE* dataloger_file;
  int v_stav, n_stav;

  do {
    scanf("%c", &command);
    if (command == 'v') v_stav = command_v(&dataloger_file);
  } while (command != 'k');

  fclose(dataloger_file);
  return 0;
  return 0;
}
