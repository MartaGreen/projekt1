#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int compare_dates(char* date1, char* date2) {
  int year1, month1, day1;
  int year2, month2, day2;

  sscanf(date1, "%4d%2d%2d", &year1, &month1, &day1);
  sscanf(date2, "%4d%2d%2d", &year2, &month2, &day2);

  int total_month1 = year1 * 12 + month1;
  int total_month2 = year2 * 12 + month2;

  int month_diff = total_month1 - total_month2;

  if (month_diff < 0) return -month_diff;
  return month_diff;
}

int compare_typy(char** typ, char checked_typ[3]) {
  if (!strcmp(*typ, checked_typ)) {
    return 1;
  }

  return 0;
}

void find_min_max(double* min, double* max, double* checked_value, int* counter) {
  (*counter)++;

  if (*min == -1 && *max == -1) {
    *min = *checked_value;
    *max = *checked_value;
  }

  if (*min > *checked_value) *min = *checked_value;
  if (*max < *checked_value) *max = *checked_value;
}

void print_table_row(char typ[2], int count, double min, double max) {
  if (min != -1 && max != -1) printf("%7s %14d %14.2lf %9.2lf\n", typ, count, min, max);
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
    (*pozicie)[i] = (char*)malloc((pozicia_size + 1) * sizeof(char));
    for (int j = 0; j < pozicia_size; j++) {
      (*pozicie)[i][j] = getc(*file);
    }
    (*pozicie)[i][pozicia_size] = '\0';
    new_line(&*file);

    // praca s typmi
    (*typy)[i] = (char*)malloc((typ_size + 1) * sizeof(char));
    for (int j = 0; j < typ_size; j++) {
      (*typy)[i][j] = getc(*file);
    }
    (*typy)[i][typ_size] = '\0';
    new_line(&*file);

    // praca s hodnotmi
    fscanf(*file, "%lf", &(*hodnoty)[i]);
    new_line(&*file);

    // praca s casom
    (*casy)[i] = (char*)malloc((cas_size + 1) * sizeof(char));
    for (int j = 0; j < cas_size; j++) {
      (*casy)[i][j] = getc(*file);
    }
    (*casy)[i][cas_size] = '\0';
    new_line(&*file);

    // praca s datami
    (*data)[i] = (char*)malloc((datum_size + 1) * sizeof(char));
    for (int j = 0; j < datum_size; j++) {
      (*data)[i][j] = getc(*file);
    }
    (*data)[i][datum_size] = '\0';
    new_line(&*file);
    // preskocit prazdny riadok
    new_line(&*file);
  }

  printf("pocet zaznaov %d\n", pocet_zoznamov);
  return pocet_zoznamov;
}

int command_c(int pocet_zoznamov, char*** ids, char*** data) {
  if (!pocet_zoznamov) {
    printf("Polia nie su vytvorene.!\n");
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

    if (!ciachovanie) printf("ID. mer. modulu [%s] nie je ciachovany.\n", (*ids)[i]);
    else {
      int date_diff = compare_dates((*data)[i], ciachovanie_datum);
      if (date_diff > y) {
        printf("ID. mer. modulu [%s] má %d mesiacov po ciachovani\n", (*ids)[i], date_diff);
      }
    }
    free(ciachovanie_datum);
  }
  return 1;
}

int command_h(int pocet_zaznamov, char vsetky_typy[6][3], char*** typy, double** hodnoty) {
  if (!pocet_zaznamov) {
    printf("Polia nie su vytvorene.\n");
    return 0;
  }

  double rd_min = -1, rd_max = -1,
    rm_min = -1, rm_max = -1,
    ro_min = -1, ro_max = -1,
    pi_min = -1, pi_max = -1,
    pe_min = -1, pe_max = -1,
    pa_min = -1, pa_max = -1;
  int rd_count = 0, rm_count = 0, ro_count = 0,
    pi_count = 0, pe_count = 0, pa_count = 0;

  for (int i = 0; i < pocet_zaznamov; i++) {
    char* current_typ = (*typy)[i];
    double hodnota = (*hodnoty)[i];

    if (compare_typy(&current_typ, vsetky_typy[0])) {
      find_min_max(&rd_min, &rd_max, &hodnota, &rd_count);
      continue;
    }
    if (compare_typy(&current_typ, vsetky_typy[1])) {
      find_min_max(&rm_min, &rm_max, &hodnota, &rm_count);
      continue;
    }
    if (compare_typy(&current_typ, vsetky_typy[2])) {
      find_min_max(&ro_min, &ro_max, &hodnota, &ro_count);
      continue;
    }
    if (compare_typy(&current_typ, vsetky_typy[3])) {
      find_min_max(&pi_min, &pi_max, &hodnota, &pi_count);
      continue;
    }
    if (compare_typy(&current_typ, vsetky_typy[4])) {
      find_min_max(&pe_min, &pe_max, &hodnota, &pe_count);
      continue;
    }
    if (compare_typy(&current_typ, vsetky_typy[5])) {
      find_min_max(&pa_min, &pa_max, &hodnota, &pa_count);
      continue;
    }
  }

  printf("%-16s %-12s %-9s %-9s\n", "Typ mer. vel.", "Pocetnost", "Minimum", "Maximum");

  print_table_row(vsetky_typy[0], rd_count, rd_min, rd_max);
  print_table_row(vsetky_typy[1], rm_count, rm_min, rm_max);
  print_table_row(vsetky_typy[2], ro_count, ro_min, ro_max);
  print_table_row(vsetky_typy[3], pi_count, pi_min, pi_max);
  print_table_row(vsetky_typy[4], pe_count, pe_min, pe_max);
  print_table_row(vsetky_typy[5], pa_count, pa_min, pa_max);
}

int command_z(FILE** orig_file, int pocet_zoznamov, char*** ids, char*** pozicie, char*** typy, double** hodnoty, char*** casy, char*** data) {
  if (!pocet_zoznamov) {
    printf("Polia nie su vytvorene.\n");
    return 1;
  }

  char orig_name[] = "dataloger.txt\0";
  char copy_name[] = "dataloger_copy.txt\0";
  FILE* file_copy = fopen(copy_name, "w");

  char id[6];
  scanf("%s", id);
  int removed_counter = 0;

  for (int i = 0; i < pocet_zoznamov; i++) {
    if (!strcmp((*ids)[i], id)) {
      removed_counter++;
      continue;
    }

    fprintf(file_copy, "%s\n", (*ids)[i]);
    fprintf(file_copy, "%s\n", (*pozicie)[i]);
    fprintf(file_copy, "%s\n", (*typy)[i]);
    fprintf(file_copy, "%lf\n", (*hodnoty)[i]);
    fprintf(file_copy, "%s\n", (*casy)[i]);
    fprintf(file_copy, "%s\n", (*data)[i]);
    fprintf(file_copy, "%s", "\n");
  }

  fclose(file_copy);
  fclose(*orig_file);
  remove(orig_name);
  *orig_file = freopen(orig_name, "r", *orig_file);
  rename(copy_name, orig_name);

  printf("Vymazalo sa : %d zaznamov !", removed_counter);

  return 1;
}

int main() {
  char command;
  FILE* dataloger_file = NULL;
  int v_stav, pocet_zoznamov = 0;

  char** ids, ** typy, ** pozicie, ** casy, ** data;
  double* hodnoty;

  char vsetky_typy[6][3] = { "RD\0", "RM\0", "RO\0", "PI\0", "PE\0", "PA\0" };

  do {
    scanf("%c", &command);
    if (command == 'v') v_stav = command_v(&dataloger_file, pocet_zoznamov, &ids, &pozicie, &typy, &hodnoty, &casy, &data);
    if (command == 'n') pocet_zoznamov = command_n(&dataloger_file, pocet_zoznamov, &ids, &pozicie, &typy, &hodnoty, &casy, &data);
    if (command == 'c') command_c(pocet_zoznamov, &ids, &data);
    if (command == 'h') command_h(pocet_zoznamov, vsetky_typy, &typy, &hodnoty);
    if (command == 'z') command_z(&dataloger_file, pocet_zoznamov, &ids, &pozicie, &typy, &hodnoty, &casy, &data);

  } while (command != 'k');

  free_all_arrays(&ids, &pozicie, &typy, &hodnoty, &casy, &data, pocet_zoznamov);
  fclose(dataloger_file);
  return 0;
  return 0;
}
