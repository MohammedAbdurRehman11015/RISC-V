#include "lab7_AI_13_15.h"

int store_flag =0;
int total_lines = 0;
char command[100];
char filename[50];
char filename_d[50];
char config_filename[50];
int address, count, line;

int line_no = 0;
int line_number = 0;

char compartment[32] = "main";
int data_section_line_count = 0;



int breakpoints[5];
int num_of_breakpoints = 0;
int current_line_no = 0;

char r[8][4] = {"add", "sub", "or", "xor", "and", "sll", "srl", "sra"};
char funct3_R[8][4] = {"000", "000", "110", "100", "111", "001", "101", "101"};
char funct7_R[8][8] = {"0000000", "0100000", "0000000", "0000000",
                       "0000000", "0000000", "0000000", "0100000"
                      };
char opcode_R[8][8] = {"0110011", "0110011", "0110011", "0110011",
                       "0110011", "0110011", "0110011", "0110011"
                      };
char registers[32][6] = {
  "x0",  "x1",  "x2",  "x3",  "x4",  "x5",  "x6",  "x7",  "x8",  "x9",  "x10",
  "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18", "x19", "x20", "x21",
  "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31"
};
char register_aliases[32][6] = {
  "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
  "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
  "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
char I[7][5] = {"addi", "xori", "ori", "andi", "slli", "srli", "srai"};
char funct3_I[7][4] = {"000", "100", "110", "111", "001", "101", "101"};
char funct6_I[3][7] = {"000000", "000000", "010000"};
char opcode_I[7][8] = {"0010011", "0010011", "0010011", "0010011",
                       "0010011", "0010011", "0010011"
                      };
char I_S[12][5] = {"lb",  "lh", "lw", "ld", "lbu", "lhu",
                   "lwu", "sb", "sh", "sw", "sd",  "jalr"
                  };
char funct3_I_S[12][4] = {"000", "001", "010", "011", "100", "101",
                          "110", "000", "001", "010", "011", "000"
                         };
char opcode_I_S[12][8] = {"0000011", "0000011", "0000011", "0000011",
                          "0000011", "0000011", "0000011", "0100011",
                          "0100011", "0100011", "0100011", "1100111"
                         };
char U[1][4] = {"lui"};
char opcode_U[1][8] = {"0110111"};

char B[6][5] = {"beq", "bne", "blt", "bge", "bltu", "bgeu"};
char funct3_B[6][4] = {"000", "001", "100", "101", "110", "111"};
char opcode_B[6][8] = {"1100011", "1100011", "1100011",
                       "1100011", "1100011", "1100011"
                      };

char J[1][4] = {"jal"};
char opcode_J[1][8] = {"1101111"};

int64_t register_values[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int PC_1 = 0;
int PC_2 = 0;
int PC_2_s =0;
int PC_s =0;

int8_t memory[MEMORY_SIZE] = {0}; 

Cache riscv_cache = {0};
FILE *output_file = NULL;

int64_t mem_values[MAX_DWORDS] = {0};
int check_file_num =0;

char lines[MAX_LINES][MAX_LINE_LEN];

int just;
int num=0;
int jal_reg =0;

int64_t mem_values_dword[MAX_DWORDS] = {0};   
int64_t mem_values_word[MAX_WORDS] = {0};     
int64_t mem_values_half[MAX_HALFS] = {0};     
uint64_t mem_values_byte[MAX_BYTES] = {0};  

char output_filename[100];
char output_dump_file[100];

extern void reset_values_for_multiple_loads();
    


int main(void) {


    char lines[MAX_LINES][MAX_LINE_LEN];
    stack* stack_1 = createemptystack();
    int flag =0;
    
    

    while (1) {
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = 0;
        
        
        

        
        if (strncmp(command, "load", 4) == 0) {
            flag =1;
            if (riscv_cache.sets != NULL) {  
        
        for(int i = 0; i < riscv_cache.number_of_sets; i++) {
            free(riscv_cache.sets[i]);
        }
        
        free(riscv_cache.sets);
        riscv_cache.sets = NULL;  
    }
            if (sscanf(command, "load %s", filename) == 1) {
                total_lines = count_lines(filename);

                
                int line_count = load_file_into_arrays(filename, lines, mem_values_dword, mem_values_word, mem_values_half, mem_values_byte);

                
                    datasection_lines(filename);
                    char* temp = strdup("main");
                    push_to_stack(stack_1, temp);
                    stack_1->line_num[stack_1->top_index] = 0;

                strncpy(output_filename, filename, sizeof(output_filename)-1);
                output_filename[sizeof(output_filename) - 1] = '\0' ;
                char *last_dot = strrchr(output_filename, '.');
                if (last_dot != NULL) {
                    *last_dot = '\0';  
                }
                
                strcat(output_filename, ".output");

                FILE *file = fopen(output_filename, "a");  
                    if (file == NULL) {
                    perror("Error opening log file");
                    return 0;
                    }
                

            } else {
                printf("Invalid command. Use: load <filename>\n");
            }
        }


        else if (strcmp(command, "run") == 0) {

            handle_instructions_from_lines(lines, total_lines, mem_values_dword, line_no, current_line_no, stack_1);

            if ((line_number == total_lines)) {
                pop_from_stack(stack_1);
            }

            cache_stats();
            
            

        } else if (strcmp(command, "regs") == 0) {
            printf("Registers:\n");

            for (int b = 0; b < 32; b++) {
                printf("%s  = 0x%016llx\n", registers[b], register_values[b]);
            }

        } else if (strncmp(command, "mem", 3) == 0) {

            if (sscanf(command, "mem %x %d", &address, &count) == 2) {

                print_memory(address, count);

            } else {
                printf("Invalid command. Use: mem <address> <count>\n");
            }

        } else if (strcmp(command, "step") == 0) {

            step(lines, total_lines, mem_values, stack_1);

            if ((line_number == total_lines)) {
                pop_from_stack(stack_1);
                cache_stats();
            }

        } else if (strcmp(command, "show-stack") == 0) {

            if (stack_1->top_index != -1) {
                printf("Call Stack:\n");
            }

            for (int i = 0; i <= stack_1->top_index; i++) {
                printf("%s:%d\n", stack_1->value[i], stack_1->line_num[i]);
            }


            if (stack_1->top_index == -1) {
                printf("Empty Call Stack: Execution complete\n");

            }

        } else if (strncmp(command, "break", 5) == 0) {

            if (sscanf(command, "break %d", &line_no) == 1) {

                printf("Setting a breakpoint at line %d\n", line_no);

                set_breakpoints(line_no);
            } else {
                printf("Invalid command. Use: break <line>\n");
            }

        } else if (strncmp(command, "del break", 9) == 0) {

            if (sscanf(command, "del break %d", &line) == 1) {
                printf("Deleting the breakpoint at line %d\n", line);

                delete_breakpoint(line);

            } else {
                printf("Invalid command. Use: del break <line>\n");
            }

        } else if (strncmp(command, "cache_sim enable", 16) == 0) {                     

            if(sscanf(command, "cache_sim enable %s", config_filename) == 1) {

                initialize_cache(config_filename);
                riscv_cache.enabled = true;
            } }
             else if (strncmp(command, "cache_sim disable", 17) == 0) {

            riscv_cache.enabled = false;

        } else if (strncmp(command, "cache_sim status", 16) == 0 ) {

            if (riscv_cache.enabled == true) {
                cache_status();
            } else if(riscv_cache.enabled == false) {
                printf("Cache simulation disabled\n");
            }
            
        } else if (strncmp(command, "cache_sim invalidate", 20) == 0) {
            invalidate_all_entries();

        } 
        else if (sscanf(command, "cache_sim dump %s", filename_d) == 1) {
            strncpy(output_dump_file, filename_d, sizeof(output_dump_file)-1);
            print_valid_cache_sets();

        } 
            else if (strncmp(command, "cache_sim stats", 15) == 0) {

            cache_stats();
            
        } 
         else if (strcmp(command, "exit") == 0) {

            printf("Exited the Simulator\n");
            pop_from_stack(stack_1);
            break;



        } else {
            printf("Invalid command. Please try again.\n");
        }
    }

    return 0;
}

