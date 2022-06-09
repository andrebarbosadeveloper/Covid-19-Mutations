#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <semaphore.h>

#define N 10
#define NProds 1
#define NCons 6

typedef struct shelf
{
  char *path;
  pthread_t thread_id;
  struct shelf *pnext;
} SHELF;

typedef struct params
{
  char *path;
  char **args;
  int numArgs;
} PARAMS;

char *buf[N];

int prodptr = 0, consptr = 0;

pthread_mutex_t trinco = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t trinco_p = PTHREAD_MUTEX_INITIALIZER; // Não seria preciso uma vez que só existe 1 produtor
pthread_mutex_t trinco_c = PTHREAD_MUTEX_INITIALIZER;

static const char *semNameProd = "semPodeProd";
sem_t semPodeProd;

static const char *semNameCons = "semPodeCons";
sem_t semPodeCons;

typedef struct stackProdutor
{
  int pos;
  char *path;
} STACKPRODUTOR;

typedef struct StackLinkedChars_Node
{
  STACKPRODUTOR item;
  struct StackLinkedChars_Node *next;
} STACKLINKEDCHARS_NODE;

void launchProdCons(PARAMS *parameters);
char *preparePath(int argc_AUX, char *argv_AUX[], int *i);
void *produtor(void *param);
void *consumidor(void *param);
char *produz(char *path, int pos);
void consome(void *params, char *dir);
int argumentFinder(char *base_path, char *args[], int num_args, struct dirent *entry);
int nameCommand(char *base_path, char *args[], int num_args, struct dirent *entry);
int iNameCommand(char *base_path, char *args[], int num_args, struct dirent *entry);
int typeCommand(char *base_path, char *args[], int num_args, struct dirent *entry);
unsigned char getType(char c);
int emptyCommand(char *base_path, char *args[], int num_args, struct dirent *entry);
int executableCommand(char *base_path, char *args[], int num_args, struct dirent *entry);
int mminCommand(char *base_path, char *args[], int num_args, struct dirent *entry);
int sizeCommand(char *base_path, char *args[], int num_args, struct dirent *entry);
int isDirectoryEmpty(char *dirname);
long int getSize(char *str);
void printList(SHELF *aux);
char *reallocBuffer(char *str);
void addToList(char *pathToPrint);
int isEmpty_StackLinkedChars(STACKLINKEDCHARS_NODE *first);
void push_StackLinkedChars(STACKLINKEDCHARS_NODE **first, STACKPRODUTOR item);
STACKPRODUTOR pop_StackLinkedChars(STACKLINKEDCHARS_NODE **first);

int k = 0, l = 0, m = 0, n = 0, o = 0, p = 0, q = 0; // Usadas nas funçãos de comandos

SHELF *first = NULL;
SHELF *last = NULL;

int main(int argc, char *argv[])
{
  /*for (int i = 0; i < N; i++)
  {
    strcpy(buf[i], "");
  }*/

  int i = 0;
  //char *main_path = malloc(sizeof(char) * 300);

  char *main_path = preparePath(argc, argv, &i);

  PARAMS *parameters = (PARAMS *)malloc(sizeof(PARAMS));
  parameters->path = main_path;
  parameters->args = argv + 1 + i;
  parameters->numArgs = argc - 1 - i;

  first = (SHELF *)malloc(sizeof(SHELF));
  first->pnext = NULL;
  first->path = main_path;
  last = first;
   
  launchProdCons(parameters);
  printList(first);

  pthread_exit(NULL);
  return 0;
}

/**
 * Lança NProds threads Produtoras e NCons threads Consumidoras
 */
void launchProdCons(PARAMS *parameters)
{
  SHELF th_data_array_prod[NProds];
  SHELF th_data_array_cons[NCons];

  sem_init(&semPodeProd, 0, N);
  sem_init(&semPodeCons, 0, 0);

  for (int i = 0; i < NProds; i++)
  {
    pthread_create(&th_data_array_prod[i].thread_id, NULL, &produtor, parameters);
  }

  for (int i = 0; i < NCons; i++)
  {
    pthread_create(&th_data_array_cons[i].thread_id, NULL, &consumidor, parameters);
  }

  pthread_join(th_data_array_prod[0].thread_id, NULL);

  //Esperar pelos consumidores criados
  for (int i = 0; i < NCons; i++)
  {
    pthread_join(th_data_array_cons[i].thread_id, NULL);
  }
}

char *reallocBuffer(char *str){
  char *newStr = malloc(strlen(str)+1);
  strcpy(newStr, str);
  return newStr;
}

/**
 * Código a executar pelas threads produtoras
 */
void *produtor(void *param)
{
  PARAMS *params_thread;
  params_thread = (PARAMS *)param;

  STACKLINKEDCHARS_NODE *first = NULL;
  STACKPRODUTOR producedItem = {0, params_thread->path};
  //printf("-->%s\n", params_thread->path);

  sem_wait(&semPodeProd);
  pthread_mutex_lock(&trinco_p); // Não seria preciso uma vez que só existe 1 produtor
  //strcpy(buf[prodptr], params_thread->path);
  buf[prodptr]=reallocBuffer(params_thread->path);
  prodptr = (prodptr + 1) % N;
  pthread_mutex_unlock(&trinco_p); // Não seria preciso uma vez que só existe 1 produtor
  sem_post(&semPodeCons);

  push_StackLinkedChars(&first, producedItem);
  while (!isEmpty_StackLinkedChars(first)) // Enquanto não está vazio
  {
    char *dir_p = produz(producedItem.path, producedItem.pos);
    producedItem.pos++; // Faz a gestão, caso hajam mais do que 1 diretorio no diretorio passado
    if (dir_p == NULL)  // Não encontrou mais diretórios quero voltar para o anterior
    {
      producedItem = pop_StackLinkedChars(&first);
      continue; // Quero produzir o novo diretório (o anterior)
    }
    push_StackLinkedChars(&first, producedItem); // Push do anterior
    //New path
    producedItem.path = dir_p;
    producedItem.pos = 0;

    sem_wait(&semPodeProd);
    pthread_mutex_lock(&trinco_p);
    //strcpy(buf[prodptr], dir_p);
    buf[prodptr]=reallocBuffer(dir_p);
    prodptr = (prodptr + 1) % N;
    pthread_mutex_unlock(&trinco_p);
    sem_post(&semPodeCons);
  }


  //ciclo for
  for (int i = 0; i < NCons; i++)
  {
    sem_wait(&semPodeProd);
    pthread_mutex_lock(&trinco_p);
    //strcpy(buf[prodptr], "EnD1230"); //String para paragem do consumidor
    buf[prodptr]=reallocBuffer("EnD1230");
    prodptr = (prodptr + 1) % N;
    pthread_mutex_unlock(&trinco_p);
    sem_post(&semPodeCons);
  }
  //fecha for

  pthread_exit(NULL);
}

/**
 * Código a executar pelas threads consumidoras
 */
void *consumidor(void *param)
{
  while (1)
  {
    char *dir_c;
    sem_wait(&semPodeCons);

    pthread_mutex_lock(&trinco_c);

    //strcpy(dir_c, buf[consptr]);
    dir_c = buf[consptr];
    buf[consptr]=NULL;

    //strcpy(buf[consptr], "");
    consptr = (consptr + 1) % N;
    pthread_mutex_unlock(&trinco_c);

    sem_post(&semPodeProd);

    if (strcmp(dir_c, "EnD1230") == 0) //Condição de paragem dos consumidores
    {
      break;
    }

    consome(param, dir_c);
    //free(dir_c);
  }
  pthread_exit(NULL);
}

/**
 * Procura diretórios no "path" passado, se encontrar retorna o path em questão 
 */
char *produz(char *path, int pos)
{
  int countPos = 0;
  DIR *dir;
  struct dirent *entry;
  
  //printf("path: %s\n", path);

  if ((dir = opendir(path)) == NULL)
  {
    perror("opendir() error");
  }
  else
  {
    while ((entry = readdir(dir)) != NULL)
    {
      if (strcmp(entry->d_name, ".") > 0 && strcmp(entry->d_name, "..") > 0)
      {
        if (entry->d_type == DT_DIR)
        {
          if (countPos == pos)
          {
            //barra e '/0'
            char *pathAUX = malloc(strlen(path)+strlen(entry->d_name)+2);
            sprintf(pathAUX, "%s%s/", path, entry->d_name);
            return pathAUX;
          }
          else
          {
            countPos++;
          }
        }
      }
    }
    closedir(dir);
  }
  return NULL; //Caso não encontre diretorios
}

/**
 * Adiciona á lista os ficheiros em que se verificam as condições passadas em ARGV
 */
void consome(void *params, char *dirName)
{

  DIR *dir;
  struct dirent *entry;
  PARAMS *parameters = (PARAMS *)params;

  if ((dir = opendir(dirName)) == NULL)
    perror("opendir() error");
  else
  {
    //printf("contents of root %s:\n", dirName);
    while ((entry = readdir(dir)) != NULL)
    {
      if (strcmp(entry->d_name, ".") > 0 && strcmp(entry->d_name, "..") > 0)
      {
        //printf("\n%s%s\n", dirName, entry->d_name);
        int flagPrint = argumentFinder(dirName, parameters->args, parameters->numArgs, entry);
        if (flagPrint) //Existe algum path compativel com os comandos inseridos
        {
          //char *pathToPrint = (char *)calloc(sizeof(char) * 200, 1);
          char *pathToPrint = (char *)malloc(sizeof(char) * (strlen(dirName)+strlen(entry->d_name)+1));
          sprintf(pathToPrint, "%s%s", dirName, entry->d_name);
          addToList(pathToPrint); // Trinco dentro da função
        }
      }
    }
    closedir(dir);
  }
}

/**
 * Verfica quais os comandos inseridos pelo utilizador e a sua válida correspondência ou não.
 */
int argumentFinder(char *base_path, char *args[], int num_args, struct dirent *entry)
{

  if (nameCommand(base_path, args, num_args, entry) && iNameCommand(base_path, args, num_args, entry) &&
      typeCommand(base_path, args, num_args, entry) && emptyCommand(base_path, args, num_args, entry) &&
      executableCommand(base_path, args, num_args, entry) && sizeCommand(base_path, args, num_args, entry) &&
      mminCommand(base_path, args, num_args, entry))
  {
    //printf("%s%s\n", base_path, entry->d_name);
    return 1;
  }
  return 0;
}

/**
 * Command Input: myfind . -name data.txt (EX)
 */
int nameCommand(char *base_path, char *args[], int num_args, struct dirent *entry)
{
  int pos, found = 0;
  for (pos = 0; pos < num_args; pos++)
  {
    if (strcmp(args[pos], "-name") == 0)
    {
      found = 1;
      break;
    }
  }
  // Caso não exista nenhum argumento correspondente a '-name' então retornamos 1 para que seja possivel verificar a existência de outros comandos.
  if (!found)
    return 1;

  // Verifica se o argumento seguinte a '-name' existe
  if (args[pos + 1] == NULL || args[pos + 1][0] == '-')
  {
    if (n == 0)
    {
      printf("myfind: argumento em falta para '-name'\n");
      n++;
    }
    return 0;
  }

  //myfind . -name data.txt
  // Se o argumento seguinte a '-name' corresponder ao ficheiro atual, então encontrou-se uma correspondência
  if (strcmp(entry->d_name, args[pos + 1]) == 0)
  {
    return 1;
  }

  //myfind . -name ‘w2*’
  char *aux;
  //strcpy(aux, args[pos + 1]);
  aux = reallocBuffer(args[pos+1]);

  char *tok = strtok(aux, "*");
  char *tmp;
  char *name;
  //strcpy(name, entry->d_name);
  name = reallocBuffer(entry->d_name);
  /*if (name[0]!='*')
  { // Se tiver * no inicio
  }*/
  while (tok != NULL)
  {
    tmp = strstr(name, tok);
    if (tmp == NULL)
      return 0;

    strcpy(name, tmp);

    tok = strtok(NULL, "*");
  }
  /*if (name[strlen(name) - 1]!='*')
  { // Se tiver * no fim
  }*/
  return 1;
}

/**
 * Command Input: myfind . -iname Data.txt (EX)
 */
int iNameCommand(char *base_path, char *args[], int num_args, struct dirent *entry)
{
  int pos, found = 0;
  for (pos = 0; pos < num_args; pos++)
  {
    if (strcmp(args[pos], "-iname") == 0)
    {
      found = 1;
      break;
    }
  }
  // Caso não exista nenhum argumento correspondente a '-iname' então retornamos 1 para que seja possivel verificar a existência de outros comandos.
  if (!found)
    return 1;

  // Verifica se o argumento seguinte a '-iname' existe
  if (args[pos + 1] == NULL || args[pos + 1][0] == '-')
  {
    if (o == 0)
    {
      printf("myfind: argumento em falta para '-iname'\n");
      o++;
    }
    return 0;
  }

  //myfind . -iname data.txt
  // Se o argumento seguinte a '-iname' corresponder ao ficheiro atual, então encontrou-se uma correspondência
  if (strcasecmp(entry->d_name, args[pos + 1]) == 0)
  {
    return 1;
  }

  //myfind . -name ‘w2*’
  char *aux;
  //strcpy(aux, args[pos + 1]);
  aux = reallocBuffer(args[pos+1]);

  // Converter toda a string para minúsculas
  for (int i = 0; aux[i]; i++)
  {
    aux[i] = tolower(aux[i]);
  }

  char *tok = strtok(aux, "*");
  char *tmp;
  char *name;
  //strcpy(name, entry->d_name);
  name = reallocBuffer(entry->d_name);

  // Converter toda a string para minúsculas
  for (int i = 0; name[i]; i++)
  {
    name[i] = tolower(name[i]);
  }

  while (tok != NULL)
  {
    tmp = strstr(name, tok);
    if (tmp == NULL)
      return 0;

    strcpy(name, tmp);
    tok = strtok(NULL, "*");
  }
  return 1;
}

/**
 * Command Input: myfind . -type d (EX)
 */
int typeCommand(char *base_path, char *args[], int num_args, struct dirent *entry)
{
  int pos, found = 0;
  for (pos = 0; pos < num_args; pos++)
  {
    if (strcmp(args[pos], "-type") == 0)
    {
      found = 1;
      break;
    }
  }
  // Caso não exista nenhum argumento correspondente a '-type' então retornamos 1 para que seja possivel verificar a existência de outros comandos.
  if (!found)
    return 1;

  // Verifica se o argumento seguinte a '-type' existe
  if (args[pos + 1] == NULL || args[pos + 1][0] == '-')
  {
    if (k == 0)
    {
      printf("myfind: argumento em falta para '-type'\n");
      k++;
    }
    return 0;
  }

  unsigned char type;
  char tmp[50];
  strcpy(tmp, args[pos + 1]);
  // Faz-se o strtok porque é possível executar comandos como o seguinte exemplo: -type d,f
  char *tok = strtok(tmp, ",");
  while (tok != NULL)
  {
    type = getType(tok[0]);
    // Se o argumento seguinte a '-name' corresponder ao ficheiro atual, então encontrou-se uma correspondência
    if (type == entry->d_type)
    {
      return 1;
    }
    tok = strtok(NULL, ",");
  }
  return 0;
}

/**
 * Command Input: myfind . -empty (EX)
 */
int emptyCommand(char *base_path, char *args[], int num_args, struct dirent *entry)
{
  int pos, found = 0;
  for (pos = 0; pos < num_args; pos++)
  {
    if (strcmp(args[pos], "-empty") == 0)
    {
      found = 1;
      break;
    }
  }
  // Caso não exista nenhum argumento correspondente a '-empty' então retornamos 1 para que seja possivel verificar a existência de outros comandos.
  if (!found)
    return 1;

  // Verifica se o argumento seguinte a '-empty' existe
  if (args[pos + 1] != NULL && args[pos + 1][0] == '-' && isdigit(args[pos + 1][1]))
  {
    if (q == 0)
    {
      printf("myfind:paths devem proceder a expressão:'%s'\n", args[pos + 1]);
      q++;
    }
    return 0;
  }
  struct stat file_stat;
  char aux[350];
  sprintf(aux, "%s%s", base_path, entry->d_name);
  stat(aux, &file_stat);
  if (entry->d_type == DT_DIR)
  {
    // Se for um diretório e estiver vazio
    if (isDirectoryEmpty(aux))
    {
      return 1;
    }
  }
  // Se o ficheiro estiver vazio
  else if (file_stat.st_size == 0)
  {
    return 1;
  }
  return 0;
}

/**
 * Command Input: myfind . -executable (EX)
*/
int executableCommand(char *base_path, char *args[], int num_args, struct dirent *entry)
{
  int pos, found = 0;
  for (pos = 0; pos < num_args; pos++)
  {
    if (strcmp(args[pos], "-executable") == 0)
    {
      found = 1;
      break;
    }
  }
  // Caso não exista nenhum argumento correspondente a '-executable' então retornamos 1 para que seja possivel verificar a existência de outros comandos.
  if (!found)
    return 1;

  // Verifica se o argumento seguinte a '-executable' existe
  if (args[pos + 1] != NULL && (args[pos + 1][0] != '-'))
  {
    if (p == 0)
    {
      printf("myfind:paths devem proceder a expressão:'%s'\n", args[pos + 1]);
      p++;
    }
    return 0;
  }

  struct stat file_stat;
  char aux[350];
  sprintf(aux, "%s%s", base_path, entry->d_name);
  stat(aux, &file_stat);
  // Se o ficheiro for do tipo executável
  if ((file_stat.st_mode & S_IXUSR) || (file_stat.st_mode & S_IXGRP) || (file_stat.st_mode & S_IXOTH))
  {
    return 1;
  }
  return 0;
}

/**
 * Command Input: myfind . -mmin -60 (EX)
 */
int mminCommand(char *base_path, char *args[], int num_args, struct dirent *entry)
{
  int pos, found = 0;
  for (pos = 0; pos < num_args; pos++)
  {
    if (strcmp(args[pos], "-mmin") == 0)
    {
      found = 1;
      break;
    }
  }
  // Caso não exista nenhum argumento correspondente a '-mmin' então retornamos 1 para que seja possivel verificar a existência de outros comandos.
  if (!found)
    return 1;

  // Verifica se o argumento seguinte a '-mmin' existe
  if (args[pos + 1] == NULL || (args[pos + 1][0] == '-' && !isdigit(args[pos + 1][1])))
  {
    if (m == 0)
    {
      printf("myfind: argumento em falta para '-mmin'\n");
      m++;
    }
    return 0;
  }

  struct stat file_stat;
  char aux[350];
  sprintf(aux, "%s%s", base_path, entry->d_name);
  stat(aux, &file_stat);
  time_t modifiedTime = file_stat.st_mtime; // seconds
  time_t tempo;
  time(&tempo);
  int min2sec;
  char input[10];
  strcpy(input, args[pos + 1]);
  // Verifica conforme o input qual das verifcações será válida
  // Ex: -mmin -60    ->    if (tempo - modifiedTime < 60*60) return 1
  switch (args[pos + 1][0])
  {
  case '+':
    min2sec = atoi(input + 1) * 60;
    if (tempo - modifiedTime > min2sec)
      return 1;
    break;
  case '-':
    min2sec = atoi(input + 1) * 60;
    if (tempo - modifiedTime < min2sec)
      return 1;
    break;
  default:
    min2sec = atoi(input) * 60;
    if (tempo - modifiedTime == min2sec)
      return 1;
    break;
  }
  return 0;
}

/**
 * Command Input: myfind . -size +5M (EX)
 */
int sizeCommand(char *base_path, char *args[], int num_args, struct dirent *entry)
{
  int pos, found = 0;
  for (pos = 0; pos < num_args; pos++)
  {
    if (strcmp(args[pos], "-size") == 0)
    {
      found = 1;
      break;
    }
  }
  // Caso não exista nenhum argumento correspondente a '-size' então retornamos 1 para que seja possivel verificar a existência de outros comandos.
  if (!found)
    return 1;

  // Verifica se o argumento seguinte a '-size' existe
  if (args[pos + 1] == NULL || (args[pos + 1][0] == '-' && !isdigit(args[pos + 1][1])))
  {
    if (l == 0)
    {
      printf("myfind: argumento em falta para '-size'\n");
      l++;
    }
    return 0;
  }

  struct stat file_stat;
  char aux[350];
  sprintf(aux, "%s%s", base_path, entry->d_name);
  stat(aux, &file_stat);
  // Verifica conforme o input qual das verifcações será válida
  // Ex: -size +5M    ->    if(file_stat.st_size > 5*1048576 (bytes)) return 1
  switch (args[pos + 1][0])
  {
  case '+':
    if (file_stat.st_size > getSize(args[pos + 1]))
      return 1;
    break;
  case '-':
    if (file_stat.st_size < getSize(args[pos + 1]))
      return 1;
    break;
  default:
    if (file_stat.st_size == getSize(args[pos + 1]))
      return 1;
    break;
  }
  return 0;
}

/**
 * Conforme char recebido, retorna o tipo correspondente
 * Ex: -type d, retorna DT_DIR
 */
unsigned char getType(char c)
{
  switch (c)
  {
  case 'b':
    return DT_BLK;
  case 'c':
    return DT_CHR;
  case 'd':
    return DT_DIR;
  case 'p':
    return DT_FIFO;
  case 'f':
    return DT_REG;
  case 'l':
    return DT_LNK;
  case 's':
    return DT_SOCK;
  default:
    return DT_UNKNOWN;
  }
}

/**
 * Verifica se o diretório passado encontra-se vazio
 */
int isDirectoryEmpty(char *dirname)
{
  int n = 0;
  struct dirent *d;
  DIR *dir = opendir(dirname);
  if (dir == NULL) //Not a directory or doesn't exist
    return 1;
  while ((d = readdir(dir)) != NULL)
  {
    if (++n > 2)
      break;
  }
  closedir(dir);
  if (n <= 2) //Directory Empty
    return 1;
  else
    return 0;
}

/**
 *  Obter o tamanho númerico da string passada no argumento
 *  Ex: Recebe "5M", retorna 5*1048576 (bytes)
 */
long int getSize(char *str)
{
  int i = 1;
  char aux[200];
  strcpy(aux, str);
  char *temp = strtok(aux, "");
  if (isdigit(temp[0]))
    i = 0;
  switch (*(str + strlen(str) - 1))
  {
  case 'b':
    return 512 * atoi(temp + i);
  case 'c':
    return atoi(temp + i);
  case 'w':
    return 2 * atoi(temp + i);
  case 'k':
    return 1024 * atoi(temp + i);
  case 'M':
    return 1048576 * atoi(temp + i);
  case 'G':
    return 1073741824 * atoi(temp + i);
  default:
    return atoi(temp + i);
  }
}

/**
 * Imprimir conteudo de uma Lista ligada
 */
void printList(SHELF *aux)
{
  int num_prints = -1;
  pthread_t thread_id = 0;
  SHELF *current = aux;
  //printf("\nLINKED LIST:\n");
  while (current != NULL)
  {
    if (thread_id != current->thread_id)
      pthread_join(current->thread_id, NULL);
    printf("%s \t -> %ld\n", current->path, current->thread_id);
    num_prints++;
    current = current->pnext;
    // Guarda o thread_id da thread em questão exceto se esta for a mainThread, ou seja, a última
    if (current != NULL)
      thread_id = current->thread_id;
  }
  printf("\nNum of Matches: %d\n", num_prints);
}

/**
 * Adicionar um novo path à cauda da Fila (Lista Ligada)
 */
void addToList(char *pathToPrint)
{
  pthread_mutex_lock(&trinco);
  last->pnext = (SHELF *)malloc(sizeof(SHELF));
  last = last->pnext;
  last->path = pathToPrint;
  last->pnext = NULL;
  last->thread_id = pthread_self();
  pthread_mutex_unlock(&trinco);
}

/**
 * Vericia se foi inserido um path em argumento, senão coloca o path './' por defeito
 */
char *preparePath(int argc_AUX, char *argv_AUX[], int *i)
{
  *i = 0;
  char *path = malloc(sizeof(char) * 300);
  if (argc_AUX > 1)
  {
    // Se não tiver caminho especifico atribui-se o ./ como caminho
    if (argv_AUX[1][0] == '-')
    {
      strcpy(path, "./");
    }
    else
    {
      *i = 1;
      strcpy(path, argv_AUX[1]);
    }
  } // Se tentarmos excutar apenas 'myfind'
  else
  {
    strcpy(path, "./");
  }
  // Se no final de 'path' não tiver '/' coloca-se
  if (path[strlen(path) - 1] != '/')
  {
    strcat(path, "/");
  }
  //printf("%s\n", path);
  return path;
}

int isEmpty_StackLinkedChars(STACKLINKEDCHARS_NODE *first)
{
  return first == 0;
}

void push_StackLinkedChars(STACKLINKEDCHARS_NODE **first, STACKPRODUTOR item)
{
  STACKLINKEDCHARS_NODE *oldfirst = *first;
  (*first) = malloc(sizeof(STACKLINKEDCHARS_NODE));
  (*first)->item = item;
  (*first)->next = oldfirst;
}

STACKPRODUTOR pop_StackLinkedChars(STACKLINKEDCHARS_NODE **first)
{
  STACKLINKEDCHARS_NODE *oldfirst = *first;
  STACKPRODUTOR item = (*first)->item;
  (*first) = (*first)->next;
  free(oldfirst);
  return item;
}