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
int command_v(FILE** file, int pocet_zoznamov, char*** ids, char*** pozicie, char*** typy, double** hodnoty, char*** casy, char*** data) {
  *file = fopen("dataloger.txt", "r");
  int file_status = skontrolovat_subor(*file);
  if (!file_status) return 0;

  char symbol;
  if (!pocet_zoznamov) {
    while ((symbol = getc(*file)) != EOF) {
      printf("%c", symbol);
    }
    printf("\n");
    to_file_start(*file);
    return 1;
  }

  for (int i = 0; i < pocet_zoznamov; i++) {
    printf("ID. mer. modulu: %s\n", (*ids)[i]);
    printf("Pozícia modulu: %s\n", (*pozicie)[i]);
    printf("Typ mer. veliciny: %s\n", (*typy)[i]);
    printf("Hodnota: %lf\n", (*hodnoty)[i]);
    printf("Cas merania: %s\n", (*casy)[i]);
    printf("Datum merania: %s\n", (*data)[i]);
    printf("\n");
  }

  return 1;
}

// vytvorenie dynamickych polii
int command_n(FILE** file, char*** ids, char*** pozicie, char*** typy, double** hodnoty, char*** casy, char*** data) {
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

  // iniacilizacia dinamickych polii
  *ids = (char**)malloc(pocet_zoznamov * sizeof(char*));
  *pozicie = (char**)malloc(pocet_zoznamov * sizeof(char*));
  *typy = (char**)malloc(pocet_zoznamov * sizeof(char*));
  *hodnoty = (double*)malloc(pocet_zoznamov * sizeof(double));
  *casy = (char**)malloc(pocet_zoznamov * sizeof(char*));
  *data = (char**)malloc(pocet_zoznamov * sizeof(char*));

  int id_size = 6, pozicia_size = 14,
    typ_size = 2, cas_size = 4, datum_size = 8;

  for (int i = 0; i < pocet_zoznamov; i++) {
    // praca s id
    (*ids)[i] = (char*)malloc(id_size * sizeof(char));
    for (int j = 0; j < id_size; j++) {
      (*ids)[i][j] = getc(*file);
    }
    new_line(&*file);

    // praca s poziciami
    (*pozicie)[i] = (char*)malloc(pozicia_size * sizeof(char));
    for (int j = 0; j < pozicia_size; j++) {
      (*pozicie)[i][j] = getc(*file);
    }
    new_line(&*file);

    // praca s typmi
    (*typy)[i] = (char*)malloc(typ_size * sizeof(char));
    for (int j = 0; j < typ_size; j++) {
      (*typy)[i][j] = getc(*file);
    }
    new_line(&*file);

    // praca s hodnotmi
    fscanf(*file, "%lf", &(*hodnoty)[i]);
    new_line(&*file);

    // praca s casom
    (*casy)[i] = (char*)malloc(cas_size * sizeof(char));
    for (int j = 0; j < cas_size; j++) {
      (*casy)[i][j] = getc(*file);
    }
    new_line(&*file);

    // praca s datami
    (*data)[i] = (char*)malloc(datum_size * sizeof(char));
    for (int j = 0; j < datum_size; j++) {
      (*data)[i][j] = getc(*file);
    }
    new_line(&*file);
    // preskocit prazdny riadok
    new_line(&*file);
  }

  printf("pocet zaznaov %d\n", pocet_zoznamov);
  return pocet_zoznamov;
}

int main() {
  char command;
  FILE* dataloger_file = NULL;
  int v_stav, pocet_zoznamov = 0;

  char** ids, ** typy, ** pozicie, ** casy, ** data;
  double* hodnoty;

  do {
    scanf("%c", &command);
    if (command == 'v') v_stav = command_v(&dataloger_file, pocet_zoznamov, &ids, &pozicie, &typy, &hodnoty, &casy, &data);
    if (command == 'n') {
      pocet_zoznamov = command_n(&dataloger_file, &ids, &pozicie, &typy, &hodnoty, &casy, &data);
      for (int i = 0; i < pocet_zoznamov; i++) {
      }
    }

  } while (command != 'k');

  free(ids);
  fclose(dataloger_file);
  return 0;
  return 0;
}
