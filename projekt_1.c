#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// v mojom priklade pouzivam 0 ako vratanu hodnotu s chybou
// 1 ako uspesu vratnu hodnotu

// todo: recreate arrays str; add \0

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

// free memory functions

void free_char_2D(char*** pole, int rows) {
  for (int i = 0; i < rows; i++) {
    free((*pole)[i]);
  }
  free(*pole);
}
void free_double_1D(double** pole) {
  free(*pole);
}

void free_all_arrays(char*** ids, char*** pozicie, char*** typy, double** hodnoty, char*** casy, char*** data, int rows) {
  free_char_2D(&*ids, rows);
  free_char_2D(&*pozicie, rows);
  free_char_2D(&*typy, rows);
  free_double_1D(&*hodnoty);
  free_char_2D(&*casy, rows);
  free_char_2D(&*data, rows);
}

int find_ciachovanie(FILE** ciachovanie_file, char** id, int id_size, char** ciachovanie_datum, int ciachovanie_datum_size) {
  char symbol;
  int counter = 0;
  char* check_id = (char*)malloc((id_size + 1) * sizeof(char));

  while ((symbol = getc(*ciachovanie_file)) != EOF) {
    if (symbol == '\n') {
      new_line(&*ciachovanie_file);
      counter = 0;
      free(check_id);
      check_id = (char*)malloc((id_size + 1) * sizeof(char));
      continue;
    }

    if (counter < id_size) {
      check_id[counter] = symbol;
      counter++;
      continue;
    }
    if (counter == id_size) check_id[counter] = '\0';

    printf("str1 str2: %s %s %d %d\n", *id, check_id, strcmp(*id, check_id), counter);
    if (counter == id_size && !strcmp(*id, check_id)) {
      char datum_char;
      for (int i = 0; i < ciachovanie_datum_size; i++) {
        datum_char = getc(*ciachovanie_file);
        (*ciachovanie_datum)[i] = datum_char;
      }
      free(check_id);
      to_file_start(*ciachovanie_file);
      return 1;
    }
    else {
      counter = 0;
      free(check_id);
      check_id = (char*)malloc((id_size + 1) * sizeof(char));
      continue;
    }
  }

  return 0;
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
int command_n(FILE** file, int pocet_zoznamov, char*** ids, char*** pozicie, char*** typy, double** hodnoty, char*** casy, char*** data) {
  int file_status = skontrolovat_subor(*file);
  if (!file_status) return 0;

  if (!pocet_zoznamov) {
    printf("polia neboli vytvorene, vytvoram!\n");
    printf("address ids check %p\n", &*ids);
  }
  else {
    // printf("adres1 %p\n", &*ids);
    printf("data from array %s\n", (*pozicie)[0]);
    free_all_arrays(&*ids, &*pozicie, &*typy, &*hodnoty, &*casy, &*data, pocet_zoznamov);
    printf("data from array %s\n", (*pozicie)[0]);
  }

  char symbol;
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

  int id_size = 5, pozicia_size = 14,
    typ_size = 2, cas_size = 4, datum_size = 8;

  for (int i = 0; i < pocet_zoznamov; i++) {
    // praca s id
    (*ids)[i] = (char*)malloc((id_size + 1) * sizeof(char));
    for (int j = 0; j < id_size; j++) {
      (*ids)[i][j] = getc(*file);
    }
    (*ids)[i][id_size] = '\0';
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

int command_c(int pocet_zoznamov, char*** ids) {
  if (!pocet_zoznamov) {
    printf("POlia neboli vytvorene!\n");
    return 0;
  }

  int y;

  printf("Zadajte pocet mesiacov\n");
  scanf("%d", &y);

  FILE* ciachovanie_file = fopen("ciachovanie.txt", "r");
  if (ciachovanie_file == NULL) {
    printf("Nepodarilo sa otvorit subor\n");
    return 0;
  }

  int ciachovanie_datum_size = 8;
  char* ciachovanie_datum;

  for (int i = 0; i < pocet_zoznamov; i++) {
    ciachovanie_datum = (char*)malloc(ciachovanie_datum_size * sizeof(char));
    int ciachovanie = find_ciachovanie(&ciachovanie_file, &(*ids)[i], 5, &ciachovanie_datum, ciachovanie_datum_size);

    if (!ciachovanie) printf("nie je take ciachovanie\n");
    else printf("Nasla som ciachovaie. Datum: %s\n", ciachovanie_datum);
    free(ciachovanie_datum);
  }
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
    if (command == 'n') pocet_zoznamov = command_n(&dataloger_file, pocet_zoznamov, &ids, &pozicie, &typy, &hodnoty, &casy, &data);
    if (command == 'c') command_c(pocet_zoznamov, &ids);

  } while (command != 'k');

  free_all_arrays(&ids, &pozicie, &typy, &hodnoty, &casy, &data, pocet_zoznamov);
  fclose(dataloger_file);
  return 0;
  return 0;
}
