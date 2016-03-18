// Project 2-- Hayley Sundra and Ryan Mysliwiec
//  wordc-mp.c
//

#include "wordc-mp.h"

struct list *head;
struct list *current;

int main(int argc, char **argv) {
    
    // declare clock variables and start the clock
    clock_t begin, end;
    double execution_time;
    begin = clock();
    
    // collect the arguments
    char * arg1, * arg2, * arg3, * arg4;
    arg1 = argv[1];
    arg2 = argv[2];
    arg3 = argv[3];
    arg4 = argv[4]; //number of processes
    const int processNum = atoi(arg4);
    
    // open the files
    FILE *file1 = fopen(arg1, "r");
    FILE *file2 = fopen(arg2, "w");
    FILE *file3 = fopen(arg3, "w");
    
    // check to make sure that the files opened correctly
    if (file1 == NULL | file2 == NULL | file3 == NULL) {
        perror("Error opening file");
        return(-1);
    }
    
    // parse the input text file to count the total number of words in the file
    int input_text;
    char temp_letter;
    char temp_word[30] = {(char)0};
    int count = 0; // variable for total number of words in file
    int j = 0; // iterates over each letter in every word
    
    while ((input_text = fgetc(file1)) != EOF) {
        if (isalpha(input_text)) { // it is a letter so store the letter
            input_text = tolower(input_text);
            temp_letter = (char)(input_text);
            temp_word[j] = temp_letter;
            j++;
        }
        else if ((input_text == ' ') | (input_text == '\n')) { // there is a space so the word should end and count should increment
            if (temp_word[0] == '\0') { // the word is null so don't store anything
                j = 0;
            }
            else {
                count++;
                j = 0;
            }
            
            // set temp_word back to null
            int k;
            for (k = 0; k <= j; k++) {
                temp_word[k] = '\0';
            }
        }
        else {} // it is a special character and should be ignored
    }
    
    // add final word after the file ends if temp_word is not empty
    if (temp_word[0] != '\0') {
        count++;
        // clear final temp_word
        int k;
        for (k = 0; k <= j; k++) {
            temp_word[k] = '\0';
        }
    }
    
    // calculate how many words each process will parse
    int childCount; // the number of words each child should count
    int parentCount; // the number of words each parent should count
    int modOverflow; // calculate overflow
    parentCount = childCount = (count / processNum); // calculate child count and parentCount
    if ((modOverflow = count % processNum) != 0) {
        // unevenly divided words per number of processes, so add excess to parentCount
        parentCount += modOverflow;
    }
    
    // communicate which words each process will be reading to the arrays startWord and endWord
    int startWord[processNum];
    int endWord[processNum];
    int n; // iterator n
    int m = 0; // iterator m to keep track of the index right before n
    for (n = 0; n < (processNum); n++) { // iterate through each process
        int counter = 1; // starting word
        if (n == 0) {
            // it is the parent
            startWord[n] = counter;
        }
        else if (n == 1) {
            // it is the first child right after the parent
            startWord[n] = parentCount + startWord[m]; // must start counting after (parentCount) number of words
            endWord[m] = startWord[n] - 1; // store endWord for the previous index
            m++;
        }
        else {
            // it is a subsequent child
            startWord[n] = childCount + startWord[m]; // must start counting after (childCount) number of words
            endWord[m] = startWord[n] - 1; // store endWord for the previous index
            m++;
        }
    }
    
    // add the last endWord for the last process
    endWord[m] = count;
    
    // restart the input file from the beginning
    rewind(file1);
    
    pid_t childpid;
    
    // multi-dimensional array to to store the pipes based on processNum so that it can change for the processNum input
    int fd[(processNum - 1)][2];
    
    // forks the parent, creating the children and pipes
    int i;
    // specific starting word and ending word variable for each process
    int specificStartingWord = 0;
    int specificEndingWord;
    
    for (i = 0; i < (processNum - 1); i++) {
        if (pipe(fd[i])) {
            printf("pipe failed\n");
        }
        childpid = fork();
        if (childpid < 0) {
            // error with fork()
            break;
        }
        if (childpid > 0 && specificStartingWord == 0) {
            // it is the parent
            specificStartingWord = startWord[i];
            specificEndingWord = endWord[i];
        }
        
        else if (childpid == 0) { // it is a child
            int childNumber = i - 1; // assign each child an index, starting with 0
            specificStartingWord = startWord[i + 1];
            specificEndingWord = endWord[i + 1];
            //close and reopen the file to prevent race condition
            fclose(file1);
            file1 = fopen(arg1, "r");
            break;
        }
        else {}
     }
    
    if (processNum == 1) { // there is only the parent, assign specific starting and ending words
        specificStartingWord = 1;
        specificEndingWord = count;
    }


    head = (struct list*) malloc (sizeof (struct list));
    current = (struct list*) malloc (sizeof (struct list));
    if (NULL == current || NULL == head) {
        printf("Node creation failed.\n");
    }
    head->count = 0;
    head->next = 0;
    current = head;
    input_text = '\0';
    
    
    
    int u = 0;
    int g;
    int counter = 1;
    
    // read through the file again and actually store words
    while ((input_text = fgetc(file1)) != EOF && counter <= specificEndingWord) { // read until the ending word is encountered for each process
        if (isalpha(input_text)) {
            // convert it to an alpha and store it in array
            input_text = tolower(input_text);
            temp_letter = (char)(input_text);
            temp_word[u] = temp_letter;
            u++;
        }
        else if ((input_text == ' ') | (input_text == '\n')) { // there is a space so the word should end
            // the word should be added to the linked list.
            if (temp_word[0] == '\0') { // counter shouldn't increment because there isn't a word
                counter--;
            }
            else if (counter < specificStartingWord) {
                // don't store the word because it is not in the process's range
            }
            else if (head->word[0] == '\0' && counter >= specificStartingWord) {
                // head is empty so store in head
                add_first_list_element(temp_word, u);
            }
            else {
                add_additional_list_element(temp_word, u);
            }
            // set temp_word back to null
            int j;
            for (j = 0; j < u; j++) {
                temp_word[j] = '\0';
            }
            u = 0;
            counter++;
        }
        else {} // it is a special character and should be ignored
    }
    // add the final word to last child
    i++;
    if (i == (processNum - 1) && childpid == 0) { // it is the final child
        if (temp_word[0] == '\0') {
            // counter shouldn't increment because there isn't a word
        }
        else if (counter < specificStartingWord) {
            // don't store the word because it is not in the process's range
        }
        else if (head->word[0] == '\0' && counter >= specificStartingWord) {
            // head is empty so store in head
            add_first_list_element(temp_word, u);
        }
        else {
            add_additional_list_element(temp_word, u);
        }
    }
    
    if (processNum == 1) { // if there is only the parent process- only one running process
        // add the final word to the parent
        if (temp_word[0] == '\0') {}
        else if (head->word[0] == '\0' && counter >= specificStartingWord) {
            // head is empty so store in head
            add_first_list_element(temp_word, u);
        }
        else {
            add_additional_list_element(temp_word, u);
        }
        
        // print linked list to file
        print_list(file2);
        
        // stop the clock and print result
        end = clock();
        execution_time = (long double)(end - begin) / CLOCKS_PER_SEC;
        fprintf(file3, "Execution Time: %f seconds\n", execution_time);
        
        
        // close file streams when finished
        fclose(file1);
        fclose(file2);
        fclose(file3);
        
        // exit the program
        exit(0);
    }
    
    // communicate results of the children processes to the parent
    
    struct list *ptr1 = (struct list*) malloc (sizeof (struct list));
    struct list *ptr2 = (struct list*) malloc (sizeof (struct list));
    struct list *ptr3 = (struct list*) malloc (sizeof (struct list));
    
    if (NULL == ptr1 || NULL == ptr2 || NULL == ptr3) {
        printf("Node creation failed.\n");
    }

    // 0 is reading, 1 is writing for fd read and write
    int childCounter = 1; // counts number of words in each child's linked list
    
    if (childpid == 0) {
        // it is a child
        i--;
        ptr1 = ptr3 = head;
        // iterate through to count number of words in child's linked list
        do {
            ptr3 = ptr3->next;
            childCounter++;
        } while(ptr3->next != '\0');
        // pass number of values in the child list to the parent so that the parent knows how many times to read in the for-loop
        close(fd[i][0]);
        write(fd[i][1], &childCounter, sizeof(childCounter));
        while (ptr1->word[0] != '\0') { // send the words to the parent through pipes
             write(fd[i][1], ptr1, sizeof(ptr1));
             write(fd[i][1], &ptr1->count, sizeof(ptr1->count));
             ptr1 = ptr1->next;
        }
        close(fd[i][1]);
        exit(0); // exit each child process after it sends its information
    }
    else {
        // parent waits for all children to finish before reading results
        wait(NULL);
        int y, t;
        close(fd[0][1]);
        read(fd[0][0], &childCounter, sizeof(childCounter));
        // first for loop reads from the first child
        for (t = 0; t < childCounter; t++)
        {
            read(fd[0][0], ptr2, sizeof(ptr2));
            read(fd[0][0], &ptr2->count, sizeof(ptr2->count));
            sort(ptr2);
        }
        close(fd[0][0]);
        
        // second for loop reads from subsequent children (after first child)
        if (processNum >= 2) {
            for (y = 1; y < (processNum - 1); y++) {
                close(fd[y][1]);
                read(fd[y][0], &childCounter, sizeof(childCounter));
                for (t = 0; t < childCounter; t++)
                {
                    read(fd[y][0], ptr2, sizeof(ptr2));
                    read(fd[y][0], &ptr2->count, sizeof(ptr2->count));
                    sort(ptr2);
                }
                close(fd[y][0]);
            }
        }
    }

    // print the final results to the new files
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

// function to add the initial element in a linked list (head node)
struct list* add_first_list_element (char val[30], int j) {
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

// function to add to a linked list after there is already a head node
struct list* add_additional_list_element (char val[30], int j) {
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    struct list *search_ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr | NULL == search_ptr) {
        printf("Node creation failed.\n");
    }
    // call search_list function to see if the value already exists
    bool search;
    search = search_list(val);
    
    if (search == 1) { // the word already exists-- find the node and increment the count value
        // call the find_existing_node function
        search_ptr = find_existing_node(val);
        search_ptr->count++;
    }
    else {
        // the word doesn't exist-- add a new node
        // call find_prior_node function to find the value right before where the node should be inserted, if there is one
        search_ptr = find_prior_node(val);
        if (search_ptr == NULL) { // add the node to the end of the list
            int i;
            for (i = 0; i < j; i++){
                ptr->word[i] = val[i];
            }
            ptr->next = 0;
            ptr->count = 1;
            current->next = ptr;
            current = ptr;
        }
        
        else if (search_ptr == head) // the new word becomes the new head or one right after head
        {
            if (strcmp(search_ptr->word, val) > 0) {
                // the word becomes the new head node
                struct list *switch_ptr = (struct list*) malloc (sizeof (struct list));
                if (switch_ptr == NULL) {
                    printf("Node creation failed.\n");
                }
                int i;
                for (i = 0; i < j; i++){
                    ptr->word[i] = val[i];
                }
                ptr->count = 1;
                switch_ptr = head;
                ptr->next = switch_ptr;
                head = ptr;
            }
            else {
                // word to be inserted immediately after the head node
                struct list *switch_ptr = (struct list*) malloc (sizeof (struct list));
                if (switch_ptr == NULL) {
                    printf("Node creation failed.\n");
                }
                int i;
                for (i = 0; i < j; i++){
                    ptr->word[i] = val[i];
                }
                ptr->count = 1;
                switch_ptr = head;
                switch_ptr = switch_ptr->next;
                head->next = ptr;
                ptr->next = switch_ptr;
            }
        }
        else { // word goes somewhere in the middle of list
            int i;
            for (i = 0; i < j; i++){
                ptr->word[i] = val[i];
            }
            ptr->count = 1;
            struct list *moving_ptr = (struct list*) malloc (sizeof (struct list));
            if (NULL == moving_ptr) {
                printf("Node creation failed.\n");
            }
            moving_ptr = search_ptr;
            moving_ptr = moving_ptr->next;
            ptr->next = moving_ptr;
            search_ptr->next = ptr;
        }
    }
    return ptr;
}

// adds the words from the child linked lists to the parent list
struct list* add_additional_list_element_parent (char val[30], int j, int count) {
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    struct list *search_ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr | NULL == search_ptr) {
        printf("Node creation failed.\n");
    }
    // call search_list function to see if the value already exists
    bool search;
    search = search_list(val);
    if (search == 1) { // the word already exists-- find the node and increment the count value
        search_ptr = find_existing_node(val);
        search_ptr->count += count;
    }
    else { // word doesn't already exist-- add a new node
        search_ptr = find_prior_node(val);
        if (search_ptr == NULL) { // add the node to the end of the list
            int i;
            for (i = 0; i < j; i++){
                ptr->word[i] = val[i];
            }
            ptr->next = 0;
            ptr->count = count;
            current->next = ptr;
            current = ptr;
        }
        
        else if (search_ptr == head) // the new word becomes the new head or one right after the head
        {
            if (strcmp(search_ptr->word, val) > 0) {
                // word becomes the new head
                struct list *switch_ptr = (struct list*) malloc (sizeof (struct list));
                if (NULL == switch_ptr) {
                    printf("Node creation failed.\n");
                }
                int i;
                for (i = 0; i < j; i++){
                    ptr->word[i] = val[i];
                }
                ptr->count = count;
                switch_ptr = head;
                ptr->next = switch_ptr;
                head = ptr;
            }
            else {
                // word should be inserted right after the headnode
                struct list *switch_ptr = (struct list*) malloc (sizeof (struct list));
                if (NULL == switch_ptr) {
                    printf("Node creation failed.\n");
                }
                int i;
                for (i = 0; i < j; i++){
                    ptr->word[i] = val[i];
                }
                ptr->count = count;
                switch_ptr = head;
                switch_ptr = switch_ptr->next;
                head->next = ptr;
                ptr->next = switch_ptr;
            }
        }
        else { // word should be inserted somewhere in the middle of the list
            int i;
            for (i = 0; i < j; i++){
                ptr->word[i] = val[i];
            }
            ptr->count = count;
            
            struct list *moving_ptr = (struct list*) malloc (sizeof (struct list));
            if (NULL == moving_ptr) {
                printf("Node creation failed.\n");
            }
            moving_ptr = search_ptr;
            moving_ptr = moving_ptr->next;
            ptr->next = moving_ptr;
            search_ptr->next = ptr;
        }
    }
    return ptr;
}

// search the linked list to see if the value already exists or not-- return true if it does or false if it doesn't
bool search_list (char val[30]) {
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr) {
        printf("Node creation failed.\n");
    }
    ptr = head;
    int i;
    do {
        i = strcmp(val, ptr->word);
        ptr = ptr->next;
    } while (ptr != 0 && i > 0);
    if (i == 0) {
        return true;
    }
    
    else {
        return false;
    }
}

// if the word already exists, locate that specific node and return it
struct list* find_existing_node (char val[30]) {
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr) {
        printf("Node creation failed.\n");
    }
    ptr = head;
    int search;
    search = strcmp(val, ptr->word);
    // while search doesn't equal zero, it hasn't found the word yet
    while (search != 0) {
        ptr = ptr->next;
        search = strcmp(val, ptr->word);
    }
    return ptr;
}

// find the node right before where the new word should be inserted
struct list* find_prior_node (char val[30]) {
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    struct list *curr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr | NULL == curr) {
        printf("Node creation failed.\n");
    }
    ptr = curr = head;
    int search = 1;
    // search the head node
    search = strcmp(val, curr->word);
    if (search < 0) {
        // return head because the value is higher in the alphabet than the head
        return head;
    }
    if (ptr->next != NULL) { // move curr to the node ahead of ptr
        curr = curr->next;
        search = strcmp(val, curr->word);
    }
    else if (ptr->next == NULL) { // there is only the headnode and the word goes at the end of the list
        return NULL;
    }
    // will enter while loop if the head node is higher in the alphabet than val
    while (search > 0 && curr != NULL) {
        // ptr becomes the same as curr
        ptr = ptr->next;
        // curr moves head to the next node to search it
        curr = curr->next;
        // search curr node
        if (curr != NULL) {
            search = strcmp(val, curr->word);
        }
        else {
            search = 0;
        }
    }
    
    if (search < 0) {
        return ptr;
    }
    else {
        return NULL; // the word should go at the end or beginning
    }
}

// function to print the list to the file
void print_list (FILE *file2) {
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr) {
        printf("Node creation failed\n");
    }
    ptr = head;
    do {
        fprintf(file2, "%s, %d\n", ptr->word, ptr->count);
        ptr = ptr->next;
    } while (ptr->next != 0);
    // print the final word
    fprintf(file2, "%s, %d\n", ptr->word, ptr->count);
}

// function to sort child lists into parent lists-- calls the add_additional_list_element_parent function
void sort(struct list *new) {
    add_additional_list_element_parent(new->word, strlen(new->word) + 1, new->count);
}
