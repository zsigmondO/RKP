#include "bmpencoder.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <locale.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <omp.h>
#include <signal.h>

//A tiszta kódbázis érdekében nevesített konstansokat alkalmaztam.
//Bővebben ezekről és másokról a rövidített dokumentációban olvashat.
#define ENVFILENAME "sample.env"
#define MAXLINESIZE 512
#define ESC 27
#define KEY_ONE "NeptunID="
#define KEY_TWO "PostedText="
#define MAX_RESPONSE_LENGTH 4096
#define HEADER_BITMAP_LENGTH 16
#define MY_NEPTUNID "V7DCHS"

//24-bit bitmap bmp képhez tartozó header
typedef struct {
    char signature[2];
    int file_size;
    int number_of_hidden_chars;
    int pixel_array_offset;
} bitmap_header;

//Szépített hibakiírást megvalósító eljárás.
void error(const char *msg, int status) {
    fprintf(stderr, "%c[1m", ESC); //changes text to bold
    fprintf(stderr, "RKP main.c: ");

    fprintf(stderr, "\033[0;31m"); //changes color to red
    fprintf(stderr, "fatal error: ");
    fprintf(stderr, "%c[0m", ESC); //changes text back to normal non-bold text
    fprintf(stderr, "\033[0m"); //changes back to normal color

    fprintf(stderr, "%s\n", msg);

    exit(status);
}

//int argc: a parancssori argumentumok száma (argv mérete)
//char **argv: az argumentumokat tartalmazó 2 dimenziós karaktertömb
//A függvény a parancssori argumentumokat kezeli. Ezek alapján ad utasítást.
//Lehetséges return értékek az alprogram által:
//  Kód     Jelentés
//  111     Browser függvényt kell meghívni a továbbiakban
//  222     Képet adtunk meg parancssori argumentumként
//  1337    Ha valami nem volt rendben akkor a file nem kerül megnyitásra és ez az érték térül vissza.
//          (ez a main-ben van lekezelve)
//
//Lehetséges hibakódok az alprogram által:
//  Kód     Jelentés
//  11      Nem sikerült megnyitni a .env filet
//  12      Nem értelmezhető parancssori argumentum
//  13      Nincs olvasási / futtatási jogosultságunk a fájlhoz
int HandleArgv(int argc, char **argv) {
    int index_of_picture = 1337;

    if (argc == 1 || (argc == 2 && (strcmp(argv[1], "-fopenmp") == 0))) {
        printf("%c[1m", ESC); //changes text to bold
        printf("RKP main.c: ");
        printf("%c[0m", ESC); //changes text back to normal non-bold text

        printf("program startol.\n");
        printf("Tallózó került meghívásra.\n");

        return (111);

    } else if (argc == 2 && argv[1][0] != '-') {
        return (222);

    } else {
        int version_was_printed_before = 0;
        int help_was_printed_before = 0;
        int only_one_picture_is_allowed = 0;

        for (int i = 1; i < argc; ++i) {

            if (argv[i][0] == '-') {
                if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-version") == 0) {
                    if (!version_was_printed_before) {

                        version_was_printed_before = 1;
                        int saved_errno = 0;
                        FILE *fp = fopen(ENVFILENAME, "r");
                        saved_errno = errno;

                        if (!fp) {

                            error(".env fájl megnyitása sikertelen,\nfuttatás terminálva.", 11);

                        }

                        char line_buffer[MAXLINESIZE] = {0};

                        line_buffer[strlen(line_buffer) - 3] = '\0';

                        while (fgets(line_buffer, MAXLINESIZE, fp)) {
                            char *token = strtok(line_buffer, "=");
                            token = strtok(NULL, "=");

                            printf("%s", token);
                            token = strtok(NULL, "=");
                        }

                        puts("");

                        fclose(fp);

                    }
                } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-help") == 0) {
                    if (!help_was_printed_before) {

                        help_was_printed_before = 1;

                        printf("Program manuál:\n");
                        printf("%c[1m", ESC); //changes text to bold
                        printf("RKP tippek:\n");
                        printf("%c[0m", ESC); //changes text back to normal non-bold text

                        printf("A --version kapcsolóval "
                               "információkat szerezhet a programról.\n\n");

                        printf("Adjon meg egy TrueColor bmp képfájlt argumentumként a dekódoláshoz.\n");

                    }
                } else {
                    char error_msg[50];
                    strcpy(error_msg, "ismeretlen kapcsoló '");
                    strcat(error_msg, argv[i]);
                    strcat(error_msg, "' futtatás terminálva.");

                    error(error_msg, 12);
                }
            } else {
                if (!only_one_picture_is_allowed) {

                    only_one_picture_is_allowed = 1;

                    if (access(argv[i], F_OK) != 0 || access(argv[i], R_OK) != 0) {
                        char error_msg[50];
                        strcpy(error_msg, "fájl '");
                        strcat(error_msg, argv[i]);
                        strcat(error_msg, "' nem létezik,\n");
                        strcat(error_msg, "vagy nincs futtatási/írási engedély adva\nfuttatás terminálva.");

                        error(error_msg, 13);

                    } else {
                        printf("%c[1m", ESC); //changes text to bold
                        printf("RKP main.c: ");

                        printf("\033[0;32m"); //changes color to green
                        printf("%c[0m", ESC); //changes text back to normal non-bold text
                        printf("\033[0m"); //changes back to normal color

                        printf("fájl: '%s' sikeresen megtalálva!\n", argv[i]);

                        index_of_picture = i;

                    }
                }
            }
        }
    }

    return index_of_picture;
}

//A függvény logikai 1-essel tér vissza, amennyiben a vizsgált file egy reguláris file
//A függvény logikai 0-ással tér vissza, amennyiben a vizsgált file (mondjuk) egy könyvtár (azaz nem reguláris file)
//const char *path: a file-hoz vezető út
int IsRegularFile(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

//A BrowseForOpen() függvény miatt jött létre, amennyiben nem sikerül megnyitni bináris olvasásra a file-t
//a 14-es hibakóddal fog visszatérni az operációs rendszerhez.
//Az eljárás kétszer fordul elő összesen.
void PrettyErrorChecker(int fp) {
    if (fp < 0) {
        char *error_msg = "error during file opening (BrowseForOpen())\ncompilation terminated.\n";

        perror("Failed: ");

        error(error_msg, 14);
    }
}

//Karakteresen megvalósított fájltallózó függvény szabványos UNIX clear paranccsal kiegészítve. (Azaz windows-zal nem kompatibilis.)
//A legösszetettebb függvény komplex memóriafoglalási és felaszabadítási folyamatokkal.
//Bővebben a rövidített dokumentációban olvashat róla.
//Visszatérési értéke egy olyan 32 bites egész ami file leíróként funkcionál.
int BrowseForOpen() {
    DIR *dir;
    DIR *root;
    int number_of_files = 0;
    int files_in_root = 0;
    struct dirent *just_one_file_or_library;
    struct dirent **first_directory = calloc(100, sizeof(struct dirent *));
    struct dirent **root_directory = calloc(100, sizeof(struct dirent *));
    int to_be_returned_file = 0;

    dir = opendir(".");
    root = opendir("/");

    while ((just_one_file_or_library = readdir(dir)) != NULL) {
        first_directory[number_of_files] = just_one_file_or_library;
        number_of_files++;
        printf(">> %s\n", just_one_file_or_library->d_name);
    }

    while ((just_one_file_or_library = readdir(root)) != NULL) {
        root_directory[files_in_root] = just_one_file_or_library;
        files_in_root++;
    }

    char user_desired_destination[50];
    char *real_path = calloc(512, sizeof(char));
    struct dirent **all_the_directories;

    while (fgets(user_desired_destination, 50, stdin) != NULL) {
        user_desired_destination[strlen(user_desired_destination) - 1] = '\0';

        if (strcmp(user_desired_destination, "..") == 0) {
            if (first_directory != NULL) {
                if (number_of_files == files_in_root) {
                    int counter = 0;

                    for (int i = 0; i < files_in_root; ++i) {
                        if (strcmp(root_directory[i]->d_name, first_directory[i]->d_name) == 0) {
                            counter++;
                        }
                    }

                    if (counter == files_in_root) {
                        printf("Vigyázat: elértük a gyökér (/) könyvtárat.\n");
                        continue;
                    }
                }
            } else {
                if (number_of_files == files_in_root) {
                    int counter = 0;

                    for (int i = 0; i < files_in_root; ++i) {
                        if (strcmp(root_directory[i]->d_name, all_the_directories[i]->d_name) == 0) {
                            counter++;
                        }
                    }

                    if (counter == files_in_root) {
                        printf("Vigyázat: elértük a gyökér (/) könyvtárat.\n");
                        continue;
                    }
                }
            }
        }

        if (first_directory != NULL) {
            int did_we_find = 0;

            for (int i = 0; i < number_of_files; ++i) {
                if (strcmp(first_directory[i]->d_name, user_desired_destination) == 0) {
                    did_we_find = 1;
                    break;
                }
            }

            if (!did_we_find) {
                printf("Itt nincs ilyen nevű fájl: '%s'\n", user_desired_destination);
                continue;
            } else {
                number_of_files = 0;
                closedir(dir);

                char *temp_path = calloc(512, sizeof(char));

                strcpy(temp_path, real_path);
                strcat(temp_path, user_desired_destination);

                if (IsRegularFile(temp_path)) {
                    to_be_returned_file = open(temp_path, O_RDONLY);

                    free(root_directory);
                    free(real_path);
                    free(temp_path);
                    free(first_directory);
                    closedir(root);

                    PrettyErrorChecker(to_be_returned_file);

                    return to_be_returned_file;
                }
                free(temp_path);

                free(first_directory);
                first_directory = NULL;
                all_the_directories = calloc(100, sizeof(struct dirent *));
            }
        } else {
            int did_we_find = 0;

            for (int i = 0; i < number_of_files; ++i) {
                if (strcmp(all_the_directories[i]->d_name, user_desired_destination) == 0) {
                    did_we_find = 1;
                    break;
                }
            }

            if (!did_we_find) {
                printf("Itt nincs ilyen nevű fájl: '%s'\n", user_desired_destination);
                continue;
            } else {
                number_of_files = 0;
                closedir(dir);

                char *temp_path = calloc(512, sizeof(char));

                strcpy(temp_path, real_path);
                strcat(temp_path, user_desired_destination);

                if (IsRegularFile(temp_path)) {
                    strcat(real_path, user_desired_destination);

                    to_be_returned_file = open(temp_path, O_RDONLY);

                    free(root_directory);
                    free(real_path);
                    free(temp_path);
                    free(first_directory);
                    free(all_the_directories);
                    closedir(root);

                    PrettyErrorChecker(to_be_returned_file);

                    return to_be_returned_file;
                }
                free(temp_path);

                free(all_the_directories);
                all_the_directories = NULL;
                all_the_directories = calloc(100, sizeof(struct dirent *));
            }
        }

        strcat(real_path, user_desired_destination);
        strcat(real_path, "/");
        dir = opendir(real_path);

        system("clear");

        while ((just_one_file_or_library = readdir(dir)) != NULL) {
            all_the_directories[number_of_files] = just_one_file_or_library;

            printf(">> %s\n", just_one_file_or_library->d_name);
            number_of_files++;
        }
    }

    free(first_directory);
    free(root_directory);
    free(real_path);
    closedir(root);
    closedir(dir);

    return to_be_returned_file;
}

//Egy olyan eljárás amely felkészíti a programot a SIGALRM és a SIGINT jelekre.
void WhatToDo(int sig) {
    if (sig == SIGALRM) { //alarm signalt az időzítő küldi "alarm(n)"
        error("Hiba: a program túl sokáig futott, ezért leáll!\n", 7);
    } else if (sig == SIGINT) { // interrupt signal "program interrupt" azaz ctrl+c karakter-t kap
        pid_t pid;
        pid = fork();
        if (pid == 0) {
            printf("A program INTR (interrupt -> ctrl+c) karakterrel sem állítható le!\n");
            kill(getpid(), SIGKILL);
        }
    }
}

//A HTTP metódusok egyikét megvalósító függvény.
//A HTTP POST-ot adatküldésre használjuk egy szerver felé, vagy valamilyen erőforrás kreálására, frissítésére.
//A függvény 0-val tér vissza, ha sikeres volt a küldés (erről nyugta is készül kiíratás formájában).
//Minen más esetben az operációs rendszerhez ezen értékek kerülnek visszatérítésre:
//  Kód     Jelentés
//  2       Nem sikerült létrehozni a socketet
//  3       A megadott host nem létezik, vagy nem sikerült megtalálni
//  4       Nem sikerült a csatlakozás
//  5       Nem sikerült üzenetet írni a socketre
//  6       Nem sikerült elolvasni a szervertől érkezett választ
//  7       Nem sikerült elküldeni az üzenetet a szerverre
int Post(char *neptunID, char *message, int NumCh) {
    int portno = 80;
    char *host = "irh.inf.unideb.hu";
    char *message_fmt = "POST /~vargai/post.php HTTP/1.1\r\n"
                        "Host: irh.inf.unideb.hu\r\n"
                        "Content-Length: %d\r\n"
                        "Content-Type: application/x-www-form-urlencoded\r\n\r\n"
                        "%s%s&%s%s\r\n";

    /* A példában 33 */
    int real_content_length = strlen(KEY_ONE) +
                              strlen(KEY_TWO) +
                              strlen(neptunID) +
                              NumCh + 1;

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    int length_calculated = strlen(message_fmt) + real_content_length;
    char *message_to_send = (char *) calloc(length_calculated, sizeof(char));
    char response[MAX_RESPONSE_LENGTH];

    sprintf(message_to_send, 
            message_fmt,
            real_content_length,
            KEY_ONE,
            neptunID,
            KEY_TWO,
            message);

    /* socket létrehozása */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR a socket létrehozásánál", 2);

    /* megkeressük az ip címet a host alapján */
    server = gethostbyname(host);
    if (server == NULL) error("ERROR, nincs ilyen host", 3);

    /* struktúra feltöltése */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    /* socket csatlakozás */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR csatlakozásnál", 4);

    /* kérés küldése */
    total = strlen(message_to_send);
    sent = 0;

    do {
        bytes = write(sockfd, message_to_send + sent, total - sent);
        if (bytes < 0)
            error("ERROR nem sikerült üzenetet írni a socketre", 5);
        if (bytes == 0)
            break;
        sent += bytes;
    } while (sent < total);

    /* válasz fogadása */
    memset(response, 0, sizeof(response));
    total = sizeof(response) - 1;
    received = 0;

    do {
        bytes = read(sockfd, response + received, total - received);

        if (bytes < 0) {
            error("ERROR nem sikerült üzenetet olvasni a socketről", 6);
        }

        if (bytes == 0) {
            break;
        }

        received += bytes;

    } while (received < total);

    /* socket bezárása */
    close(sockfd);

    if (strstr(response, "The message has been received.") != NULL) {
        printf("Sikeres üzenetküldés!\n");
    } else {
        free(message_to_send);
        error("Az üzenet nem lett elküldve a szerverre!", 7);
    }

    free(message_to_send);
    return 0;
}

//A fent található struktúrát tölti fel a bitmap header részével.
//Visszatérési értéke egy bitmap_header struktúra.
//const u_char *array: Korábban binárisan beolvasott bájtokat tartalmaz.
bitmap_header pretty_formatter(const u_char *array) {
    bitmap_header to_be_returned;

    //0. és 1. byte
    if (array[0] != 66 || array[1] != 77) {
        error("Rossz fájlformátum!\nCsak BMP képek engedélyezettek!", 10);
    } else {
        to_be_returned.signature[0] = 'B';
        to_be_returned.signature[1] = 'M';
    }

    //2. 3. 4. 5. byte little endian :(
    to_be_returned.file_size = array[5];
    to_be_returned.file_size = (to_be_returned.file_size << 8) | array[4];
    to_be_returned.file_size = (to_be_returned.file_size << 8) | array[3];
    to_be_returned.file_size = (to_be_returned.file_size << 8) | array[2];

    //6. 7. 8. 9. byte
    to_be_returned.number_of_hidden_chars = array[9];
    to_be_returned.number_of_hidden_chars = (to_be_returned.number_of_hidden_chars << 8) | array[8];
    to_be_returned.number_of_hidden_chars = (to_be_returned.number_of_hidden_chars << 8) | array[7];
    to_be_returned.number_of_hidden_chars = (to_be_returned.number_of_hidden_chars << 8) | array[6];

    //10. 11. 12. 13.
    to_be_returned.pixel_array_offset = array[13];
    to_be_returned.pixel_array_offset = (to_be_returned.pixel_array_offset << 8) | array[12];
    to_be_returned.pixel_array_offset = (to_be_returned.pixel_array_offset << 8) | array[11];
    to_be_returned.pixel_array_offset = (to_be_returned.pixel_array_offset << 8) | array[10];

    return to_be_returned;
}

//Komplex, generikus párhuzamos programozás során használt folyamatokkal ellátott függvény. Bővebben a rövidített dokumentációban olvashat róla.
//Visszatérési értéke egy olyan karaktertömb, amely az elküldésre kész kikódolt üzenetet tartalmazza.
//char *Pbuff: Nyers pixel_array tömb.
//int NumCh: Kódolt karakterek száma. (A méret ennek a számnak a háromszorosa, hisz 3 szín alkot egy pixelt (és a padding ha van)).
//Az 1-es hibakód minden esetben a memória allokálás hibáját jelzi.
//A háttérben elkészül egy log.txt is amely a párhuzamos programozás hitelességét jelzi.
char *Unwrap(char *Pbuff, int NumCh) {
    int length = NumCh * 3;
    char *to_be_returned_array = (char *) calloc(NumCh + 1, sizeof(char));
    int indexer = 0;

    if (to_be_returned_array == NULL) {
        error("Hiba: memóriafoglalás nem sikerült.", 1);
    }

    int green = 1;
    int red = 2;

    int max_threads = omp_get_max_threads();

    omp_set_num_threads(max_threads);

    int proof[max_threads];

    for (int i = 0; i < max_threads; ++i) {
        proof[i] = 0;
    }

    for (int blue = 0; blue < length; blue += 3) {
        #pragma omp parallel default(none) shared(Pbuff, blue, green, red, to_be_returned_array, indexer, proof)
        {
            char c_blue = Pbuff[blue];
            char mask = 0x03;
            c_blue = c_blue & mask;

            char c_green = Pbuff[green];
            mask = 0x07;
            c_green = c_green & mask;

            char c_red = Pbuff[red];
            mask = 0x07;
            c_red = c_red & mask;

            to_be_returned_array[indexer] = (c_blue << 3) | c_green;
            to_be_returned_array[indexer] = (to_be_returned_array[indexer] << 3) | c_red;

            proof[omp_get_thread_num()]++;
        }

        #pragma omp critical
        {
            green += 3;
            red += 3;
            indexer++;
        }
    }

    free(Pbuff);
    Pbuff = NULL;

    FILE *proof_file = fopen("log.txt", "w");

    if (proof_file == NULL) {
        error("Hiba a log.txt csinálásnál!\n", 10);
    }

    for (int i = 0; i < max_threads; ++i) {
        fprintf(proof_file, "Szál sorszáma: %d. Ennyiszer dolgozott: %d\n", i, proof[i]);
    }

    fclose(proof_file);
    proof_file = NULL;

    return to_be_returned_array;
}

//Bináris olvasást végző függvény.
//Visszatérési értéke a nyers pixel_array.
//int f: 32 bites egész ami fájleíróként funkcionál.
//int *NumCh: Kódolt karakterek száma, outputként működik.
//A függvény az operációs rendszer felé 0-val tér vissza, ha nem tartalmaz rejtett szöveget a kép.
//Az 1-es hibakód minden esetben a memória allokálás hibáját jelzi.
char *ReadPixels(int f, int *NumCh) {
    u_char buf[HEADER_BITMAP_LENGTH] = {0};
    size_t nbytes;
    ssize_t bytes_read;
    nbytes = sizeof(buf);
    bytes_read = read(f, buf, nbytes);

    bitmap_header container = pretty_formatter(buf);

    if (container.number_of_hidden_chars == 0) {
        error("Ez a kép nem tartalmaz rejtett szöveget.", 0);
    }

    *NumCh = container.number_of_hidden_chars;
    char *pixel_array_text = (char *) calloc(container.number_of_hidden_chars * 3, sizeof(char));

    if (pixel_array_text == NULL) {
        error("Hiba: memóriafoglalás nem sikerült.", 1);
    }

    lseek(f, container.pixel_array_offset, SEEK_SET);
    read(f, pixel_array_text, container.number_of_hidden_chars * 3);

    return pixel_array_text;
}

//A fent látható bitmap_header struktúra csinos kiíratását végzi el.
//bitmap_header container: egy struktúra, melyből az adatok származnak.
//Ez az eljárás egyszer sem szerepel. Hibakezelés céljából jött létre.
void pretty_header_print(bitmap_header container) {
    printf("Kép aláírása: %s\n", container.signature);
    printf("Fájlméret (bájtokban): %d\n", container.file_size);
    printf("Rejtett karakterek száma a képben: %d\n", container.number_of_hidden_chars);
    printf("Eltolás (offset) mérete: %d\n", container.pixel_array_offset);
}

//A 2. feladatban törlésre ítélt függvény, amely már nem fontos.
/*
har random_char_generator()
{
    return (rand() % (UPPER - LOWER + 1)) + LOWER;
}

char* test_array(int* number_of_decoded_chars)
{
    char* to_be_returned = calloc(DECODEDAMOUNT, sizeof(char));

    int j = strlen(TODECODE) - 1;

    for (int i = DECODEDAMOUNT - 1; i >= DECODEDAMOUNT - strlen(TODECODE) -1; --i)
    {
        to_be_returned[i] = TODECODE[j];
        j--;
    }

    for (int i = 0; i < DECODEDAMOUNT - strlen(TODECODE); ++i)
    {
        char get_random_char = random_char_generator();

        to_be_returned[i] = get_random_char;
    }

    int temp = strlen(TODECODE);    //Getting rid of the warning.

    *number_of_decoded_chars = temp;
    return to_be_returned;
}*/
