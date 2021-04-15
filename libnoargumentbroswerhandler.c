#include "noargumentbroswerhandler.h"
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <locale.h>

#define ESC 27

int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void pretty_file_error_checker(FILE *fp) {
   if (fp == NULL) {
      printf("%c[1m", ESC); //changes text to bold
      printf("RKP main.c: ");

      printf("\033[0;31m"); //changes color to red
      printf("fatal error: ");
      printf("%c[0m", ESC); //changes text back to normal non-bold text
      printf("\033[0m"); //changes back to normal color
      
      printf("error during file opening (broswe_for_open())\n");
      printf("compilation terminated.\n");

      perror("Failed: ");
      exit(1);
   }
}

FILE *broswe_for_open() {
    DIR *dir;
    DIR *root;
    int number_of_files = 0;
    int files_in_root = 0;
    struct dirent *just_one_file_or_library;
    struct dirent **first_directory = calloc(100, sizeof(struct dirent *));
    struct dirent **root_directory = calloc(100, sizeof(struct dirent *));
    FILE *to_be_returned_file = NULL;

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
                        printf("Error: root directory (/) reached.\n");
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
                        printf("Error: root directory (/) reached.\n");
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
                printf("Typo: '%s'\n", user_desired_destination);
                continue;
            } else {
                number_of_files = 0;
                closedir(dir);

                char *temp_path = calloc(512, sizeof(char));

                strcpy(temp_path, real_path);
                strcat(temp_path, user_desired_destination);

                if (is_regular_file(temp_path)) {
                    to_be_returned_file = fopen(temp_path, "rb");

                    free(root_directory);
                    free(real_path);
                    free(temp_path);
                    free(first_directory);
                    closedir(root);

                    pretty_file_error_checker(to_be_returned_file);

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
                printf("Typo: '%s'\n", user_desired_destination);
                continue;
            } else {
                number_of_files = 0;
                closedir(dir);

                char *temp_path = calloc(512, sizeof(char));

                strcpy(temp_path, real_path);
                strcat(temp_path, user_desired_destination);

                if (is_regular_file(temp_path)) {
                    strcat(real_path, user_desired_destination);

                    to_be_returned_file = fopen(temp_path, "rb");

                    free(root_directory);
                    free(real_path);
                    free(temp_path);
                    free(first_directory);
                    free(all_the_directories);
                    closedir(root);

                    pretty_file_error_checker(to_be_returned_file);

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