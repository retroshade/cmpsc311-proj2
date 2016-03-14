//
//  wordc.c
//  
//
//  Created by Hayley M. Sundra on 2/22/16.
//
//

#include "wordc.h"

struct list *head;
struct list *current;

int main(int argc, char **argv) {
    
    // declare clock variables and start the clock
    clock_t begin, end;
    double execution_time;
    begin = clock();
    
    char * arg1, * arg2, * arg3;
    arg1 = argv[1];
    arg2 = argv[2];
    arg3 = argv[3];
    
    // open the files
    FILE *file1 = fopen(arg1, "r");
    FILE *file2 = fopen(arg2, "w");
    FILE *file3 = fopen(arg3, "w");
    
    // check to make sure that the files opened correctly. if not, exit the program with an error.
    if (file1 == NULL | file2 == NULL | file3 == NULL) {
        perror("Error opening file");
        return(-1);
    }
    
    head = (struct list*) malloc (sizeof (struct list));
    current = (struct list*) malloc (sizeof (struct list));
    //head->word = 0;
    head->count = 0;
    head->next = 0;
    current = head;
    
    //printf("Allocated space for head and current\n");
    
    
    int input_text;
    char temp_word[30] = {(char)0};
    char temp_letter;
    int i = 0;
    while ((input_text = fgetc(file1)) != EOF) {
        printf("Parsing file\n");
        if (isalpha(input_text)) {
            // convert it to an alpha and store it in array
            input_text = tolower(input_text);
            temp_letter = (char)(input_text);
            temp_word[i] = temp_letter;
            printf("%s\n", temp_word);
            i++;
        }
        else if ((input_text == ' ') | (input_text == '\n')) { // there is a space so the word should end
            // the word should be added to the linked list.
            printf("Word has ended\n");
            
            if (temp_word[0] == '\0') {}
            
            else if (head->word[0] == '\0') {
                printf("head is empty\n");
                add_first_list_element(temp_word, i);
            }
            else {
                add_additional_list_element(temp_word, i);
            }
            
            // set temp_word back to null
            printf("clearing out temp_word\n");
            int j;
            for (j = 0; j < i; j++) {
                temp_word[j] = '\0';
            }
            i = 0;
        }
        else {} // it is a special character and should be ignored
        
    }
    
    // add last word
    add_additional_list_element(temp_word, i);
    
    printf("head word is %s\n", head->word);
    
    // print the results to the new files
    print_list(file2);
    
    // stop the clock and print result
    end = clock();
    execution_time = (long double)(end - begin) / CLOCKS_PER_SEC;
    fprintf(file3, "Execution Time: %f seconds\n", execution_time);
    
    // close file streams when finished
    fclose(file1);
    fclose(file2);
    fclose(file3);
    
    return 0;
}

struct list* add_first_list_element (char val[30], int j) {
    printf("Add first element function\n");
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr) {
        printf("Node creation failed.\n");
    }
    int i;
    for (i = 0; i < j; i++){
        ptr->word[i] = val[i];
    }
    ptr->next = 0;
    ptr->count = 1;
    current = head = ptr;
    return ptr;
}

struct list* add_additional_list_element (char val[30], int j) {
    printf("Add additional element function\n");
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    struct list *search_ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr | NULL == search_ptr) {
        printf("Node creation failed.\n");
    }
    bool search;
    search = search_list(val);
    printf("search is %d\n", search);
    
    if (search == 1) { // the word already exists-- find the node and increment the count value
        printf("increment node value\n");
        search_ptr = find_existing_node(val);
        search_ptr->count++;
    
    }
    else { // add a new node
        search_ptr = find_prior_node(val);
        if (search_ptr == NULL) { // add the node to the end of the list
            printf("add node to end of list\n");
            int i;
            for (i = 0; i < j; i++){
                ptr->word[i] = val[i];
            }
            ptr->next = 0;
            ptr->count = 1;
            current->next = ptr;
            current = ptr;
        }
        
        else if (search_ptr == head) // the new word becomes the new head
        {
            struct list *switch_ptr = (struct list*) malloc (sizeof (struct list));
            printf("word becomes new head\n");
            int i;
            for (i = 0; i < j; i++){
                ptr->word[i] = val[i];
            }
            
            ptr->count = 1;
            
            switch_ptr = head;
            printf("word in switch is now %s\n", switch_ptr->word); // hello
            ptr->next = switch_ptr;
            
            
            head = ptr;
            printf("word in head is now %s\n", head->word);
            
        }
        else {
            int i;
            for (i = 0; i < j; i++){
                ptr->word[i] = val[i];
            }
            //printf("new word in ptr is %s\n", ptr->word); // lit
            ptr->count = 1;
            
            struct list *moving_ptr = (struct list*) malloc (sizeof (struct list));
            if (NULL == moving_ptr) {
                printf("Node creation failed.\n");
            }
            
            moving_ptr = search_ptr; // word in moving ptr is hello
            moving_ptr = moving_ptr->next; // word in moving ptr is world
            ptr->next = moving_ptr;
            search_ptr->next = ptr;
            
        }
    }
    return ptr;
}

bool search_list (char val[30]) {
    //printf("Search function\n");
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr) {
        printf("Node creation failed.\n");
    }
    ptr = head;
    int i;
    do {
        i = strcmp(val, ptr->word);
        printf("i is %d\n", i); // if val ahead of ptr->word in alphabet then will return a positive number-- keep going until they are equal or negative
        ptr = ptr->next;
    } while (ptr != 0 && i > 0);
    printf("end do while\n");
    
    if (i == 0) {
        printf("word already exists\n");
        return true;
    }
    
    else {
        printf("word doesn't already exist\n");
        return false;
    }
    
}

struct list* find_existing_node (char val[30]) {
    printf("Existing node function\n");
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr) {
        printf("Node creation failed.\n");
    }
    
    ptr = head;
    int search;
    search = strcmp(val, ptr->word);
    printf("Search is %d\n", search);
    while (search != 0) {
        ptr = ptr->next;
        search = strcmp(val, ptr->word);
        printf("Search is %d\n", search);
    }
    
    return ptr;
}

struct list* find_prior_node (char val[30]) {
    printf("Prior node function\n");
    
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    struct list *curr = (struct list*) malloc (sizeof (struct list));
    
    if (NULL == ptr | NULL == curr) {
        printf("Node creation failed.\n");
    }
    
        // search the head node. if the head node is greater than val, then move on to the next node and search that node.
    
    ptr = curr = head;
    int search = 1;
    
    // search the head node
    search = strcmp(val, curr->word);
    printf("Search head node is %d\n", search);
    if (search < 0) {
        printf("head is being returned\n");
        return head;
    }
    if (ptr->next != NULL) { // move curr to the node ahead of ptr
        curr = curr->next;
        printf("word in curr is %s\n", curr->word);
        search = strcmp(val, curr->word);
    }
    else if (ptr->next == NULL) { // there is only the headnode and the word goes at the end
        printf("there is only the headnode\n");
        return NULL;
    }
    // will enter while loop if the head node is higher in the alphabet than val
    // val is lower in the alphabet than the word in the headnode
    while (search > 0 && curr != NULL) {
        // ptr becomes the same as curr
        printf("move ptr to next\n");
        ptr = ptr->next;
        // curr moves to the next node
        printf("move curr to next\n");
        curr = curr->next;
        // search searches curr node
        printf("search curr\n");
        if (curr != NULL) {
            printf("inside if statement\n");
            search = strcmp(val, curr->word);
            printf("Search is %d\n", search);
        }
        else {
            search = 0;
            printf("Search is %d\n", search);
        }

        //search = strcmp(val, curr->word);
        //printf("Search is %d\n", search);
    }
    //printf("end while with search = %d\n", search);

    if (search < 0) {
        //printf("returning node with %s\n", ptr->word);
        printf("returning ptr\n");
        return ptr;
    }
    else {
        printf("returning null\n");
        return NULL; // the word should go at the end or beginning
    }
}

void print_list (FILE *file2) {
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr) {
        printf("Node creation failed\n");
    }
    ptr = head;
    //printf("ptr word is %s\n", ptr->word);
    do {
        fprintf(file2, "%s, %d\n", ptr->word, ptr->count);
        ptr = ptr->next;
    }
    while (ptr->next != 0);
    fprintf(file2, "%s, %d\n", ptr->word, ptr->count);
    //printf("printed\n");
}
