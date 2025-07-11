#include <stdio.h>
#include <stdint.h>
#include "lab7_AI_13_15.h"
#include<stdbool.h>



int load_no;
int global_assoc;

extern int store_flag;
extern int total_lines;
extern char command[100];
extern char filename[50];
extern char filename_d[50];
extern char config_filename[50];
extern int address, count, line;

extern int line_no;
extern int line_number;

extern char compartment[32];
extern int data_section_line_count;


extern int breakpoints[5];
extern int num_of_breakpoints;
extern int current_line_no;

extern char r[8][4];
extern char funct3_R[8][4];
extern char funct7_R[8][8];
extern char opcode_R[8][8];
extern char registers[32][6];
extern char register_aliases[32][6];
extern char I[7][5];
extern char funct3_I[7][4];
extern char funct6_I[3][7];
extern char opcode_I[7][8];
extern char I_S[12][5];
extern char funct3_I_S[12][4];
extern char opcode_I_S[12][8];
extern char U[1][4];
extern char opcode_U[1][8];

extern char B[6][5];
extern char funct3_B[6][4];
extern char opcode_B[6][8];

extern char J[1][4];
extern char opcode_J[1][8];

extern int64_t register_values[32];

extern int PC_1;
extern int PC_2;
extern int PC_2_s;
extern int PC_s;

extern int8_t memory[MEMORY_SIZE]; 

extern Cache riscv_cache;
extern FILE *output_file;

extern int64_t mem_values[MAX_DWORDS];
extern int check_file_num;

extern char lines[MAX_LINES][MAX_LINE_LEN];

extern int just;
extern int num;
extern int jal_reg;

extern int64_t mem_values_dword[MAX_DWORDS];   
extern int64_t mem_values_word[MAX_WORDS];     
extern int64_t mem_values_half[MAX_HALFS];     
extern uint64_t mem_values_byte[MAX_BYTES];  

extern char output_filename[100];
extern char output_dump_file[100];

int dword_count = 0;  
int word_count = 0;  
int half_count = 0;  
int byte_count = 0;

void datasection_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    char line[256];


    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '.') {
            data_section_line_count++;
        }
    }

    fclose(file);

}






void set_breakpoints(int line_no) {
    breakpoints[num_of_breakpoints] = line_no;
    num_of_breakpoints++;
    printf("Break point is set \n", line_no);
}

int breakpoint_found(int line_no) {
    for (int i = 0; i < num_of_breakpoints; i++) {
        if (breakpoints[i] == line_no) {
            return 1;
        }
    }
    return 0;
}

void delete_breakpoint(int line) {
    int found = 0;

    
    for (int i = 0; i < num_of_breakpoints; i++) {
        if (breakpoints[i] == line ) {
            found = 1;
            printf("Breakpoint at line %d deleted\n", line);

            for (int j = i; j < num_of_breakpoints - 1; j++) {
                breakpoints[j] = breakpoints[j + 1];
            }
            num_of_breakpoints--;
            break;
        }
    }

    if (!found) {
        printf("Error: No breakpoint found at line %d\n", line);
    }
}



void print_memory(int64_t addr, int count) {

    for (int i = 0; i < count; i++) {
        printf("Memory[0x%08llX] = 0x%02X\n", addr + i, (unsigned char)memory[addr + i]);

    }
}






void step(const char lines[][MAX_LINE_LEN], int total_lines, int64_t mem_values[], stack* stack_1) {

    if (line_number >= total_lines) {
        printf("Nothing to step\n");
        return;
    }

    PC_s = 4 * (line_number);

    int jal_line_s = 0;


    char line[MAX_LINE_LEN];
    strcpy(line, lines[line_number]);  

    line[strcspn(line, "\n")] = '\0';

        int num_args = count_args_from_input(line);

    char b[10], c[10], d[10], e[20];
    char a[HEX_SIZE] = {0};
    parse_instruction_input(line, b, c, d, e);

    int r_index = validate_R_instruction(b);
    int i_index = validate_I_instruction(b);
    int i_s_index = validate_I_S_instruction(b);
    int u_index = validate_U_instruction(b);
    int b_index = validate_B_instruction(b);
    int j_index = validate_J_instruction(b);

    if (r_index != -1) {
        if (num_args > 4) {
            printf("Exceeded arguments in line %d\n", line_number);
        }
        else if (check_register_format(c) && check_register_format(d) &&
                 check_register_format(e)) {
            execute_R_instruction(a, b, c, d, e);
            stack_1->line_num[stack_1->top_index] = line_number + 1;
        } else {
            printf("Error: Unrecognized instruction at line %d: %s", line_number,
                   line);
        }
    } else if (i_index != -1) {
        if (num_args > 4) {
            printf("Exceeded arguments in line %d\n", line_number);

        }
        else if (check_register_format(c) && check_register_format(d) &&
                 check_immediate_format(e) && !check_register_format(e)) {
            unsigned int e_val = (unsigned int)atoi(e);
            if ((e_val < -2048) && (e_val > 2047)) {
                printf("Immediate out of bounds in line %d\n ", line_number);

            }
            else {
                int e_val = ( int)atoi(e);
                if (((i_index == 6) || (i_index == 4) || (i_index == 5)) && (((e_val < 0) || (e_val > 63)))) {

                    printf("Immediate out of bounds in line %d\n", line_number);

                }
                else {
                    execute_I_instruction(a, b, c, d, e);
                    stack_1->line_num[stack_1->top_index] = line_number + 1;
                }

            }

        } else {
            printf("Error: Unrecognized instruction at line %d: %s", line_number,
                   line);
        }
    } else if (i_s_index != -1) {
        if (num_args > 4) {
            printf("Exceeded arguments in line %d\n", line_number);

        }
        else if (check_register_format(c) && check_register_format(e) &&
                 check_immediate_format(d)) {
            unsigned int d_val = (unsigned int)atoi(d);
            if (-2048 > d_val && d_val > 2047) {
                printf("Immediate out of bounds in line %d\n ", line_number);

            }

            else {
                just = PC_s + 4 ;
                execute_I_S_instruction(a, b, c, d, e, mem_values);
                stack_1->line_num[stack_1->top_index] = line_number + 1;
                if (strcmp(b, "jalr") == 0) {
                    pop_from_stack(stack_1);
                }
                line_number += (just - PC_s - 4) / 4;
            }
        } else {
            printf("Error: Unrecognized instruction at line %d: %s", line_number,
                   line);
        }
    } else if (u_index != -1) {
        if (num_args > 3) {
            printf("Exceeded arguments in line %d\n", line_number);

        }
        else if (check_register_format(c) && check_immediate_format(d)) {
            unsigned int d_val = (unsigned int)atoi(d);

            if ((d_val < 0) || (d_val > 1048575 )) {
                printf("Immediate out of bounds in line %d\n ", line_number);
            }
            else {
                execute_U_instruction(a, b, c, d);
                stack_1->line_num[stack_1->top_index] = line_number + 1;
            }
        } else {
            printf("Error: Unrecognized instruction at line %d: %s", line_number,
                   line);
        }
    } else if (b_index != -1) {
        if (num_args > 4) {
            printf("Exceeded arguments in line %d\n", line_number);

        }

        else if (check_register_format(c) && check_register_format(d)) {
            unsigned int e_val = (unsigned int)atoi(e);
            if ((e_val < -4096) && (e_val > 4095)) {
                printf("Immediate out of bounds in line %d\n ", line_number);

            }
            else {
                execute_B_instruction(a, b, c, d, e, lines, total_lines);
                line_number += (PC_2_s - PC_s - 4) / 4;
                stack_1->line_num[stack_1->top_index] = line_number + 1;
            }
        } else {
            printf("Error: Unrecognized instruction at line %d: %s", line_number,
                   line);
        }

    } else if (j_index != -1) {
        if (num_args > 3) {
            printf("Exceeded arguments in line %d\n", line_number);

        }
        else if (check_register_format(c)) {
            unsigned int k_val = (unsigned int)atoi(d);
            if ((k_val < -1048576) && (k_val > 1048575)) {
                printf("Immediate out of bounds in line %d\n ", line_number);

            }
            else {
                execute_J_instruction(a, b, c, d, lines, total_lines);
                jal_line_s = line_number + 1;

                unsigned int c_val = (unsigned int)atoi(c);

                register_values[c_val] = PC_s + 4;


                strcpy(compartment, d);
                stack_1->line_num[stack_1->top_index] = line_number + 1;
                push_to_stack(stack_1, compartment);
                stack_1->line_num[stack_1->top_index] = line_number + 1;

                line_number += (PC_2_s - PC_s - 4) / 4;

            }
        }
        else {
            printf("Error: Unrecognized instruction at line %d: %s", line_number,
                   line);
        }



    }
    else {
        printf("Error: Unrecognized instruction at line %d: %s", line_number,
               line);
    }



    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Executed %s ", line);
    printf("%s", buffer);
    printf("PC = 0x%08x\n", PC_s);

    line_number++;
}

void handle_instructions_from_lines(char lines[MAX_LINES][MAX_LINE_LEN], int total_lines, int64_t mem_values[], int line_no, int current_line_no, stack* stack_1) {
    int h = 0;

    int zz = 0;
    int jal_line = 0;

    int ja = 0;


    char label_map[MAX_LINES][MAX_LINE_LEN] = {0};
    int line_map[MAX_LINES] = {0};
    int total_labels = create_label_map(lines, total_lines, label_map, line_map);  


    while (line_number < total_lines ) {
        PC_1 = 4 * (line_number);


        char line[MAX_LINE_LEN];
        strncpy(line, lines[line_number], MAX_LINE_LEN);  

        int num_args = count_args_from_input(line);

        char b[10], c[10], d[10], e[20];
        char a[HEX_SIZE] = {0};
        parse_instruction_input(line, b, c, d, e);

        int r_index = validate_R_instruction(b);
        int i_index = validate_I_instruction(b);
        int i_s_index = validate_I_S_instruction(b);
        int u_index = validate_U_instruction(b);
        int b_index = validate_B_instruction(b);
        int j_index = validate_J_instruction(b);

        if (breakpoint_found(line_number + 1 + data_section_line_count) == 1) {
            printf("Execution stopped at breakpoint\n", line_no);
            current_line_no = line_number + 1;
            break;
        }

        if (r_index != -1) {
            if (num_args > 4) {
                printf("Exceeded arguments in line %d\n", line_number);
            }
            else if (check_register_format(c) && check_register_format(d) &&
                     check_register_format(e)) {
                execute_R_instruction(a, b, c, d, e);
                stack_1->line_num[stack_1->top_index] = line_number + 1;
            } else {
                printf("1Error: Unrecognized instruction at line %d: %s", line_number,
                       line);
            }
        }
        else if (i_index != -1) {
            if (num_args > 4) {
                printf("Exceeded arguments in line %d\n", line_number);

            }
            else if (check_register_format(c) && check_register_format(d) &&
                     check_immediate_format(e) && !check_register_format(e)) {
                unsigned int e_val = (unsigned int)atoi(e);
                if ((e_val < -2048) && (e_val > 2047)) {
                    printf("Immediate out of bounds in line %d\n ", line_number);

                }
                else {
                    int e_val = ( int)atoi(e);
                    if (((i_index == 6) || (i_index == 4) || (i_index == 5)) && (((e_val < 0) || (e_val > 63)))) {

                        printf("Immediate out of bounds in line %d\n", line_number);

                    }
                    else {
                        execute_I_instruction(a, b, c, d, e);
                        stack_1->line_num[stack_1->top_index] = line_number + 1;
                    }

                }

            } else {
                printf("2Error: Unrecognized instruction at line %d: %s", line_number,
                       line);
            }
        }
        else if (i_s_index != -1) {
            if (num_args > 4) {
                printf("Exceeded arguments in line %d\n", line_number);

            }
            else if (check_register_format(c) && check_register_format(e) &&
                     check_immediate_format(d)) {
                unsigned int d_val = (unsigned int)atoi(d);
                if (-2048 > d_val && d_val > 2047) {
                    printf("Immediate out of bounds in line %d\n ", line_number);

                }

                else {
                    just = PC_1 + 4;
                    execute_I_S_instruction(a, b, c, d, e, mem_values);
                    if (strcmp(b, "jalr") != 0) {
                        stack_1->line_num[stack_1->top_index] = line_number + 1;

                    }
                    if (strcmp(b, "jalr") == 0) {
                        pop_from_stack(stack_1);
                    }
                    line_number += (just - PC_1 - 4) / 4 ;
                }
            } else {
                printf("3Error: Unrecognized instruction at line %d: %s", line_number,
                       line);
            }


        }
        else if (u_index != -1) {
            if (num_args > 3) {
                printf("Exceeded arguments in line %d\n", line_number);

            }
            
                
                    execute_U_instruction(a, b, c, d);
                    printf("Hi\n");
                    stack_1->line_num[stack_1->top_index] = line_number + 1;
                
            
        }
        else if (b_index != -1) {
            if (num_args > 4) {
                printf("Exceeded arguments in line %d\n", line_number);

            }

            else if (check_register_format(c) && check_register_format(d)) {
                unsigned int e_val = (unsigned int)atoi(e);
                if ((e_val < -4096) && (e_val > 4095)) {
                    printf("Immediate out of bounds in line %d\n ", line_number);

                }
                else {

                    execute_B_instruction(a, b, c, d, e, lines, total_lines);





                    stack_1->line_num[stack_1->top_index] = line_number + 1;
                    line_number += (PC_2 - PC_1 - 4) / 4 ;
                }
            } else {
                printf("5Error: Unrecognized instruction at line %d: %s", line_number,
                       line);
            }


        } else if (j_index != -1) {
            if (num_args > 3) {
                printf("Exceeded arguments in line %d\n", line_number);

            }
            else if (check_register_format(c)) {
                unsigned int k_val = (unsigned int)atoi(d);
                if ((k_val < -1048576) && (k_val > 1048575)) {
                    printf("Immediate out of bounds in line %d\n ", line_number);

                }
                else {
                    execute_J_instruction(a, b, c, d, lines, total_lines);
                    


                    strcpy(compartment, d);
                    stack_1->line_num[stack_1->top_index] = line_number + 1;
                    push_to_stack(stack_1, compartment);
                    stack_1->line_num[stack_1->top_index] = line_number + 1;
                    line_number += (PC_2 - PC_1 - 4) / 4 ;

                }
            }
        }
        else {
            printf("6Error: Unrecognized instruction at line %d: %s", line_number,
                   line);
        }



        if (PC_1 > 4 * (total_lines - 1)) {
            printf("Reached the last line. Exiting.\n");
            break;  
        }


        char buffer[100];
        snprintf(buffer, sizeof(buffer), "Executed %s ", line);
        printf("%s", buffer);
        printf("PC = 0x%08x\n", PC_1);



        line_number++;
        zz++;

    }

}

int find_label_position(const char *label, const char lines[MAX_LINES][MAX_LINE_LEN], int total_lines) {
    char stripped_label[10];
    snprintf(stripped_label, sizeof(stripped_label), "%s:", label);  
    for (int i = 0; i < total_lines; i++) {
        if (strstr(lines[i], stripped_label) == lines[i]) {  
            return i;  
        }
    }
    return -1;  
}

long get_line_offset(int line_number, const char lines[MAX_LINES][MAX_LINE_LEN]) {
    long offset = 0;

    for (int i = 0; i < line_number; i++) {
        size_t line_length = strlen(lines[i]);

        if (line_length > 0 && lines[i][line_length - 1] == ':') {
            continue;
        }

        offset += line_length + 1; 
    }

    return offset; 
}



int count_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    
    int line_count = 0;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), file)) {
        if (buffer[0] == '.') {
            continue;  
        }

        line_count++;
    }

    fclose(file);
    return line_count;
}



void parse_instruction_input(char input[], char b[], char c[], char d[], char e[]) {
  b[0] = c[0] = d[0] = e[0] = '\0';

  char *colon_ptr = strchr(input, ':');
  if (colon_ptr != NULL) {
    input = colon_ptr + 1;
  }

  int start = 0;
  while (isspace(input[start])) {
    start++;
  }

  int i = start;
  while (input[i] && input[i] != ' ' && input[i] != ',' && input[i] != '(' &&
         input[i] != '\n') {
    i++;
  }
  strncpy(b, input + start, i - start);
  b[i - start] = '\0';
  start = i;

  while (input[start] == ',' || isspace(input[start])) {
    start++;
  }

  i = start;
  while (input[i] && input[i] != ',' && input[i] != '(' && input[i] != ' ' &&
         input[i] != '\n') {
    i++;
  }
  strncpy(c, input + start, i - start);
  c[i - start] = '\0';
  start = i;

  while (input[start] == ',' || isspace(input[start])) {
    start++;
  }

  i = start;
  while (input[i] && input[i] != ',' && input[i] != '(' && input[i] != ' ' &&
         input[i] != '\n') {
    i++;
  }
  strncpy(d, input + start, i - start);
  d[i - start] = '\0';
  start = i;

  while (input[start] == ',' || isspace(input[start])) {
    start++;
  }

  if (input[start] == '(') {
    start++;
    i = start;
    while (input[i] && input[i] != ')' && input[i] != ' ' && input[i] != '\n') {
      i++;
    }
    strncpy(e, input + start, i - start);
    e[i - start] = '\0';
    start = i;
  }

  if (input[start] && (input[start] != ')' && input[start] != '\n')) {
    i = start;
    while (input[i] && input[i] != ' ' && input[i] != '\n') {
      i++;
    }
    strncpy(e, input + start, i - start);
    e[i - start] = '\0';
  }

  for (int j = 0; j < 4; j++) {
    char *p = (j == 0 ? b : (j == 1 ? c : (j == 2 ? d : e)));
    int len = strlen(p);
    while (len > 0 && isspace(p[len - 1])) {
      p[--len] = '\0';
    }
    p[len] = '\0';
  }
}

int validate_R_instruction(const char array[]) {
  for (int g = 0; g < 8; g++) {
    if (strcmp(array, r[g]) == 0) {
      return g;
    }
  }
  return -1;
}

int validate_I_instruction(const char array[]) {
  for (int h = 0; h < 7; h++) {
    if (strcmp(array, I[h]) == 0) {
      return h;
    }
  }
  return -1;
}

int validate_I_S_instruction(const char array[]) {
  for (int c = 0; c < 12; c++) {
    if (strcmp(array, I_S[c]) == 0) {
      return c;
    }
  }
  return -1;
}

int validate_U_instruction(const char array[]) {

  if (strcmp(array, U[0]) == 0) {
    return 0;
  }
  return -1;
}

int validate_B_instruction(const char array[]) {
  for (int l = 0; l < 6; l++) {
    if (strcmp(array, B[l]) == 0) {
      return l;
    }
  }
  return -1;
}

int validate_J_instruction(const char array[]) {
  for (int y = 0; y < 6; y++) {
    if (strcmp(array, J[y]) == 0) {
      return y;
    }
  }
  return -1;
}

void execute_R_instruction(char a[], char b[], char c[], char d[], char e[]) {
  char binary_R[33] = {0};
  int g = validate_R_instruction(b);


  for (int i = 0; i < 32; i++) {
    if (strcmp(c, register_aliases[i]) == 0)
      strcpy(c, registers[i]);
    if (strcmp(d, register_aliases[i]) == 0)
      strcpy(d, registers[i]);
    if (strcmp(e, register_aliases[i]) == 0)
      strcpy(e, registers[i]);
  }

  memmove(c, c + 1, strlen(c));
  memmove(d, d + 1, strlen(d));
  memmove(e, e + 1, strlen(e));

  unsigned int c_val = (unsigned int)atoi(c);
  unsigned int d_val = (unsigned int)atoi(d);
  unsigned int e_val = (unsigned int)atoi(e);


  if(strcmp(b,"add")==0){
  register_values[c_val]=register_values[d_val]+register_values[e_val];
}

  if(strcmp(b,"sub")==0){
  register_values[c_val]=register_values[d_val]-register_values[e_val];
}  

  if(strcmp(b,"or")==0){
  register_values[c_val]=register_values[d_val] | register_values[e_val];
}

  if(strcmp(b,"xor")==0){
  register_values[c_val]=register_values[d_val] ^ register_values[e_val];
}

  if(strcmp(b,"and")==0){
  register_values[c_val]=register_values[d_val] & register_values[e_val];
}

  if(strcmp(b,"sll")==0){
  register_values[c_val]=register_values[d_val] << register_values[e_val];
}

  if(strcmp(b,"srl")==0){
  register_values[c_val]=register_values[d_val] >> register_values[e_val];
}

  if(strcmp(b,"sra")==0){
  register_values[c_val]=(int)register_values[d_val] >> (int)register_values[e_val];
}  



 register_values[0]=0;
}

void execute_I_instruction(char a[], char b[], char c[], char d[], char e[]) {
  char binary_I[33] = {0};
  int v = validate_I_instruction(b);

  for (int i = 0; i < 32; i++) {

    if (strcmp(c, register_aliases[i]) == 0)
      strcpy(c, registers[i]);
    if (strcmp(d, register_aliases[i]) == 0)
      strcpy(d, registers[i]);
  }

  memmove(c, c + 1, strlen(c));
  memmove(d, d + 1, strlen(d));
  memmove(e, e, strlen(e));

  unsigned int c_val = (unsigned int)strtoul(c, NULL, 0);  
unsigned int d_val = (unsigned int)strtoul(d, NULL, 0);  
 int64_t e_val = strtoull(e, NULL, 0);

if (strcmp(b, "addi") == 0) {
    register_values[c_val] = register_values[d_val] + e_val;
}

if (strcmp(b, "ori") == 0) {
    register_values[c_val] = register_values[d_val] | e_val;
}

if (strcmp(b, "xori") == 0) {
    register_values[c_val] = register_values[d_val] ^ e_val;
}

if (strcmp(b, "andi") == 0) {
    register_values[c_val] = register_values[d_val] & e_val;
}

if (strcmp(b, "slli") == 0) {
    register_values[c_val] = register_values[d_val] << e_val;
}

if (strcmp(b, "srli") == 0) {
    register_values[c_val] = register_values[d_val] >> e_val;
}

if (strcmp(b, "srai") == 0) {
    register_values[c_val] = (int)register_values[d_val] >> (int)e_val;
}

   register_values[0]=0;

}


void execute_I_S_instruction(char a[], char b[], char c[], char d[], char e[], int64_t mem_values[]) {
    int w = validate_I_S_instruction(b);

    if (w == -1) {
        return;
    }

    for (int i = 0; i < 32; i++) {
        if (strcmp(c, register_aliases[i]) == 0)
            strcpy(c, registers[i]);
        if (strcmp(e, register_aliases[i]) == 0)
            strcpy(e, registers[i]);
    }

    memmove(c, c + 1, strlen(c));
    memmove(d, d, strlen(d));
    memmove(e, e + 1, strlen(e));

    unsigned int c_val = (unsigned int)atoi(c);   
    int d_val;
    sscanf(d, "%d", &d_val);
    unsigned int e_val = (unsigned int)atoi(e);  





 if (strcmp(b, "sd") == 0) {
    store_flag = 8;
    int64_t memory_address = register_values[e_val] + d_val;
    uint64_t value_to_store = register_values[c_val];

    if (memory_address + 8 > MEMORY_SIZE) {
        printf("Error: Memory address 0x%08llX out of bounds\n", memory_address);
        return;
    }

    for (int i = 0; i < 8; i++) {
        memory[memory_address + i] = (value_to_store >> (i * 8)) & 0xFF;
    }
    
    cache_access(memory_address,true,value_to_store);
    printf("%d\n",value_to_store);
   
}


else if (strcmp(b, "sw") == 0) {
    store_flag = 4;
    int64_t memory_address = register_values[e_val] + d_val;
    uint32_t value_to_store = register_values[c_val];  

    if (memory_address + 4 > MEMORY_SIZE) {
        printf("Error: Memory address 0x%08llX out of bounds\n", memory_address);
        return;
    }

    for (int i = 0; i < 4; i++) {
        memory[memory_address + i] = (value_to_store >> (i * 8)) & 0xFF;
    }

    cache_access(memory_address,true,value_to_store);
    printf("%d\n",value_to_store);

  
}

else if (strcmp(b, "sb") == 0) {
    store_flag = 1;
    int64_t memory_address = register_values[e_val] + d_val;
    uint8_t value_to_store = register_values[c_val];  

    if (memory_address + 1 > MEMORY_SIZE) {
        printf("Error: Memory address 0x%08llX out of bounds\n", memory_address);
        return;
    }

    memory[memory_address] = value_to_store & 0xFF;

    cache_access(memory_address,true,value_to_store);
    printf("%d\n",value_to_store);

}


else if (strcmp(b, "sh") == 0) {
    store_flag = 2;
    int64_t memory_address = register_values[e_val] + d_val;
    uint16_t value_to_store = register_values[c_val]; 

    if (memory_address + 2 > MEMORY_SIZE) {
        printf("Error: Memory address 0x%08llX out of bounds\n", memory_address);
        return;
    }

    for (int i = 0; i < 2; i++) {
        memory[memory_address + i] = (value_to_store >> (i * 8)) & 0xFF;
    }

    cache_access(memory_address,true,value_to_store);
    printf("%d\n",value_to_store);
}




else if (strcmp(b, "lb") == 0) {
    load_no =1;
    int8_t loaded_value = 0;
    int64_t offset = d_val;

    int64_t effective_address = register_values[e_val] + offset;

    if(riscv_cache.enabled == false){

    
    
     if (effective_address >= 0 && effective_address < MEMORY_SIZE) {
        loaded_value = memory[effective_address];  
        register_values[c_val] = (uint64_t)(int64_t)(int8_t)loaded_value;  
    }



}

else if(riscv_cache.enabled == true){
    uint64_t loaded_val = cache_access(effective_address, false, 0);

register_values[c_val] = loaded_val;
}
}

else if (strcmp(b, "lbu") == 0) {
    load_no = 1;
    uint8_t loaded_value = 0;
    int64_t offset = d_val;

    int64_t effective_address = register_values[e_val] + offset;

    if(riscv_cache.enabled == false){



     if (effective_address >= 0 && effective_address < MEMORY_SIZE) {
        loaded_value = memory[effective_address];
        register_values[c_val] = (uint64_t)loaded_value;
    }


}
else if(riscv_cache.enabled == true){

    uint64_t loaded_val = cache_access(effective_address, false, 0);
register_values[c_val] = loaded_val;
}

}



else if (strcmp(b, "lh") == 0) {
    load_no = 2;
    int16_t loaded_value = 0;
    int64_t offset = d_val;

    int64_t effective_address = register_values[e_val] + offset;

    if(riscv_cache.enabled == false){

     if (effective_address >= 0 && effective_address < MEMORY_SIZE) {
        for (int i = 0; i < 2; i++) {  
            loaded_value |= ((int64_t)memory[effective_address + i] & 0xFF) << (i * 8);
        }
        register_values[c_val] = (uint64_t)(int64_t)(int16_t)loaded_value;  
    }

}
else if(riscv_cache.enabled == true){

    uint64_t loaded_val = cache_access(effective_address, false, 0);
register_values[c_val] = loaded_val;
}
}


else if (strcmp(b, "lhu") == 0) {
    load_no= 2;
    uint16_t loaded_value = 0;
    int64_t offset = d_val;

    int64_t effective_address = register_values[e_val] + offset;

    if(riscv_cache.enabled == false){

        if (effective_address >= 0 && effective_address < MEMORY_SIZE) {
        for (int i = 0; i < 2; i++) {
            loaded_value |= ((int64_t)memory[effective_address + i] ) << (i * 8);
        }
        register_values[c_val] = (uint64_t)loaded_value;  // Store in register without sign extension
    }


}
else if(riscv_cache.enabled == true){

    uint64_t loaded_val = cache_access(effective_address, false, 0);
register_values[c_val] = loaded_val;
}
}




else if (strcmp(b, "lw") == 0) {
    load_no = 4;
    int32_t loaded_value = 0;
    int64_t offset = d_val;

     int64_t effective_address = register_values[e_val] + offset;

     if(riscv_cache.enabled == false){

        if (effective_address >= 0 && effective_address < MEMORY_SIZE) {
        for (int i = 0; i < 4; i++) {  
            loaded_value |= ((int64_t)memory[effective_address + i] & 0xFF) << (i * 8);
            printf("%d\n", ((int64_t)memory[effective_address + i] & 0xFF) << (i * 8));
        }
        register_values[c_val] = (uint64_t)(int64_t)(int32_t)loaded_value;  
    }
}

else if(riscv_cache.enabled == true){
    uint64_t loaded_val = cache_access(effective_address, false, 0);

register_values[c_val] = loaded_val;
}

}

else if (strcmp(b, "lwu") == 0) {
    load_no = 4;
    uint32_t loaded_value = 0;
    int64_t offset = d_val;

    int64_t effective_address = register_values[e_val] + offset;

    if(riscv_cache.enabled == false){

        if (effective_address >= 0 && effective_address < MEMORY_SIZE) {
        for (int i = 0; i < 4; i++) {
            loaded_value |= ((uint64_t)memory[effective_address + i] & 0xFF) << (i * 8);
        }
        register_values[c_val] = (uint64_t)loaded_value; 
    }
}

else if(riscv_cache.enabled == true){


    uint64_t loaded_val = cache_access(effective_address, false, 0);
register_values[c_val] = loaded_val;
    }

}





else if (strcmp(b, "ld") == 0) {
    load_no =8;
    int64_t loaded_value = 0;

    int64_t offset = d_val;
    int64_t effective_address = register_values[e_val] + offset;


    if(riscv_cache.enabled == false){
        if (effective_address >= 0 && effective_address < MEMORY_SIZE) {
        for (int i = 0; i < 8; i++) {
            loaded_value |= ((int64_t)memory[effective_address + i] & 0xFF) << (i * 8);
        }
        register_values[c_val] = loaded_value;  
    }

    }

    else if(riscv_cache.enabled == true){
        uint64_t loaded_val = cache_access(effective_address, false, 0);
register_values[c_val] = loaded_val;
    }

store_flag = 0;

}





    else if (strcmp(b,"jalr")==0){

          just = register_values[jal_reg] + d_val;


    }

     register_values[0]=0;



}







int load_file_into_arrays(const char *filename, char lines[MAX_LINES][MAX_LINE_LEN],
                          int64_t mem_values_dword[MAX_DWORDS], 
                          int64_t mem_values_word[MAX_WORDS], 
                          int64_t mem_values_half[MAX_HALFS], 
                          uint64_t mem_values_byte[MAX_BYTES]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }
    dword_count = 0;  
    word_count = 0;  
    half_count = 0;  
    byte_count = 0;  
    int line_count = 0;
    char buffer[MAX_LINE_LEN];
    bool text_section_started = false;
    while (fgets(buffer, MAX_LINE_LEN, file)) {
        buffer[strcspn(buffer, "\n\r")] = '\0';  
        if (strncmp(buffer, ".data", 5) == 0) {
            continue;
        }
        if (strncmp(buffer, ".text", 5) == 0) {
            text_section_started = true;
            continue;
        }
        
        if (strncmp(buffer, ".dword", 6) == 0) {
            char *token = strtok(buffer + 7, ", ");  
            while (token && dword_count < MAX_DWORDS) {
                mem_values_dword[dword_count++] = strtoull(token, NULL, 0);  
                token = strtok(NULL, ", ");
            }
        } else if (strncmp(buffer, ".word", 5) == 0) {
            char *token = strtok(buffer + 6, ", ");  
            while (token && word_count < MAX_WORDS) {
                mem_values_word[word_count++] = (int32_t)strtol(token, NULL, 0);  
                token = strtok(NULL, ", ");
            }
        } else if (strncmp(buffer, ".half", 5) == 0) {
            char *token = strtok(buffer + 6, ", ");  
            while (token && half_count < MAX_HALFS) {
                mem_values_half[half_count++] = (int16_t)strtol(token, NULL, 0);  
                token = strtok(NULL, ", ");
            }
        } else if (strncmp(buffer, ".byte", 5) == 0) {
            char *token = strtok(buffer + 6, ", ");  
            while (token && byte_count < MAX_BYTES) {
                mem_values_byte[byte_count++] = (uint8_t)strtol(token, NULL, 0);  
                token = strtok(NULL, ", ");
            }
        } else if (line_count < MAX_LINES) {
            strncpy(lines[line_count++], buffer, MAX_LINE_LEN - 1);
            lines[line_count - 1][MAX_LINE_LEN - 1] = '\0';  
        }
        
        if (line_count >= MAX_LINES) {
            break;
        }
    }
    

    fclose(file);
    return line_count;  
}




void execute_U_instruction(char a[], char b[], char c[], char d[]) {
    int p = validate_U_instruction(b);
    if (p == -1) {
        return;
    }

    
    for (int i = 0; i < 32; i++) {
        if (strcmp(c, register_aliases[i]) == 0) {
            strcpy(c, registers[i]);
        }
    }
    memmove(c, c + 1, strlen(c));
    unsigned long long u_imm = strtoull(d, NULL, 0);
    unsigned int c_val = (unsigned int)atoi(c);
    u_imm = u_imm << 12;
    register_values[c_val] = u_imm;
    int start_address = register_values[c_val];

    int current_address = start_address;

    
    for (int i = 0; i < dword_count; i++) {
        int64_t value_to_store = mem_values_dword[i];
        for (int j = 0; j < 8; j++) {  
            memory[current_address + j] = (value_to_store >> (j * 8)) & 0xFF;
        }
        current_address += 8;
    }

    
    for (int i = 0; i < word_count; i++) {
        int32_t value_to_store = mem_values_word[i];
      

        for (int j = 0; j < 4; j++) {  
            memory[current_address + j] = (value_to_store >> (j * 8)) & 0xFF;
        }
        current_address += 4;
    }

    for (int i = 0; i < half_count; i++) {
        int16_t value_to_store = mem_values_half[i];
        for (int j = 0; j < 2; j++) {  
            memory[current_address + j] = (value_to_store >> (j * 8)) & 0xFF;
        }
        current_address += 2;
    }

    for (int i = 0; i < byte_count; i++) {
        uint8_t value_to_store = mem_values_byte[i];
        memory[current_address] = value_to_store;  
        current_address += 1;
    }

    register_values[0] = 0;
}


void process_file_lines(char lines[MAX_LINES][MAX_LINE_LEN], int line_count) {
  for (int i = 0; i < line_count; i++) {
  }
}



void execute_B_instruction(char a[], char b[], char c[], char d[], char e[], const char lines[MAX_LINES][MAX_LINE_LEN], int total_lines) {
    int offset;
    int offset_s;
    int target_line = -1;
    int current_line = PC_1 / 4;  
    int current_line_s = PC_s/4;

   target_line = find_label_position(e, lines, total_lines);

    if (target_line == -1) {

      if (isdigit(e[0]) || (e[0] == '-' && isdigit(e[1]))) {
            offset = atoi(e);
            offset_s = offset; 
        } else {
        printf("Error: Label '%s' not found.\n", e);
        return;
      }
    }

    else {

    offset = (target_line - current_line) * 4;
    offset_s = (target_line - current_line_s) * 4;
   }

    memmove(c, c + 1, strlen(c));  
    memmove(d, d + 1, strlen(d));
    unsigned int c_val = (unsigned int)atoi(c);
    unsigned int d_val = (unsigned int)atoi(d);

    if (strcmp(b, "beq") == 0) {
        if (register_values[c_val] == register_values[d_val]) {
            PC_2 = PC_1 + offset;
            PC_2_s = PC_s + offset_s;
        } else {
            PC_2 = PC_1 + 4;
            PC_2_s = PC_s +4;
        }
    } else if (strcmp(b, "bne") == 0) {
        if (register_values[c_val] != register_values[d_val]) {
            PC_2 = PC_1 + offset;
            PC_2_s = PC_s + offset_s;
        } else {
            PC_2 = PC_1 + 4;
            PC_2_s = PC_s +4;
        }
    } else if (strcmp(b, "blt") == 0) {
        if ((int64_t)register_values[c_val] < (int64_t)register_values[d_val]) {
            PC_2 = PC_1 + offset;
            PC_2_s = PC_s + offset_s;
        } else {
            PC_2 = PC_1 + 4;
            PC_2_s = PC_s + 4;
        }
    } else if (strcmp(b, "bge") == 0) {
        if (register_values[c_val] >= register_values[d_val]) {
            PC_2 = PC_1 + offset;
            PC_2_s = PC_s + offset_s;
        } else {
            PC_2 = PC_1 + 4;
            PC_2_s = PC_s +4;
        }
    }

    else if (strcmp(b, "bltu") == 0) {
    if ((uint64_t)register_values[c_val] < (uint64_t)register_values[d_val]) {
        PC_2 = PC_1 + offset;
        PC_2_s = PC_s + offset_s;
    } else {
        PC_2 = PC_1 + 4;
        PC_2_s = PC_s + 4;
    }
} 
else if (strcmp(b, "bgeu") == 0) {
    if ((uint64_t)register_values[c_val] >= (uint64_t)register_values[d_val]) {
        PC_2 = PC_1 + offset;
        PC_2_s = PC_s + offset;
    } else {
        PC_2 = PC_1 + 4;
        PC_2_s = PC_s + 4;
    }
}


     register_values[0]=0;

}


void execute_J_instruction(char a[], char b[], char c[], char d[], const char lines[MAX_LINES][MAX_LINE_LEN], int total_lines) {
    int offset;
    int offset_s;
    int target_line = -1;
    int current_line = PC_1 / 4;
    int current_line_s = PC_s/4;

    target_line = find_label_position(d, lines, total_lines);

    if (target_line == -1) {
      if (isdigit(d[0]) || (d[0] == '-' && isdigit(d[1]))) {
            offset = atoi(d);
            offset_s = offset;  
        } else {
            printf("Error: Label '%s' not found or invalid offset.\n", d);
            return;
        }

    }

    else {

    offset = (target_line - current_line) * 4;
    offset_s = (target_line - current_line_s) * 4;
   }


    memmove(c, c + 1, strlen(c));
    unsigned int c_val = (unsigned int)atoi(c);

    

    jal_reg = c_val;
    register_values[c_val] = PC_1 + 4;
    
    PC_2 = PC_1 + offset;
    PC_2_s = PC_s + offset_s;
     register_values[0]=0;



}





int check_immediate_format(char *num) {
    if (!num || !*num) {
        return 0; 
    }

    if (*num == '+' || *num == '-') {
        num++;
    }

    if (!*num) {
        return 0; 
    }

    if (*num == '0' && (num[1] == 'x' || num[1] == 'X')) {
        num += 2;  
        if (!*num) {
            return 0;  
        }

        while (*num) {
            if (!isxdigit((unsigned char)*num)) {
                return 0;  
            }
            num++;
        }
    } else {
        while (*num) {
            if (!isdigit((unsigned char)*num)) {
                return 0;  
            }
            num++;
        }
    }

    return 1;  
}


int create_label_map(char lines[MAX_LINES][MAX_LINE_LEN], int total_lines, char label_map[MAX_LINES][MAX_LINE_LEN], int line_map[MAX_LINES]) {
    int label_count = 0;
    for (int i = 0; i < total_lines; i++) {
        char line_copy[MAX_LINE_LEN];
        strncpy(line_copy, lines[i], MAX_LINE_LEN);  
        line_copy[MAX_LINE_LEN - 1] = '\0';  

        char *label_ptr = strchr(line_copy, ':');
        if (label_ptr != NULL) {
            *label_ptr = '\0';  
            strcpy(label_map[label_count], line_copy);  
            line_map[label_count] = i;  
            label_count++;
        }
    }
    return label_count;  
}

int count_args_from_input(char input[]) {
  int count = 0;
  int start = 0;
  int in_parentheses = 0;

  while (isspace(input[start])) {
    start++;
  }

  char *colon_ptr = strchr(input, ':');
  if (colon_ptr != NULL) {
    start = colon_ptr - input + 1;
    while (isspace(input[start])) {
      start++;
    }
  }

  while (input[start] != '\0') {
    if (input[start] == '(') {
      in_parentheses = 1;
      start++;
      while (input[start] && input[start] != ')') {
        start++;
      }
      if (input[start] == ')') {
        start++;
      }
      count++;
      while (isspace(input[start]) || input[start] == ',') {
        start++;
      }
    } else {
      if (!isspace(input[start]) && input[start] != ',' && input[start] != '(') {
        count++;
        while (input[start] && !isspace(input[start]) && input[start] != ',' && input[start] != '(') {
          start++;
        }
      }
      while (isspace(input[start]) || input[start] == ',') {
        start++;
      }
    }
  }

  return count;
}

int check_register_format(char *reg) {
  if (reg[0] == 'x') {
    if (reg[1] != '\0') {
      int reg_num = atoi(reg + 1);
      return (reg_num >= 0 && reg_num < 32);
    } else {
      return 0; 
    }
  }

  for (int i = 0; i < 32; i++) {
    if (strcmp(reg, register_aliases[i]) == 0) {
      return 1;
    }
  }
  return 0;
}




void initialize_cache(const char *config_file) {
    FILE *f = fopen(config_file, "r");
    if(!f) {
        printf("Error: Config file not found\n");
        return;
    }

    


    fscanf(f, "%d %d %d %s %s",&riscv_cache.cache_size,&riscv_cache.block_size,&riscv_cache.associativity,riscv_cache.replacement_policy,riscv_cache.write_policy);
    fclose(f); 

    global_assoc = riscv_cache.associativity;

    if (riscv_cache.associativity == 0) {
        int assoc = riscv_cache.cache_size / riscv_cache.block_size;
        riscv_cache.associativity = assoc;
        riscv_cache.number_of_sets = 1;  
    } else {

    riscv_cache.number_of_sets = riscv_cache.cache_size / (riscv_cache.block_size * riscv_cache.associativity);
}
    riscv_cache.fifo_indices = (int *)calloc(riscv_cache.number_of_sets, sizeof(int));

    riscv_cache.sets = (Block **)malloc(riscv_cache.number_of_sets * sizeof(Block *));
    

    for(int i=0;i < riscv_cache.number_of_sets; i++){
        riscv_cache.sets[i] = (Block *)malloc(riscv_cache.associativity * sizeof(Block));
        

        for(int j=0;j < riscv_cache.associativity;j++){
            riscv_cache.sets[i][j].valid = false;
            riscv_cache.sets[i][j].dirty = false;
            riscv_cache.sets[i][j].tag = 0;
            riscv_cache.sets[i][j].last_access = 0;
            riscv_cache.sets[i][j].data = (unsigned char *)malloc(riscv_cache.block_size);
            if (!riscv_cache.sets[i][j].data) {
                printf("Error: Failed to allocate data for block %d in set %d\n", j, i);
                for (int k = 0; k < j; k++) {
                    free(riscv_cache.sets[i][k].data);
                }
                for (int k = 0; k < i; k++) {
                    for (int l = 0; l < riscv_cache.associativity; l++) {
                        free(riscv_cache.sets[k][l].data);
                    }
                    free(riscv_cache.sets[k]);
                }
                free(riscv_cache.sets[i]);
                free(riscv_cache.sets);
                free(riscv_cache.fifo_indices);
                return;
            }
        }

        riscv_cache.fifo_indices[i] = 0 ;
    }

    riscv_cache.enabled = false;
    riscv_cache.accesses = 0;
    riscv_cache.hits = 0;
    random_number_generation();
}



int find_block(Block *set, uint32_t tag) {
    for(int i=0;i < riscv_cache.associativity;i++){
        if(set[i].valid && set[i].tag == tag){
            return i;
        }
    }
    return -1;
}

void free_cache() {
    if(riscv_cache.sets) {
        for(int i=0;i < riscv_cache.number_of_sets;i++){
            free(riscv_cache.sets[i]);
        }
        free(riscv_cache.sets);
        free(riscv_cache.fifo_indices);       
        riscv_cache.sets = NULL;
        riscv_cache.fifo_indices = NULL;      
    }
    riscv_cache.enabled = false;
}




void log_access(char op, uint32_t addr, uint32_t set_idx, uint32_t tag, bool hit, bool dirty, const char* output_filename) {
    FILE *file = fopen(output_filename, "a");  
    if (file == NULL) {
        perror("Error opening log file");
        return;
    }

    fprintf(file, "%c: Address: 0x%x, Set: 0x%x, %s, Tag: 0x%x, %s\n",
            op, addr, set_idx, hit ? "Hit" : "Miss", tag,
            dirty ? "Dirty" : "Clean");

    fclose(file);
}

void random_number_generation() {    
    srand(time(NULL));
}

int replaced_block(Block *set,uint64_t set_index){
    for(int i=0;i < riscv_cache.associativity;i++){
        if(!set[i].valid) return i;
    }

    if(strcmp(riscv_cache.replacement_policy, "LRU")==0) {
        int lru_index =0;
        uint32_t lru_time = set[0].last_access;
        for(int i=1; i < riscv_cache.associativity; i++) {
            if(set[i].last_access < lru_time) {
                lru_time = set[i].last_access;
                lru_index = i;
            }
        }

        return lru_index;
    }
    else if (strcmp(riscv_cache.replacement_policy, "FIFO") == 0) {

        int fifo_index = riscv_cache.fifo_indices[set_index];
        riscv_cache.fifo_indices[set_index] = (fifo_index + 1) % riscv_cache.associativity;
        return fifo_index;
    
    } 
    else if (strcmp(riscv_cache.replacement_policy, "RANDOM") == 0) {
        

        return rand() % riscv_cache.associativity;

    }
}


uint64_t cache_access(uint64_t address, bool write_cond,uint64_t write_data)  {
    if(!riscv_cache.enabled) return 0;

    riscv_cache.accesses++;
    uint64_t set_index;
    uint64_t tag;

 if (riscv_cache.number_of_sets == 1) {  
        set_index = 0;
        tag = address / riscv_cache.block_size;
    } else {  

     set_index = (address / riscv_cache.block_size) % riscv_cache.number_of_sets;
     tag = address / (riscv_cache.block_size * riscv_cache.number_of_sets);
}

    uint64_t block_offset = address % riscv_cache.block_size;

    Block *set = riscv_cache.sets[set_index];

    int block_index = find_block(set,tag);
    bool hit = (block_index != -1);

    char access_type;
if (write_cond) {
    access_type = 'W';
} else {
    access_type = 'R';
}

    if(hit) {
        
        riscv_cache.hits++;
        set[block_index].last_access = riscv_cache.accesses;

        if(write_cond){
            for (int i = 0; i < 8; i++) {
                set[block_index].data[block_offset + i] = (write_data >> (i * 8)) & 0xFF;
            }
            

         if(strcmp(riscv_cache.write_policy, "WT")==0) {
                for(int i = 0; i < 8; i++) {
                    if(address + i < MEMORY_SIZE) {
                        memory[address + i] = (write_data >> (i * 8)) & 0xFF;

                    }
                }
                        log_access(access_type, address, set_index, tag, hit, false ,output_filename);

            } else if(strcmp(riscv_cache.write_policy, "WB")==0) { 
                        log_access(access_type, address, set_index, tag, hit, set[block_index].dirty,output_filename);

                set[block_index].dirty = true;

            }

        }

        else {
                                    log_access(access_type, address, set_index, tag, hit, set[block_index].dirty,output_filename);

        }

        uint64_t loaded_read = 0;
        for (int i = 0; i < load_no; i++) {
            loaded_read |= ((uint64_t)set[block_index].data[block_offset + i] & 0xFF) << (i * 8);
        }
        volatile uint64_t return_value = loaded_read;

            return return_value;
    }

    else {

        if(write_cond && strcmp(riscv_cache.write_policy, "WT")==0) {
            for(int i = 0; i < 8; i++) {
                if(address + i < MEMORY_SIZE) {
                    memory[address + i] = (write_data >> (i * 8)) & 0xFF;
                }
            }

            log_access(access_type, address, set_index, tag, hit, false,output_filename);



            return write_data;
        }

        block_index = replaced_block(set,set_index);

        if (set[block_index].valid && set[block_index].dirty && strcmp(riscv_cache.write_policy, "WB") == 0) {
    uint64_t old_addr = set[block_index].tag * riscv_cache.block_size * riscv_cache.number_of_sets +
                        set_index * riscv_cache.block_size;
    for (int i = 0; i < riscv_cache.block_size; i++) {
        if (old_addr + i < MEMORY_SIZE) {
            memory[old_addr + i] = set[block_index].data[ block_offset + i];
        }
    }
    set[block_index].dirty = false;
}

uint64_t block_start_addr = address - block_offset;
for (int i = 0; i < riscv_cache.block_size; i++) {
    if (block_start_addr + i < MEMORY_SIZE) {
        set[block_index].data[i] = memory[block_start_addr + i];
    } else {
        set[block_index].data[i] = 0;
    }
}

        
        
        set[block_index].valid=true;
        set[block_index].tag=tag;
        set[block_index].last_access = riscv_cache.accesses;

            log_access(access_type, address, set_index, tag, hit, set[block_index].dirty,output_filename);

        if(write_cond && strcmp(riscv_cache.write_policy, "WB")==0) {
            for (int i = 0; i < 8; i++) {
                set[block_index].data[block_offset + i] = (write_data >> (i * 8)) & 0xFF;
            }
            set[block_index].dirty = true;
        }
        
        uint64_t loaded_te = 0;
        for (int i = 0; i < load_no; i++) {
            loaded_te |= ((uint64_t)set[block_index].data[block_offset + i] & 0xFF) << (i * 8);
        }
            volatile uint64_t return_value = loaded_te;


            return return_value;
    }

    



}


void cache_read(uint64_t address) {
    cache_access(address, false,0);
}

void cache_write(uint64_t address,uint64_t value) {
    cache_access(address, true,value);
}

void cache_status() {
    printf("Cache simulation: %s\n", riscv_cache.enabled ? "Enabled" : "Disabled");
    if (riscv_cache.enabled) {
        printf("Cache Size: %d\n", riscv_cache.cache_size);
        printf("Block Size: %d\n", riscv_cache.block_size);
        printf("Associativity: %d\n", global_assoc);
        printf("Replacement Policy: %s\n", riscv_cache.replacement_policy);
        printf("Write Policy: %s\n", riscv_cache.write_policy);
    }
}

void cache_stats() {
    if (!riscv_cache.enabled) return;

    float hit_rate = 0.0f;
if (riscv_cache.accesses > 0) {
    hit_rate = (float)riscv_cache.hits / riscv_cache.accesses;
}
    
    printf("D-cache statistics: Accesses=%d, Hit=%d, Miss=%d, Hit Rate=%.4f\n",
           riscv_cache.accesses, riscv_cache.hits, 
           riscv_cache.accesses - riscv_cache.hits,
           hit_rate);
}



stack* createemptystack() {
    stack* s = (stack*)malloc(sizeof(stack));
    if (s == NULL) {
        return NULL;
    }
    s->top_index = -1;
    return s;
}

bool push_to_stack(stack *s, char* value) {
    if (s->top_index >= (maxstacksize - 1)) {
        return false;
    }
    s->value[++(s->top_index)] = value;
    return true;
}

char* pop_from_stack(stack* s) {
    if (s->top_index == -1) {
        return NULL;
    }
    return s->value[(s->top_index)--]; 
}

char* top(stack* s) {
    if (s->top_index == -1) {
        return NULL;
    }
    return s->value[s->top_index];  
}


void print_valid_cache_sets() {
    FILE *file = fopen(output_dump_file, "a");  
    if (file == NULL) {
        perror("Error opening log file");
        return;
    }

    for (int set_index = 0; set_index < riscv_cache.number_of_sets; set_index++) {
        bool set_has_val_b = false;

        for (int block_index = 0; block_index < riscv_cache.associativity; block_index++) {
            if (riscv_cache.sets[set_index][block_index].valid) {
                set_has_val_b = true;
                
                
                fprintf(file, "Set: 0x%x, Tag: 0x%x, %s\n",set_index,riscv_cache.sets[set_index][block_index].tag,riscv_cache.sets[set_index][block_index].dirty ? "Dirty" : "Clean");
            }
        }
       
    }

    fclose(file);
}

void invalidate_all_entries() {
    for (int set_index = 0; set_index < riscv_cache.number_of_sets; set_index++) {


        for (int block_index = 0; block_index < riscv_cache.associativity; block_index++) {

            if (riscv_cache.sets[set_index][block_index].valid && 
                riscv_cache.sets[set_index][block_index].dirty &&
                strcmp(riscv_cache.write_policy, "WB") == 0) {
                
                uint64_t old_addr = riscv_cache.sets[set_index][block_index].tag * 
                                  riscv_cache.block_size * riscv_cache.number_of_sets + 
                                  set_index * riscv_cache.block_size;
                
                for (int i = 0; i < riscv_cache.block_size; i++) {
                    if (old_addr + i < MEMORY_SIZE) {
                        memory[old_addr + i] = riscv_cache.sets[set_index][block_index].data[i];
                    }
                }
            }
            

            riscv_cache.sets[set_index][block_index].valid = false;
            riscv_cache.sets[set_index][block_index].dirty = false;
        }
    }
}

void reset_values_for_multiple_loads(){
     total_lines = 0;
     line_no = 0;
     line_number = 0;
     data_section_line_count = 0;
     num_of_breakpoints = 0;
    current_line_no = 0;
    just = 0;
    num = 0;
    jal_reg = 0;
    check_file_num = 0;
    PC_1 = 0;
    PC_2 = 0;
    PC_2_s = 0;
    PC_s =0;
    address =0;
    count =0;
    line =0;

    memset(lines, 0, sizeof(lines));
    memset(mem_values_byte, 0, sizeof(mem_values_byte));
    memset(mem_values_half, 0, sizeof(mem_values_half));
    memset(mem_values_word, 0, sizeof(mem_values_word));
    memset(mem_values_dword, 0, sizeof(mem_values_dword));
    memset(register_values, 0, sizeof(register_values));
    memset(memory, 0, sizeof(memory));
}