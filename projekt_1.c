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

// Posuvam ukazovatel v subore na zaciatok suboru
int to_file_start(FILE* file) {
  if (fseek(file, 0, SEEK_SET) == 0) return 1;
  return 0;
}

/*
vzhladom na to, ze na konci kazdeho riadku
je nejaky neurcity pocet medzier,
touto funkciou sa presuvam na dalsi riadok
*/
void new_line(FILE** file) {
  char c;
  while ((c = getc(*file)) != '\n' && c != EOF) {
    continue;
  }
}

// otvorit subor
int command_v(FILE** file) {
  *file = fopen("dataloger.txt", "r");
  int file_status = skontrolovat_subor(*file);
  if (!file_status) return 0;

  return 1;
}

int command_n(FILE** file, char*** ids) {
  int file_status = skontrolovat_subor(*file);
  if (!file_status) return 0;

  char symbol;
  int pocet_zoznamov = 0;
  while ((symbol = getc(*file)) != EOF) {
    if (symbol == '\n') {
      pocet_zoznamov++;
    }
  }
  pocet_zoznamov = (pocet_zoznamov + 1) / 7;
  to_file_start(*file);

  *ids = (char**)malloc(pocet_zoznamov * sizeof(char*));

  int id_size = 6;

  for (int i = 0; i < pocet_zoznamov; i++) {
    // praca s id
    (*ids)[i] = (char*)malloc(id_size * sizeof(char));
    for (int j = 0; j < id_size; j++) {
      (*ids)[i][j] = getc(*file);
    }

    new_line(&*file);
  }

  printf("pocet zaznaov %d\n", pocet_zoznamov);
}

int main() {
  char command;
  FILE* dataloger_file;
  int v_stav, n_stav;

  char** ids;

  do {
    scanf("%c", &command);
    if (command == 'v') v_stav = command_v(&dataloger_file);
    if (command == 'n') {
      n_stav = command_n(&dataloger_file, &ids);
      printf("%s", ids[0]);
    }
  } while (command != 'k');

  fclose(dataloger_file);
  return 0;
  return 0;
}
