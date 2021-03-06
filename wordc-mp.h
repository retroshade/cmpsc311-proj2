// Project 2-- Hayley Sundra and Ryan Mysliwiec
//  wordc-mp.h
//  


#ifndef wordc_h
#define wordc_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct list {
    char word[30];
    int count;
    struct list *next;
};

typedef int bool;
#define true 1
#define false 0

// function prototypes
struct list* add_first_list_element (char val[30], int j);
struct list* add_additional_list_element (char val[30], int j);
struct list* add_additional_list_element_parent (char val[30], int j, int count);
bool search_list (char val[30]);
struct list* find_existing_node (char val[30]);
struct list* find_prior_node (char val[30]);
void print_list (FILE *file2);
void sort(struct list *new);

#endif /* wordc_mp_h */
