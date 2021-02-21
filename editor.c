#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 1024

int undo = 0;

typedef struct editor Editor;
typedef struct file_state State;
typedef struct dictionary Dictionary ;

/**char* is the struct that represents a string that will be stored into the dictionary
 * @field string is the string stored
 * @field time is the time when it has been stored
 */

/**Dictionary is the struct that contains all the char* stored
 * @field max_length is a long that represents the maximum length of the dictionary
 * @field current_index is a long that represents the index to where the next char* will be stored
 */
struct dictionary{
    long max_length;
    long current_index;
    char** list_of_strings;
};


struct file_state{
    long time;
    long size;
    long* file;
};


struct editor{
    long max_time;
    long time_size;
    long current_time;
    long current_index;
    State** file_states;

};

typedef struct command{
    char comm;
    long first;
    long second;
} Command;


// -------------- UTILITY ----------------------

/**Get the char* of the command on input
 * @param string is the input string
 * @return the command object that contains all the char*
 */
Command get_command(char* string){
    char comm = string[strlen(string) - 2];
    char* ptr;
    long first = 0, second = 0;
    if (strlen(string) > 1){
        first = strtol(string, &ptr, 10);
        while (string[0] != ',' && strlen(string) > 0){
            string = string + 1;
        }
        string = string + 1;
        second = strtol(string, &ptr, 10);
    }
    Command res;
    res.comm = comm;
    res.first = first;
    res.second = second;

    return res;
}

/**Get the maximum value between first and second
 * @param first is a long value
 * @param second is a long value
 * @return the maximum value
 */
long max_long(long first, long second){
    if (second < first) {
        return first;
    } else {
        return second;
    }
}




// ------------- DICTIONARY ----------------

void add_string(char* string, Dictionary* dict, long time){
    if (dict->current_index >= dict->max_length){
        dict->max_length = dict->max_length + 50;
        dict->list_of_strings = (char**) realloc(dict->list_of_strings, dict->max_length * sizeof(char*));
    }

    dict->list_of_strings[dict->current_index] = string;
    dict->current_index++;

}






// ------------ UPDATE -------------------------


void update_change_state(State* starting_state, State* ending_state, long first_address, long second_address,const long* array, long time){
    if (starting_state->size == 0 || starting_state == NULL){
        ending_state->size = second_address;
        ending_state->file = (long*) malloc(ending_state->size* sizeof(long));

        for (long i = 0; i < second_address; i++) {
            ending_state->file[i] = array[i];
        }
    }

    else if(first_address >= starting_state->size) {
        ending_state->size = second_address;
        ending_state->time = time;
        if (starting_state->file!= NULL){
            ending_state->file = (long*) realloc(starting_state->file, second_address * sizeof(long));
        }
        else{
            ending_state->file = (long*) calloc(second_address ,sizeof(long));
        }
        for (long i = first_address - 1, k = 0; i < second_address; ++i) {
            ending_state->file[i] = array[k];
            k++;
        }
    }
    else {
        ending_state->size = max_long(starting_state->size, second_address);
        ending_state->file = (long*) malloc(ending_state->size * sizeof(long));
        for (long i = 0, k = 0; i < ending_state->size; i++) {
            if (i >= first_address - 1 && i <= second_address -1){
                ending_state->file[i] = array[k];
                k++;
            }
            else ending_state->file[i] = starting_state->file[i];
        }
    }

    ending_state->time = time;
}



void update_delete_state(State* state, State* end_state,long first_address, long second_address, long time){
    int diff = second_address - first_address;

    if(state->size == 0 ){
        end_state->size = 0;
        end_state->file = (long*) calloc(1, sizeof(long));
        end_state->file[0] = 18;
    }


    else if (first_address > state->size){
        end_state->size = state->size;
        end_state->file = (long*) calloc(state->size , sizeof(long));

        for (long i = 0; i < state->size; i++){
            end_state->file[i] = state->file[i];
        }
    }
    else if (first_address <= 1 && second_address >= state->size){
        end_state->size = 0;
        end_state->file = (long*) calloc(1, sizeof(long));
        end_state->file[0] = 18;
    }

    else if (first_address <= state->size && second_address >= state->size){
        end_state->size = first_address ;
        end_state->file = (long*) calloc(first_address , sizeof(long));

        for (long i = 0; i < first_address - 1 ; i++) {
            end_state->file[i] = state->file[i];
        }
    }
    else{
        end_state->size = state->size - diff - 1;
        end_state->file = (long*) calloc(end_state->size , sizeof(long));

        for (int i = 0; i < end_state->size; i++) {
            if (i >= first_address - 1) {
                end_state->file[i] = state->file[i + diff + 1];
            }

            else{
                end_state->file[i] = state->file[i];
            }
        }
    }


    end_state->time = time;
}


// -------------------- EDITOR -------------------

void initialize_editor(Editor *edu){
    edu->current_index = 0;
    edu->current_time = 0;
    edu->max_time = 0;
    edu->time_size = 40;
    edu->file_states = (State**) calloc(40, sizeof(State*));

}


// ------------------ COMMAND --------------

void command_print(Editor* edu, Dictionary *dictionary, long first_address, long second_address){



    if (dictionary->current_index == 0  || edu->current_index <= 0 || edu->file_states[edu->current_index-1]->size == 0){
        for (long i = first_address - 1; i < second_address; i++) {
            fputs(".\n", stdout);
        }
    } else{
        State * state = edu->file_states[edu->current_index - 1];
        for (long i = first_address - 1; i < second_address; i++) {
            if (i < state->size && i >= 0){
                fputs(dictionary->list_of_strings[state->file[i]], stdout);
            } else{
                fputs(".\n", stdout);
            }
        }
    }
}



void command_change(Editor * edu, Dictionary *dictionary, long first_address, long second_address, char** pString){
    long time = edu->max_time;
    long current_index = edu->current_index;


    edu->file_states[current_index] = (State*) calloc(1, sizeof(State));
    long array[second_address];


    if (dictionary->current_index + second_address - first_address + 1 >= dictionary->max_length){
        dictionary->max_length = dictionary->max_length + second_address - first_address + 50;
        dictionary->list_of_strings = realloc(dictionary->list_of_strings, dictionary->max_length * sizeof(char*));
    }


    if (edu->current_index == 0){
        edu->file_states[0]->size = second_address;
        edu->file_states[0]->time = edu->max_time;

        if (time != 0){
            dictionary->current_index = 0;
        }


    } else if (edu->time_size < edu->current_index + 1){
        edu->time_size += 50 ;
        edu->file_states = (State**) realloc(edu->file_states,edu->time_size * sizeof(State*));
        edu->file_states[edu->current_index]->size = second_address;
        edu->file_states[edu->current_index]->time = edu->max_time;


    }

    long i = 0;
    for (; i < second_address - first_address +1; i++) {

        array[i] = dictionary->current_index;
        add_string(pString[i], dictionary, edu->current_time);
    }


    if (edu->current_index == 0){

        update_change_state(edu->file_states[0], edu->file_states[0], first_address, second_address, array, edu->current_time);
    }
    else{
        update_change_state(edu->file_states[edu->current_index-1], edu->file_states[edu->current_index], first_address, second_address, array, edu->current_time);

    }

    edu->max_time++;
    edu->current_index++;
}




void command_delete(Editor * edu, long first_address, long second_address){
    long time = edu->max_time;
    long current_index = edu->current_index;

    edu->file_states[current_index] = (State*) calloc(1 , sizeof(State));
    edu->file_states[current_index]->size = 0;

    if (edu->time_size < edu->current_index + 1){
        edu->time_size += 50 ;
        edu->file_states = (State**) realloc(edu->file_states,edu->time_size * sizeof(State*));
        edu->file_states[edu->current_index]->time = edu->max_time;

    }


    if (edu->current_index == 0){
        update_delete_state(edu->file_states[0], edu->file_states[0], first_address, second_address, time);

    } else{

        update_delete_state(edu->file_states[edu->current_index - 1], edu->file_states[edu->current_index], first_address, second_address, time);
    }
    edu->max_time++;
    edu->current_index++;
}

void print_all_states(Editor* edu, long maxed){
    for (int i = 0; i < maxed; ++i) {
        printf("\n\nFile %d:\n", i);
        for (int j = 0; j < edu->file_states[i]->size; ++j) {
            printf("%ld\n", edu->file_states[i]->file[j]);
        }
    }
}

// -------------- MAIN --------------------

int main(){

    int maxed = 0;

    int temp = 0;

    Dictionary *dictionary = (Dictionary*) calloc(1, sizeof(Dictionary));
    dictionary->max_length = 400;
    dictionary->current_index = 0;
    dictionary->list_of_strings = (char**) calloc(400,  sizeof(char*));

    Editor * edu = (Editor*) calloc(1, sizeof(Editor));

    initialize_editor(edu);
    Command active;



    char* string = (char*) calloc(N, sizeof(char));
    while (1){

        string = fgets(string, N, stdin);
        active = get_command(string);

        if (active.comm == 'q'){
            free(edu);
            free(dictionary);

            return 0;
        }

        else if(active.comm == 'p') {

            command_print(edu, dictionary, active.first, active.second);

        }

        else if (active.comm == 'c'){
            char* array_string[active.second - active.first + 2];
            for (long j = 0; j < active.second - active.first + 2; j++) {
                array_string[j] = strdup( fgets(string, N, stdin));
            }

            command_change(edu, dictionary, active.first, active.second, array_string);

            maxed = edu->current_index;
            undo = 0;
        }

        else if (active.comm == 'd'){
            command_delete(edu, active.first, active.second);
            maxed = edu->current_index;
            undo = 0;

        }

        else if (active.comm == 'u') {
            undo += active.first;
            if (undo >= maxed){
                undo = maxed;
            }
            edu->current_index = maxed - undo;
            if (edu->current_index < 0) edu->current_index = 0;
        }

        else if (active.comm == 'r'){
            temp = undo - active.first;

            if (temp < 0){
                undo = 0;
            }
            else{
                undo = temp;
            }


            edu->current_index = maxed - undo;
            if (edu->current_index < 0) edu->current_index = 0;

        }
    }



}
