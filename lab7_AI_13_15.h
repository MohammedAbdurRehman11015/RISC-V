#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define SIZE 30
#define HEX_SIZE 9
#define MAX_INSTRUCTIONS 100
#define MAX_LINES 200
#define MAX_LINE_LEN 60
#define MAX_DWORDS 1024
#define MAX_WORDS (MAX_DWORDS * 2) 
#define MAX_HALFS (MAX_DWORDS * 4) 
#define MAX_BYTES (MAX_DWORDS * 8) 

#define MEMORY_SIZE 0x200000 
#define TEXT_SIZE 0x10000     
#define DATA_START 0x10000    
#define MAX_STACK_FRAMES 64
#define STACK_START 0x50000
#define STACK_SIZE 0x50000
#define maxstacksize 0x50000



extern char output_filename[100];
extern char output_dump_file[100];

typedef struct stack{
    int top_index;
    char* value[maxstacksize];     
    int line_num[maxstacksize];    
} stack;

typedef struct {
    bool valid;
    bool dirty;
    uint32_t tag;
    uint32_t last_access;
    uint8_t *data;
} Block;

typedef struct {
    bool enabled;
    int cache_size;
    int block_size;
    int associativity;
    int number_of_sets;
    char replacement_policy[8];
    char write_policy[3];
    Block **sets; 
    int accesses;
    int hits;
    int *fifo_indices;
} Cache;


stack* createemptystack(); 

bool push_to_stack(stack *s, char* value);

char* pop_from_stack(stack* s);

char* top(stack* s);





    

uint64_t cache_access(uint64_t address, bool write_cond,uint64_t write_data);
void random_number_generation();
int create_label_map(char lines[MAX_LINES][MAX_LINE_LEN], int total_lines, char label_map[MAX_LINES][MAX_LINE_LEN], int line_map[MAX_LINES]);
long get_line_offset(int line_number, const char lines[MAX_LINES][MAX_LINE_LEN]);
int find_label_position(const char *label, const char lines[MAX_LINES][MAX_LINE_LEN], int total_lines);

void parse_instruction_input(char *input, char *b, char *c, char *d, char *e);
void print_memory(int64_t start_address,int num_bytes);
int load_file_into_arrays(const char *filename, char lines[MAX_LINES][MAX_LINE_LEN],
                          int64_t mem_values_dword[MAX_DWORDS], 
                          int64_t mem_values_word[MAX_WORDS], 
                          int64_t mem_values_half[MAX_HALFS], 
                          uint64_t mem_values_byte[MAX_BYTES]);

void process_file_lines(char lines[MAX_LINES][MAX_LINE_LEN], int line_count);
int validate_R_instruction(const char array[]);
void execute_R_instruction(char a[], char b[], char c[], char d[], char e[]);
void store_double_word(int64_t memory_address, int64_t value_to_store);
int check_register_format(char *reg);


int validate_I_instruction(const char array[]);
void execute_I_instruction(char a[], char b[], char c[], char d[], char e[]);
int validate_I_S_instruction(const char array[]);
void execute_I_S_instruction(char a[], char b[], char c[], char d[], char e[],int64_t mem_values[]);
int validate_U_instruction(const char array[]);
void execute_U_instruction(char a[], char b[], char c[], char d[]);
int validate_B_instruction(const char array[]);
void execute_B_instruction(char a[], char b[], char c[], char d[], char e[], const char lines[MAX_LINES][MAX_LINE_LEN], int total_lines);
int validate_J_instruction(const char array[]);
void execute_J_instruction(char a[], char b[], char c[], char d[], const char lines[MAX_LINES][MAX_LINE_LEN], int total_lines);
int check_immediate_format(char *num);
int count_lines(const char *filename);
int count_args_from_input(char input[]);

int calculate_total_lines (const char *filename);
void datasection_lines(const char *filename);

void step(const char lines[][MAX_LINE_LEN], int total_lines, int64_t mem_values[], stack* stack_1);
void handle_instructions_from_lines(char lines[MAX_LINES][MAX_LINE_LEN], int total_lines, int64_t mem_values[], int line_no, int current_line_no, stack* stack_1);
void  reset_values_for_mutiple_loads();
void set_breakpoints(int line_no);
int breakpoint_found(int line_no);
void delete_breakpoint(int line);
void print_memory(int64_t addr, int count);





int find_label_position(const char *label, const char lines[MAX_LINES][MAX_LINE_LEN], int total_lines);

long get_line_offset(int line_number, const char lines[MAX_LINES][MAX_LINE_LEN]);



int count_lines(const char *filename);



void parse_instruction_input(char input[], char b[], char c[], char d[], char e[]);

int validate_R_instruction(const char array[]);

int validate_I_instruction(const char array[]);

int validate_I_S_instruction(const char array[]);

int validate_U_instruction(const char array[]);

int validate_B_instruction(const char array[]);

int validate_J_instruction(const char array[]);

void execute_R_instruction(char a[], char b[], char c[], char d[], char e[]);

void execute_I_instruction(char a[], char b[], char c[], char d[], char e[]);


void execute_I_S_instruction(char a[], char b[], char c[], char d[], char e[], int64_t mem_values[]);



int load_file_into_arrays(const char *filename, char lines[MAX_LINES][MAX_LINE_LEN],
                          int64_t mem_values_dword[MAX_DWORDS], 
                          int64_t mem_values_word[MAX_WORDS], 
                          int64_t mem_values_half[MAX_HALFS], 
                          uint64_t mem_values_byte[MAX_BYTES]);



void execute_U_instruction(char a[], char b[], char c[], char d[]);

void process_file_lines(char lines[MAX_LINES][MAX_LINE_LEN], int line_count);


void execute_B_instruction(char a[], char b[], char c[], char d[], char e[], const char lines[MAX_LINES][MAX_LINE_LEN], int total_lines);


void execute_J_instruction(char a[], char b[], char c[], char d[], const char lines[MAX_LINES][MAX_LINE_LEN], int total_lines); 





int check_immediate_format(char *num);


int create_label_map(char lines[MAX_LINES][MAX_LINE_LEN], int total_lines, char label_map[MAX_LINES][MAX_LINE_LEN], int line_map[MAX_LINES]);

int count_args_from_input(char input[]);

int check_register_format(char *reg);


void initialize_cache(const char *config_file);


int find_block(Block *set, uint32_t tag);

void free_cache(); 

void init_cache_log(const char *filename);

void close_cache_log();

void log_access(char op, uint32_t addr, uint32_t set_idx, uint32_t tag, bool hit, bool dirty, const char* output_filename);

void random_number_generation();

int replaced_block(Block *set,uint64_t set_index);


uint64_t cache_access(uint64_t address, bool write_cond,uint64_t write_data);


void cache_read(uint64_t address);

void cache_write(uint64_t address,uint64_t value);

void cache_status();

void cache_stats();

void print_valid_cache_sets();

void invalidate_all_entries();