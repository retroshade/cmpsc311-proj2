//
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
    
    char * arg1, * arg2, * arg3, * arg4;
    arg1 = argv[1];
    arg2 = argv[2];
    arg3 = argv[3];
    arg4 = argv[4]; //number of processes
    const int processNum = atoi(arg4);
    printf("arg4 is %s\n", arg4);
    printf("number of processes is %d\n", processNum);
    
    // open the files
    FILE *file1 = fopen(arg1, "r");
    FILE *file2 = fopen(arg2, "w");
    FILE *file3 = fopen(arg3, "w");
    
    // check to make sure that the files opened correctly. if not, exit the program with an error.
    if (file1 == NULL | file2 == NULL | file3 == NULL) {
        perror("Error opening file");
        return(-1);
    }
    
    // parse the input text file to count the number of words in the file
    int input_text;
    char temp_letter;
    char temp_word[30] = {(char)0};
    int count = 0;
    int j = 0;
    while ((input_text = fgetc(file1)) != EOF) {
        if (isalpha(input_text)) {
            input_text = tolower(input_text);
            temp_letter = (char)(input_text);
            temp_word[j] = temp_letter;
            j++;
        }
        else if ((input_text == ' ') | (input_text == '\n')) { // there is a space so the word should end
            if (temp_word[0] == '\0') {
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
    
    // add final word
    if (temp_word[0] != '\0') {
        count++;
        // clear final word
        int k;
        for (k = 0; k <= j; k++) {
            temp_word[k] = '\0';
        }
    }
    
    printf("count is %d\n", count);
    
    // calculate how many words each process will parse
    int childCount;
    int parentCount;
    int modOverflow;
    parentCount = childCount = (count / processNum);
    if ((modOverflow = count % processNum) != 0) {
        // unevenly divided words per number of processes
        parentCount += modOverflow;
    }
    
    
    // communicate which words each process will be reading
    int startWord[processNum];
    int endWord[processNum];
    int n;
    int m = 0;
    for (n = 0; n < (processNum); n++) {
        int counter = 1; // starting word
        if (n == 0) { // it is the parent
            startWord[n] = counter;
            printf("parent\n");
            printf("startword is %d\n", startWord[n]);
        }
        else if (n == 1) { // it is a child and right after the parent
            startWord[n] = parentCount + startWord[m];
            printf("startword is %d\n", startWord[n]);
            endWord[m] = startWord[n] - 1;
            printf("endword is %d\n", endWord[m]);
            m++;
        }
        else { // it is a subsequent child
            startWord[n] = childCount + startWord[m];
            printf("startword is %d\n", startWord[n]);
            endWord[m] = startWord[n] - 1;
            printf("endword is %d\n", endWord[m]);
            m++;
        }
    }
    
    // add ending word
    endWord[m] = count;
    printf("endword is %d\n", endWord[m]);
    
    pid_t childpid;
    int fd[(processNum - 1)][2];
    
    // forks the parent, creating the children and pipes
    int i;
    int specificStartingWord = 0;
    int specificEndingWord;
    
    for (i = 0; i < (processNum -1); i++) {
        if (pipe(fd[i])) {
            printf("pipe failed\n");
        }
        printf("created pipe in %d\n", i);
        printf("created new child\n");
        childpid = fork();
        if (childpid < 0) { // error with fork()
            printf("break\n");
            break;
        }
        if (childpid > 0 && specificStartingWord == 0) {
            specificStartingWord = startWord[i];
            specificEndingWord = endWord[i];
            //i++;
        }
        
        else if (childpid == 0) { // it is a child
            //if (i == 0)
                //i++;
            int childNumber = i - 1;
            printf("child number is %d\n", i);
            specificStartingWord = startWord[i + 1];
            specificEndingWord = endWord[i + 1];
            break;
        }
        else{}
     }


    head = (struct list*) malloc (sizeof (struct list));
    current = (struct list*) malloc (sizeof (struct list));
    head->count = 0;
    head->next = 0;
    current = head;
    input_text = '\0';
    
    int u = 0;
    int g;
    int counter = 1;
    
    rewind(file1);
    
    while ((input_text = fgetc(file1)) != EOF && counter <= specificEndingWord) { //&& counter <= specificEndingWord) {
        
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
                // don't store the word
                
            }
            
            else if (head->word[0] == '\0' && counter >= specificStartingWord) {
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
    
    // add last word
    
    if (temp_word[0] == '\0') { // counter shouldn't increment because there isn't a word
        counter--;
    }
    else if (counter < specificStartingWord) {
        // don't store the word
    }
    else if (head->word[0] == '\0' && counter >= specificStartingWord) {
        add_first_list_element(temp_word, u);
        
    }
    else {
        add_additional_list_element(temp_word, u);
    }
    
    // communicate results to the parent
    
    struct list *ptr1 = (struct list*) malloc (sizeof (struct list));
    struct list *ptr2 = (struct list*) malloc (sizeof (struct list));
    int bytesIN;
    // 0 is reading, 1 is writing
    if (childpid == 0) {
        printf("child\n");
        printf("head is %s\n", head->word);
        printf("child number is %d\n", i);
        ptr1 = head;
        printf("ptr1 is %s\n", ptr1->word);
        //dup2(fd[i][1], 1);
        close(fd[i][0]);
        
         while (ptr1->word[0] != '\0') {
            printf("loop val sent is %s\n", ptr1->word);
            printf("loop count sent is %d\n", ptr1->count);
             write(fd[i][1], ptr1, sizeof(ptr1));
             write(fd[i][1], &ptr1->count, sizeof(ptr1->count));
             //bytesIN = write(fd[i][1], ptr1, sizeof(ptr1));
             //printf("bytesIN = %d\n", bytesIN);
            //if (bytesIN <= 0)
              //  printf("failed\n");
            ptr1 = ptr1->next;
        }
        
        close(fd[i][1]);
        exit(0);
    }
    
    else {
        
        printf("parent\n");
        printf("head is %s\n", head->word);
        printf("parent waiting\n");
        wait(NULL);
        printf("parent resume\n");
        int y;
        int t;
        for (t = 0; t < parentCount; t++)
        {
            printf("parentCount is %d\n", parentCount);
            printf("first for loop\n");
            close(fd[0][1]);
            read(fd[0][0], ptr2, sizeof(ptr2));
            read(fd[0][0], &ptr2->count, sizeof(ptr2->count));
            printf("val received is %s\n", ptr2->word);
            printf("count received is %d\n", ptr2->count);
            sort(ptr2);
            
        }
       // sort(ptr2);
        close(fd[0][0]);
        
        
        if (processNum >= 2) {
        for (y = 1; y < (processNum - 1); y++) {
            printf("second for loop\n");
            //dup2(fd[y][0], 0);
            close(fd[y][1]);
            //int t;
            for (t = 0; t < childCount; t++)
            {
                read(fd[y][0], ptr2, sizeof(ptr2));
                printf("val received is %s\n", ptr2->word);
                sort(ptr2);
            }
            //sort(ptr2);
            close(fd[y][0]);
            }
        }
        
        
    }
    printf("sort final word\n");
    sort(ptr2);
    
    printf("val received after is %s\n", ptr2->word);
    
    
    
    // print the results to the new files
    //print_list(file2);
    printf("made it to the print_test function\n");
    print_test();
    
    
    // stop the clock and print result
    end = clock();
    execution_time = (long double)(end - begin) / CLOCKS_PER_SEC;
    //fprintf(file3, "Execution Time: %f seconds\n", execution_time);

    
    // close file streams when finished
    fclose(file1);
    fclose(file2);
    fclose(file3);
    
    printf("sos?\n");
    
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

struct list* add_additional_list_element_parent (char val[30], int j, int count) {
    printf("Add additional element PARENT function\n");
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    struct list *search_ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr | NULL == search_ptr) {
        printf("Node creation failed.\n");
    }
    bool search;
    search = search_list(val);
    
    if (search == 1) { // the word already exists-- find the node and increment the count value
        printf("increment node value\n");
        search_ptr = find_existing_node(val);
        printf("count is %d\n", count);
        printf("searchptr count is %d\n", search_ptr->count);
        search_ptr->count += count;
        printf("new searchptr count is %d\n", search_ptr->count);
        
    }
    else { // add a new node
        search_ptr = find_prior_node(val);
        if (search_ptr == NULL) { // add the node to the end of the list
            printf("add word to end\n");
            int i;
            for (i = 0; i < j; i++){
                ptr->word[i] = val[i];
            }
            ptr->next = 0;
            ptr->count = count;
            current->next = ptr;
            printf("word in current is %s\n", current->word);
            current = ptr;
            printf("new word in current is %s\n", current->word);
            
            
        }
        
        else if (search_ptr == head) // the new word becomes the new head
        {
            struct list *switch_ptr = (struct list*) malloc (sizeof (struct list));
            int i;
            for (i = 0; i < j; i++){
                ptr->word[i] = val[i];
            }
            
            ptr->count = count;
            
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
            ptr->count = count;
            
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
        //printf("i is %d\n", i); // if val ahead of ptr->word in alphabet then will return a positive number-- keep going until they are equal or negative
        ptr = ptr->next;
    } while (ptr != 0 && i > 0);
    //printf("end do while\n");
    
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
    //printf("Search is %d\n", search);
    while (search != 0) {
        ptr = ptr->next;
        search = strcmp(val, ptr->word);
        //printf("Search is %d\n", search);
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
    //printf("Search head node is %d\n", search);
    if (search < 0) {
        printf("head is being returned\n");
        return head;
    }
    if (ptr->next != NULL) { // move curr to the node ahead of ptr
        curr = curr->next;
        //printf("word in curr is %s\n", curr->word);
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
        //printf("move ptr to next\n");
        ptr = ptr->next;
        // curr moves to the next node
        //printf("move curr to next\n");
        curr = curr->next;
        // search searches curr node
        //printf("search curr\n");
        if (curr != NULL) {
            //printf("inside if statement\n");
            search = strcmp(val, curr->word);
            //printf("Search is %d\n", search);
        }
        else {
            search = 0;
            //printf("Search is %d\n", search);
        }
    }
    
    if (search < 0) {
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
    do {
        fprintf(file2, "%s, %d\n", ptr->word, ptr->count);
        ptr = ptr->next;
    }
    while (ptr->next != 0);
    fprintf(file2, "%s, %d\n", ptr->word, ptr->count);
}

void print_test() {
    struct list *ptr = (struct list*) malloc (sizeof (struct list));
    if (NULL == ptr) {
        printf("Node creation failed\n");
    }
    ptr = head;
    
    do {
        printf("%s, %d\n", ptr->word, ptr->count);
        if (ptr->next == NULL)
            return;
        ptr = ptr->next;
    } while (ptr->next != 0);
    if (ptr->word[0] != '\0') {
        printf("%s, %d\n", ptr->word, ptr->count);
    }
    printf("finished print function\n");
}

void sort(struct list *new) {
    printf("inside sort function\n");

    printf("word in the sort function currently is %s\n", new->word);
    printf("count in the sort function currently is %d\n", new->count);
    
    add_additional_list_element_parent(new->word, strlen(new->word) + 1, new->count);
    
}
