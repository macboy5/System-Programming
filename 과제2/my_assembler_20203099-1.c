/**
 * @file my_assembler_00000000.c
 * @date 2026-06-21
 * @version 0.1.0
 *
 * @brief SIC/XE 소스코드를 object code로 변환하는 프로그램
 *
 * @details
 * SIC/XE 소스코드를 해당 머신에서 동작하도록 object code로 변환하는
 * 프로그램이다. 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을
 * 기입한다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* 파일명의 "00000000"은 자신의 학번으로 변경할 것 */
#include "my_assembler_20203099-1.h"

/**
 * @brief 사용자로부터 SIC/XE 소스코드를 받아서 object code를 출력한다.
 *
 * @details
 * 사용자로부터 SIC/XE 소스코드를 받아서 object code를 출력한다. 특별한 사유가
 * 없는 한 변경하지 말 것.
 */
int main(int argc, char **argv) {
    /** SIC/XE 머신의 instruction 정보를 저장하는 테이블 */
    inst *inst_table[MAX_INST_TABLE_LENGTH];
    int inst_table_length;

    /** SIC/XE 소스코드를 저장하는 테이블 */
    char *input[MAX_INPUT_LINES];
    int input_length;

    /** 소스코드의 각 라인을 토큰 전환하여 저장하는 테이블 */
    token *tokens[MAX_INPUT_LINES];
    int tokens_length;

    /** 소스코드 내의 심볼을 저장하는 테이블 */
    symbol *symbol_table[MAX_TABLE_LENGTH];
    int symbol_table_length;

    /** 소스코드 내의 리터럴을 저장하는 테이블 */
    literal *literal_table[MAX_TABLE_LENGTH];
    int literal_table_length;

    /** 오브젝트 코드를 저장하는 변수 */
    object_code *obj_code = (object_code *)malloc(sizeof(object_code));

    int err = 0;

    if ((err = init_inst_table(inst_table, &inst_table_length,
                               "inst_table.txt")) < 0) {
        fprintf(stderr,
                "init_inst_table: 기계어 목록 초기화에 실패했습니다. "
                "(error_code: %d)\n",
                err);
        return -1;
    }

    if ((err = init_input(input, &input_length, "input.txt")) < 0) {
        fprintf(stderr,
                "init_input: 소스코드 입력에 실패했습니다. (error_code: %d)\n",
                err);
        return -1;
    }

    if ((err = assem_pass1((const inst **)inst_table, inst_table_length,
                           (const char **)input, input_length, tokens,
                           &tokens_length, symbol_table, &symbol_table_length,
                           literal_table, &literal_table_length)) < 0) {
        fprintf(stderr,
                "assem_pass1: 패스1 과정에서 실패했습니다. (error_code: %d)\n",
                err);
        return -1;
    }

    /** 프로젝트1에서는 불필요함 */
    /*
    if ((err = make_opcode_output("output_opcode.txt", (const token **)tokens,
                                  tokens_length, (const inst **)inst_table,
                                  inst_table_length)) < 0) {
        fprintf(stderr,
                "make_opcode_output: opcode 파일 출력 과정에서 실패했습니다. "
                "(error_code: %d)\n",
                err);
        return -1;
    }
    */

    if ((err = make_symbol_table_output("output_symtab.txt",
                                        (const symbol **)symbol_table,
                                        symbol_table_length)) < 0) {
        fprintf(stderr,
                "make_symbol_table_output: 심볼테이블 파일 출력 과정에서 "
                "실패했습니다. (error_code: %d)\n",
                err);
        return -1;
    }

    if ((err = make_literal_table_output("output_littab.txt",
                                         (const literal **)literal_table,
                                         literal_table_length)) < 0) {
        fprintf(stderr,
                "make_literal_table_output: 리터럴테이블 파일 출력 과정에서 "
                "실패했습니다. (error_code: %d)\n",
                err);
        return -1;
    }

    if ((err = assem_pass2((const token **)tokens, tokens_length,
                           (const inst **)inst_table, inst_table_length,
                           (const symbol **)symbol_table, symbol_table_length,
                           (const literal **)literal_table,
                           literal_table_length, obj_code)) < 0) {
        fprintf(stderr,
                "assem_pass2: 패스2 과정에서 실패했습니다. (error_code: %d)\n",
                err);
        return -1;
    }

    if ((err = make_objectcode_output("output_objectcode.txt",
                                      (const object_code *)obj_code)) < 0) {
        fprintf(stderr,
                "make_objectcode_output: 오브젝트코드 파일 출력 과정에서 "
                "실패했습니다. (error_code: %d)\n",
                err);
        return -1;
    }

    return 0;
}

/**
 * @brief 기계어 목록 파일(inst_table.txt)을 읽어 기계어 목록
 * 테이블(inst_table)을 생성한다.
 *
 * @param inst_table 기계어 목록 테이블의 시작 주소
 * @param inst_table_length 기계어 목록 테이블의 길이를 저장하는 변수 주소
 * @param inst_table_dir 기계어 목록 파일 경로
 * @return 오류 코드 (정상 종료 = 0)
 *
 * @details
 * 기계어 목록 파일(inst_table.txt)을 읽어 기계어 목록 테이블(inst_table)을
 * 생성한다. 기계어 목록 파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *    ==============================================================
 *           | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | \n |
 *    ==============================================================
 */
int init_inst_table(inst **inst_table, int *inst_table_length,
                    const char *inst_table_dir) {
    FILE *fp;
    int err = -1;
    fp = fopen(inst_table_dir, "r");
    if(fp == NULL) {
        printf("file open failure : %s\n", inst_table_dir);
        return err;
    }

    printf("------------- init_inst_table -------------\n");

    char line[100];
    int i = 0;

    while(fgets(line, sizeof(line), fp) != NULL){
        line[strcspn(line, "\n")] = '\0';
        if(line[0] == '\0') break;

        inst_table[i] = (inst *) malloc(sizeof(inst));

        char op_hex[16];
        sscanf(line, "%9[^,],%d,%15[^,],%d",
               inst_table[i]->str,
               &inst_table[i]->format,
               op_hex,
               &inst_table[i]->ops);
        inst_table[i]->op = (unsigned char)strtol(op_hex, NULL, 16);

        printf("%s %02X %d %d\n", inst_table[i]->str, inst_table[i]->op, inst_table[i]->format, inst_table[i]->ops);

        i++;


    }

    *inst_table_length = i;

    printf("inst table length : %d\n\n", *inst_table_length);

    err = 0;

    fclose(fp);

    return err;
}

/**
 * @brief SIC/XE 소스코드 파일(input.txt)을 읽어 소스코드 테이블(input)을
 * 생성한다.
 *
 * @param input 소스코드 테이블의 시작 주소
 * @param input_length 소스코드 테이블의 길이를 저장하는 변수 주소
 * @param input_dir 소스코드 파일 경로
 * @return 오류 코드 (정상 종료 = 0)
 */
int init_input(char **input, int *input_length, const char *input_dir) {
    FILE *fp;
    int err = -1;
    fp = fopen(input_dir, "r");
    if(fp == NULL) {
        printf("file open failure : %s\n", input_dir);
        return err;
    }

    printf("------------- init_input -------------\n");

    char line[100];
    int i = 0;

    while(fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if(line[0] == '\0') break;

        input[i] = (char *)malloc(strlen(line) + 1);
        strcpy(input[i], line);

        printf("%s\n", line);

        i++;
    }

    *input_length = i;
    printf("input length : %d\n\n", *input_length);
    err = 0;

    fclose(fp);

    return err;
}

/**
 * @brief 어셈블리 코드을 위한 패스 1 과정을 수행한다.
 *
 * @param inst_table 기계어 목록 테이블의 주소
 * @param inst_table_length 기계어 목록 테이블의 길이
 * @param input 소스코드 테이블의 주소
 * @param input_length 소스코드 테이블의 길이
 * @param tokens 토큰 테이블의 시작 주소
 * @param tokens_length 토큰 테이블의 길이를 저장하는 변수 주소
 * @param symbol_table 심볼 테이블의 시작 주소0
 * @param symbol_table_length 심볼 테이블의 길이를 저장하는 변수 주소
 * @param literal_table 리터럴 테이블의 시작 주소
 * @param literal_table_length 리터럴 테이블의 길이를 저장하는 변수 주소
 * @return 오류 코드 (정상 종료 = 0)
 *
 * @details
 * 어셈블리 코드를 위한 패스1 과정을 수행하는 함수이다. 패스 1에서는 프로그램
 * 소스를 스캔하여 해당하는 토큰 단위로 분리하여 프로그램 라인별 토큰 테이블을
 * 생성한다. 토큰 테이블은 token_parsing 함수를 호출하여 설정하여야 한다. 또한,
 * assem_pass2 과정에서 사용하기 위한 심볼 테이블 및 리터럴 테이블을 생성한다.
 */
int assem_pass1(const inst **inst_table, int inst_table_length,
                const char **input, int input_length, token **tokens,
                int *tokens_length, symbol **symbol_table,
                int *symbol_table_length, literal **literal_table,
                int *literal_table_length) {

    int err = 0;
    *symbol_table_length = 0;
    *literal_table_length = 0;

    int locctr = 0;               // 현재 주소
    char cur_section[10] = "";    // 현재 섹션명

    for(int i = 0; i < input_length; i++) {
        tokens[i] = (token *)malloc(sizeof(token));

        if((err = token_parsing(input[i], tokens[i], inst_table, inst_table_length)) < 0) {
            fprintf(stderr, "assem_pass1 failure. (error_code: %d)\n", err);
            return -1;
        }

        char *op = tokens[i]->operator;
        char *label = tokens[i]->label;

        // START: 시작 주소 설정
        if(op != NULL && strcmp(op, "START") == 0) {
            locctr = (int)strtol(tokens[i]->operand[0], NULL, 16);
            if(label != NULL) {
                strcpy(cur_section, label);
                symbol_table[*symbol_table_length] = (symbol *)malloc(sizeof(symbol));
                strcpy(symbol_table[*symbol_table_length]->name, label);
                symbol_table[*symbol_table_length]->addr = locctr;
                symbol_table[*symbol_table_length]->section[0] = '\0';  // 섹션명 없음
                (*symbol_table_length)++;
            }
            continue;
        }

        // CSECT: 새 섹션 시작
        if(op != NULL && strcmp(op, "CSECT") == 0) {
            locctr = 0;
            if(label != NULL) {
                strcpy(cur_section, label);
                symbol_table[*symbol_table_length] = (symbol *)malloc(sizeof(symbol));
                strcpy(symbol_table[*symbol_table_length]->name, label);
                symbol_table[*symbol_table_length]->addr = locctr;
                symbol_table[*symbol_table_length]->section[0] = '\0';  // 섹션명 없음
                (*symbol_table_length)++;
            }
            continue;
        }

        // label이 있으면 심볼 테이블에 추가
        // label이 있으면 심볼 테이블에 추가
        if(label != NULL) {
            int addr = locctr;

            // EQU 처리
            if(op != NULL && strcmp(op, "EQU") == 0) {
                if(strcmp(tokens[i]->operand[0], "*") == 0) {
                    addr = locctr;  // EQU * → 현재 주소
                } else {
                    // EQU BUFEND-BUFFER → 두 심볼 차이 계산
                    char *operand = tokens[i]->operand[0];
                    char *minus = strchr(operand, '-');
                    if(minus != NULL) {
                        char sym1[10], sym2[10];
                        strncpy(sym1, operand, minus - operand);
                        sym1[minus - operand] = '\0';
                        strcpy(sym2, minus + 1);

                        int addr1 = 0, addr2 = 0;
                        for(int k = 0; k < *symbol_table_length; k++) {
                            if(strcmp(symbol_table[k]->name, sym1) == 0) addr1 = symbol_table[k]->addr;
                            if(strcmp(symbol_table[k]->name, sym2) == 0) addr2 = symbol_table[k]->addr;
                        }
                        addr = addr1 - addr2;
                    }
                }
            }

            symbol_table[*symbol_table_length] = (symbol *)malloc(sizeof(symbol));
            strcpy(symbol_table[*symbol_table_length]->name, label);
            symbol_table[*symbol_table_length]->addr = addr;
            strcpy(symbol_table[*symbol_table_length]->section, cur_section);
            (*symbol_table_length)++;
        }

        // 리터럴 테이블
        for(int j = 0; j < MAX_OPERAND_PER_INST; j++) {
            if(tokens[i]->operand[j] == NULL) break;
            if(tokens[i]->operand[j][0] == '=') {
                // 중복 체크
                int dup = 0;
                for(int k = 0; k < *literal_table_length; k++) {
                    if(strcmp(literal_table[k]->literal, tokens[i]->operand[j]) == 0) {
                        dup = 1;
                        break;
                    }
                }
                if(!dup) {
                    literal_table[*literal_table_length] = (literal *)malloc(sizeof(literal));
                    strcpy(literal_table[*literal_table_length]->literal, tokens[i]->operand[j]);
                    literal_table[*literal_table_length]->addr = 0;
                    (*literal_table_length)++;
                }
            }
        }

        if(op != NULL && strcmp(op, "LTORG") == 0) {
        int ltorg_size = 0;
        for(int k = 0; k < *literal_table_length; k++) {
            if(literal_table[k]->addr == 0) {
                char *lit = literal_table[k]->literal;
                literal_table[k]->addr = locctr + ltorg_size;  // 현재 locctr 기준
                if(lit[1] == 'C') ltorg_size += strlen(lit) - 4;
                else if(lit[1] == 'X') ltorg_size += (strlen(lit) - 4) / 2;
            }
        }
        locctr += ltorg_size;
        continue;  // get_inst_size 호출 건너뜀
    }

            // END 처리 추가
        if(op != NULL && strcmp(op, "END") == 0) {
            int ltorg_size = 0;
            for(int k = 0; k < *literal_table_length; k++) {
                if(literal_table[k]->addr == 0) {
                    char *lit = literal_table[k]->literal;
                    literal_table[k]->addr = locctr + ltorg_size;
                    if(lit[1] == 'C') ltorg_size += strlen(lit) - 4;
                    else if(lit[1] == 'X') ltorg_size += (strlen(lit) - 4) / 2;
                }
            }
            locctr += ltorg_size;
            continue;
        }

        // 주소 증가
        locctr += get_inst_size(tokens[i], inst_table, inst_table_length,
                        literal_table, literal_table_length, locctr);
    }

    *tokens_length = input_length;
    return 0;
}

// 명령어 하나의 바이트 크기를 반환
int get_inst_size(const token *tok, const inst **inst_table, int inst_table_length,
                  literal **literal_table, int *literal_table_length, int locctr) {
    if(tok->operator == NULL) return 0;

    char *op = tok->operator;

    if(op[0] == '+') return 4;

    if(strcmp(op, "RESW") == 0) return 3 * atoi(tok->operand[0]);
    if(strcmp(op, "RESB") == 0) return atoi(tok->operand[0]);
    if(strcmp(op, "WORD") == 0) return 3;

    if(strcmp(op, "BYTE") == 0) {
        char *operand = tok->operand[0];
        if(operand[0] == 'X') return (strlen(operand) - 3) / 2;
        if(operand[0] == 'C') return strlen(operand) - 3;
        return 0;
    }

    if(strcmp(op, "START") == 0 || strcmp(op, "END") == 0 ||
       strcmp(op, "CSECT") == 0 || strcmp(op, "EXTDEF") == 0 ||
       strcmp(op, "EXTREF") == 0 || strcmp(op, "EQU") == 0 ||
       strcmp(op, "LTORG") == 0) {
        return 0; 
    }

    int idx = search_opcode(op, inst_table, inst_table_length);
    if(idx >= 0) return inst_table[idx]->format;

    return 0;
}
/**
 * @brief 한 줄의 소스코드를 파싱하여 토큰에 저장한다.
 *
 * @param input 파싱할 소스코드 문자열
 * @param tok 결과를 저장할 토큰 구조체 주소
 * @param inst_table 기계어 목록 테이블의 주소
 * @param inst_table_length 기계어 목록 테이블의 길이
 * @return 오류 코드 (정상 종료 = 0)
 */

/* [start, end) 범위의 문자열을 복사하여 반환하는 헬퍼 함수 */
static char *substr(const char *start, const char *end) {
    size_t len = (size_t)(end - start);
    char *s = (char *)malloc(len + 1);
    memcpy(s, start, len);
    s[len] = '\0';
    return s;
}

int token_parsing(const char *input, token *tok, const inst *inst_table[],int inst_table_length) {
    tok->label = NULL;
    tok->operator = NULL;
    tok->comment = NULL;
    tok->nixbpe = 0;

    for (int i = 0; i < MAX_OPERAND_PER_INST; i++)
        tok->operand[i] = NULL;

    // '.'으로 시작하면 전체가 comment
    if (input[0] == '.') {
        tok->comment = substr(input, input + strlen(input));
        return 0;
    }

    const char *p = input;
    const char *tab;

    // label: 첫 글자가 '\t'면 label 없음
    if (p[0] != '\t') {
        tab = strchr(p, '\t');
        if (tab) {
            tok->label = substr(p, tab);
            p = tab + 1;
        } else {
            tok->label = substr(p, p + strlen(p));
            return 0;
        }
    }
    // leading tab이 여러 개이거나 label 뒤 extra tab 건너뜀
    while (*p == '\t') p++;
    if (*p == '\0') return 0;

    // operator
    tab = strchr(p, '\t');
    if (tab) {
        tok->operator = substr(p, tab);
        p = tab + 1;
        while (*p == '\t') p++;  // operator 뒤 연속 탭 건너뜀
    } else {
        tok->operator = substr(p, p + strlen(p));
        return 0;
    }

    if (*p == '\0') return 0;

    // operand: 다음 탭까지를 ','로 분리
    tab = strchr(p, '\t');
    const char *operand_end = tab ? tab : p + strlen(p);

    int i = 0;
    while (p < operand_end && i < MAX_OPERAND_PER_INST) {
        const char *comma = (const char *)memchr(p, ',', (size_t)(operand_end - p));
        const char *field_end = comma ? comma : operand_end;
        tok->operand[i++] = substr(p, field_end);
        p = comma ? comma + 1 : operand_end;
    }

    // comment: 연속 탭 건너뛰고 나머지
    if (tab) {
        p = tab + 1;
        while (*p == '\t') p++;
        if (*p != '\0')
            tok->comment = substr(p, p + strlen(p));
    }

    return 0;
}


/**
 * @brief 기계어 목록 테이블에서 특정 기계어를 검색하여, 해당 기계에가 위치한
 * 인덱스를 반환한다.
 *
 * @param str 검색할 기계어 문자열
 * @param inst_table 기계어 목록 테이블 주소
 * @param inst_table_length 기계어 목록 테이블의 길이
 * @return 기계어의 인덱스 (해당 기계어가 없는 경우 -1)
 *
 * @details
 * 기계어 목록 테이블에서 특정 기계어를 검색하여, 해당 기계에가 위치한 인덱스를
 * 반환한다. '+JSUB'와 같은 문자열에 대한 처리는 자유롭게 처리한다.
 */
int search_opcode(const char *str, const inst **inst_table,
                  int inst_table_length) {

    char *copy = (char*)malloc(strlen(str)+1);
    strcpy(copy, str);

    if(copy[0] == '+'){
        memmove(copy, copy+1, strlen(copy));
    }

    printf("search_opcode: %s\n", copy);  // 검색어 확인

    for(int i =0; i< inst_table_length; i++){

        if(strcmp(inst_table[i]->str, copy) ==0){

            free(copy);
            return i;
        }
    }

    free(copy);

    return -1;
}

/**
 * @brief 소스코드 명령어 앞에 OPCODE가 기록된 코드를 파일에 출력한다.
 * `output_dir`이 NULL인 경우 결과를 stdout으로 출력한다. 프로젝트 1에서는
 * 불필요하다.
 *
 * @param output_dir 코드를 저장할 파일 경로, 혹은 NULL
 * @param tokens 토큰 테이블 주소
 * @param tokens_length 토큰 테이블의 길이
 * @param inst_table 기계어 목록 테이블 주소
 * @param inst_table_length 기계어 목록 테이블의 길이
 * @return 오류 코드 (정상 종료 = 0)
 *
 * @details
 * 소스코드 명령어 앞에 OPCODE가 기록된 코드를 파일에 출력한다. `output_dir`이
 * NULL인 경우 결과를 stdout으로 출력한다. 명세서에 주어진 출력 예시와 완전히
 * 동일할 필요는 없다. 프로젝트 1에서는 불필요하다.
 */
int make_opcode_output(const char *output_dir, const token **tokens,
                       int tokens_length, const inst **inst_table,
                       int inst_table_length) {
    FILE *fp;
    int err = -1;

    printf("--------------- make_opcode_output ---------------\n");

    printf("tokens_length: %d\n", tokens_length);  // 토큰 길이 확인

    if(output_dir == NULL) {
        fp = stdout;
        printf("출력: stdout\n");
    }
    else{
        printf("출력 파일: %s\n", output_dir);
        fp = fopen(output_dir, "w+");
        if(fp == NULL) {
            printf("output file create failure\n");
            return err;
        }
    }


    for(int i=0; i< tokens_length; i++){
        const token *tok = tokens[i];

        printf("tok[%d]: label=%s, operator=%s\n", i,  // 각 토큰 확인
               tok->label    ? tok->label    : "NULL",
               tok->operator ? tok->operator : "NULL");

        int idx = -1;
        if(tok->operator != NULL){
            idx = search_opcode(tok->operator, inst_table, inst_table_length);
        }

        // label 출력
        fprintf(fp, "%s\t", tok->label ? tok->label : "");

        // operator 출력
        fprintf(fp, "%s\t", tok->operator ? tok->operator : "");

        // operand 출력
        int op_printed = 0;
        for(int j=0; j < MAX_OPERAND_PER_INST; j++){
            if(tok->operand[j] == NULL) break;
            if(op_printed) fprintf(fp, ",");
            fprintf(fp, "%s", tok->operand[j]);
            op_printed = 1;
        }
        fprintf(fp, "\t");

        // comment 출력
        // fprintf(fp, "%s\t", tok->comment ? tok->comment : "");

        // opcode 출력
        if(idx >= 0) {
            fprintf(fp, "%02X", inst_table[idx]->op);
        }
        fprintf(fp, "\n");


    }

    if(output_dir != NULL) fclose(fp);
    err = 0;

    return err;
}

/**
 * @brief 어셈블리 코드을 위한 패스 2 과정을 수행한다.
 *
 * @param tokens 토큰 테이블 주소
 * @param tokens_length 토큰 테이블 길이
 * @param inst_table 기계어 목록 테이블 주소
 * @param inst_table_length 기계어 목록 테이블 길이
 * @param symbol_table 심볼 테이블 주소
 * @param symbol_table_length 심볼 테이블 길이
 * @param literal_table 리터럴 테이블 주소
 * @param literal_table_length 리터럴 테이블 길이
 * @param obj_code 오브젝트 코드에 대한 정보를 저장하는 구조체 주소
 * @return 오류 코드 (정상 종료 = 0)
 *
 * @details
 * 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행한다. 패스 2의
 * 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
 */
int assem_pass2(const token *tokens[], int tokens_length,
                const inst *inst_table[], int inst_table_length,
                const symbol *symbol_table[], int symbol_table_length,
                const literal *literal_table[], int literal_table_length,
                object_code *obj_code) {

    obj_code->section_count = 0;
    int cur_sec = -1;
    int locctr = 0;
    int new_record_needed = 0; 

    for(int i = 0; i < tokens_length; i++) {
        const token *tok = tokens[i];
        char *op = tok->operator;  // operator → operatr
        char *label = tok->label;

        if(op == NULL) continue;

        // ── START ──────────────────────────────
        if(strcmp(op, "START") == 0) {
            cur_sec = obj_code->section_count++;
            section_code *sec = &obj_code->sections[cur_sec];

            strcpy(sec->section_name, label);
            sec->start_addr = (int)strtol(tok->operand[0], NULL, 16);
            sec->section_length = 0;
            sec->def_count = 0;
            sec->ref_count = 0;
            sec->text_record_count = 0;
            sec->mod_record_count = 0;
            sec->has_start = 1;
            sec->exec_addr = sec->start_addr;

            locctr = sec->start_addr;
            continue;
        }

        // ── CSECT ──────────────────────────────
        if(strcmp(op, "CSECT") == 0) {
            if(cur_sec >= 0) {
                obj_code->sections[cur_sec].section_length = locctr;
            }

            cur_sec = obj_code->section_count++;
            section_code *sec = &obj_code->sections[cur_sec];

            strcpy(sec->section_name, label);
            sec->start_addr = 0;
            sec->section_length = 0;
            sec->def_count = 0;
            sec->ref_count = 0;
            sec->text_record_count = 0;
            sec->mod_record_count = 0;
            sec->has_start = 0;
            sec->exec_addr = 0;

            locctr = 0;
            continue;
        }

        section_code *sec = &obj_code->sections[cur_sec];

        // ── EXTDEF ─────────────────────────────
        if(strcmp(op, "EXTDEF") == 0) {
            for(int j = 0; j < MAX_OPERAND_PER_INST; j++) {
                if(tok->operand[j] == NULL) break;
                strcpy(sec->def_symbols[sec->def_count], tok->operand[j]);
                for(int k = 0; k < symbol_table_length; k++) {
                    if(strcmp(symbol_table[k]->name, tok->operand[j]) == 0) {
                        sec->def_addrs[sec->def_count] = symbol_table[k]->addr;
                        break;
                    }
                }
                sec->def_count++;
            }
            continue;
        }

        // ── EXTREF ─────────────────────────────
        if(strcmp(op, "EXTREF") == 0) {
            for(int j = 0; j < MAX_OPERAND_PER_INST; j++) {
                if(tok->operand[j] == NULL) break;
                strcpy(sec->ref_symbols[sec->ref_count++], tok->operand[j]);
            }
            continue;
        }

        // ── LTORG ──────────────────────────────
        if(strcmp(op, "LTORG") == 0) {
            for(int k = 0; k < literal_table_length; k++) {
                if(literal_table[k]->addr == locctr) {
                    char lit_code[20] = "";
                    int lit_len = 0;
                    char *lit = (char *)literal_table[k]->literal;
                    if(lit[1] == 'C') {
                        int len = strlen(lit) - 4;
                        for(int m = 0; m < len; m++)
                            sprintf(lit_code + m*2, "%02X", (unsigned char)lit[3+m]);
                        lit_len = len;
                    } else if(lit[1] == 'X') {
                        int len = strlen(lit) - 4;
                        strncpy(lit_code, lit + 3, len);
                        lit_code[len] = '\0';
                        lit_len = len / 2;
                    }
                    if(lit_len > 0) {
                        add_text_record(sec, locctr, lit_code, lit_len, new_record_needed);
                        new_record_needed = 0;
                        locctr += lit_len;
                    }
                }
            }
            continue;
        }

        // ── END ────────────────────────────────
        if(strcmp(op, "END") == 0) {
            for(int k = 0; k < literal_table_length; k++) {
                if(literal_table[k]->addr >= locctr) {
                    char lit_code[20] = "";
                    int lit_len = 0;
                    char *lit = (char *)literal_table[k]->literal;
                    if(lit[1] == 'C') {
                        int len = strlen(lit) - 4;
                        for(int m = 0; m < len; m++)
                            sprintf(lit_code + m*2, "%02X", (unsigned char)lit[3+m]);
                        lit_len = len;
                    } else if(lit[1] == 'X') {
                        int len = strlen(lit) - 4;
                        strncpy(lit_code, lit + 3, len);
                        lit_code[len] = '\0';
                        lit_len = len / 2;
                    }
                    if(lit_len > 0) {
                        add_text_record(sec, locctr, lit_code, lit_len, new_record_needed);
                        new_record_needed = 0;
                        locctr += lit_len;
                    }
                }
            }
            sec->section_length = locctr;
            continue;
        }


        // ── RESW / RESB ────────────────────────

        if(strcmp(op, "RESW") == 0) {
            locctr += 3 * atoi(tok->operand[0]);
            new_record_needed = 1;
            continue;
        }
        if(strcmp(op, "RESB") == 0) {
            locctr += atoi(tok->operand[0]);
            new_record_needed = 1;
            continue;
        }

        // ── EQU ────────────────────────────────
        if(strcmp(op, "EQU") == 0) continue;

        // ── 기계어 코드 생성 ───────────────────
        char machine_code[20] = "";
        int code_len = generate_code(tok, inst_table, inst_table_length,
                                     symbol_table, symbol_table_length,
                                     literal_table, literal_table_length,
                                     sec, locctr, machine_code);

        if(code_len > 0) {
            add_text_record(sec, locctr, machine_code, code_len, new_record_needed);
            new_record_needed = 0;  // 플래그 리셋
        }

        locctr += code_len;
    }

    return 0;
}


int get_register(const char *reg) {
    if(strcmp(reg, "A") == 0)  return 0;
    if(strcmp(reg, "X") == 0)  return 1;
    if(strcmp(reg, "L") == 0)  return 2;
    if(strcmp(reg, "B") == 0)  return 3;
    if(strcmp(reg, "S") == 0)  return 4;
    if(strcmp(reg, "T") == 0)  return 5;
    if(strcmp(reg, "F") == 0)  return 6;
    if(strcmp(reg, "PC") == 0) return 8;
    if(strcmp(reg, "SW") == 0) return 9;
    return 0;
}

void add_text_record(section_code *sec, int addr, const char *code, int code_len, int force_new) {
    if(sec->text_record_count == 0 ||
       sec->text_records[sec->text_record_count - 1].length + code_len > 30 ||
       force_new) {
        text_record *tr = &sec->text_records[sec->text_record_count++];
        tr->start_addr = addr;
        tr->length = 0;
        tr->code[0] = '\0';
    }

    text_record *tr = &sec->text_records[sec->text_record_count - 1];
    strcat(tr->code, code);
    tr->length += code_len;
}

int generate_code(const token *tok, const inst **inst_table, int inst_table_length,
                  const symbol **symbol_table, int symbol_table_length,
                  const literal **literal_table, int literal_table_length,
                  section_code *sec, int locctr, char *machine_code) {

    char *op = tok->operator;
    if(op == NULL) return 0;

    // ── WORD ───────────────────────────────────
    if(strcmp(op, "WORD") == 0) {
        int val = atoi(tok->operand[0]);
        sprintf(machine_code, "%06X", val);
        return 3;
    }

    // ── BYTE ───────────────────────────────────
    if(strcmp(op, "BYTE") == 0) {
        char *operand = tok->operand[0];
        if(operand[0] == 'X') {
            // X'F1' → F1
            int len = strlen(operand) - 3;
            strncpy(machine_code, operand + 2, len);
            machine_code[len] = '\0';
            return len / 2;
        } else if(operand[0] == 'C') {
            // C'EOF' → 454F46
            int len = strlen(operand) - 3;
            for(int i = 0; i < len; i++)
                sprintf(machine_code + i*2, "%02X", (unsigned char)operand[2+i]);
            machine_code[len*2] = '\0';
            return len;
        }
        return 0;
    }

    // ── 나머지 지시어는 코드 없음 ──────────────
    if(strcmp(op, "RESW") == 0 || strcmp(op, "RESB") == 0 ||
       strcmp(op, "EQU") == 0  || strcmp(op, "LTORG") == 0 ||
       strcmp(op, "EXTDEF") == 0 || strcmp(op, "EXTREF") == 0 ||
       strcmp(op, "CSECT") == 0 || strcmp(op, "START") == 0 ||
       strcmp(op, "END") == 0) {
        return 0;
    }

    // ── 기계어 명령어 ──────────────────────────
    int idx = search_opcode(op, inst_table, inst_table_length);
    if(idx < 0) return 0;

    int format = inst_table[idx]->format;
    if(op[0] == '+') format = 4;

    unsigned char opcode = inst_table[idx]->op;

    // ── Format 1 ───────────────────────────────
    if(format == 1) {
        sprintf(machine_code, "%02X", opcode);
        return 1;
    }

    // ── Format 2 ───────────────────────────────
    if(format == 2) {
        int r1 = tok->operand[0] ? get_register(tok->operand[0]) : 0;
        int r2 = tok->operand[1] ? get_register(tok->operand[1]) : 0;
        sprintf(machine_code, "%02X%X%X", opcode, r1, r2);
        return 2;
    }

    // ── Format 3 / 4 ───────────────────────────
    if(format == 3 || format == 4) {
        int n = 1, ix = 1, x = 0, b = 0, p = 0, e = 0;

        char *operand = tok->operand[0];

        // operand 없는 경우 (RSUB)
        if(operand == NULL) {
            unsigned int code = ((opcode | (n<<1|ix)) << 16) |
                                ((x<<7|b<<6|p<<5|e<<4) << 8) | 0;
            sprintf(machine_code, "%06X", code);
            return 3;
        }

        if(format == 4) e = 1;

        // 주소지정 모드 분석
        if(operand[0] == '#') {
            n = 0; ix = 1;  // 즉시 주소지정
            operand++;
        } else if(operand[0] == '@') {
            n = 1; ix = 0;  // 간접 주소지정
            operand++;
        }

        // 인덱스 주소지정 (,X)
        char op_name[20];
        strcpy(op_name, operand);
        char *comma = strchr(op_name, ',');
        if(comma && *(comma+1) == 'X') {
            x = 1;
            *comma = '\0';
        }

        // 주소 계산
        int target_addr = 0;
        int is_external = 0;
        int is_immediate_number = 0;

        if(isdigit(op_name[0])) {
            // 숫자 즉시값
            target_addr = atoi(op_name);
            is_immediate_number = 1;
        } else {
            // 심볼 테이블에서 찾기
            int found = 0;
            for(int k = 0; k < symbol_table_length; k++) {
                if(strcmp(symbol_table[k]->name, op_name) == 0) {
                    target_addr = symbol_table[k]->addr;
                    found = 1;
                    break;
                }
            }
            // 리터럴 테이블에서 찾기
            if(!found) {
                for(int k = 0; k < literal_table_length; k++) {
                    // =C'EOF' 에서 C'EOF' 부분 비교
                    if(strcmp(literal_table[k]->literal, tok->operand[0]) == 0) {
                        target_addr = literal_table[k]->addr;
                        found = 1;
                        break;
                    }
                }
            }
            if(!found) {
                // 외부 참조 심볼
                is_external = 1;
                target_addr = 0;
            }
        }

        int disp = 0;

        if(format == 4) {
            disp = target_addr;
            if(is_external || (!is_immediate_number && target_addr == 0)) {
                modification_record *mr = &sec->mod_records[sec->mod_record_count++];
                mr->addr = locctr + 1;  // opcode 다음 바이트
                mr->length = 5;         // 20비트 = 5 half-byte
                mr->sign = '+';
                strcpy(mr->symbol, op_name);
            }
        } else {
            if(is_immediate_number) {
                // 즉시 숫자값: PC/Base 상대 없이 그대로
                disp = target_addr & 0xFFF;
            } else {
                // PC 상대
                int pc = locctr + 3;
                int pc_disp = target_addr - pc;
                if(pc_disp >= -2048 && pc_disp <= 2047) {
                    p = 1;
                    disp = pc_disp & 0xFFF;
                } else {
                    // Base 상대 (현재는 단순 처리)
                    b = 1;
                    disp = target_addr & 0xFFF;
                }
            }
        }

        // 코드 생성
        if(format == 3) {
            unsigned int code = ((opcode | (n<<1|ix)) << 16) |
                                ((x<<7|b<<6|p<<5|e<<4) << 8) |
                                (disp & 0xFFF);
            sprintf(machine_code, "%06X", code);
            return 3;
        } else {
            unsigned int code = ((opcode | (n<<1|ix)) << 24) |
                                ((x<<7|b<<6|p<<5|e<<4) << 16) |
                                (disp & 0xFFFFF);
            sprintf(machine_code, "%08X", code);
            return 4;
        }
    }

    return 0;
}


/**
 * @brief 심볼 테이블을 파일로 출력한다. `symbol_table_dir`이 NULL인 경우 결과를
 * stdout으로 출력한다.
 *
 * @param symbol_table_dir 심볼 테이블을 저장할 파일 경로, 혹은 NULL
 * @param symbol_table 심볼 테이블 주소
 * @param symbol_table_length 심볼 테이블 길이
 * @return 오류 코드 (정상 종료 = 0)
 *
 * @details
 * 심볼 테이블을 파일로 출력한다. `symbol_table_dir`이 NULL인 경우 결과를
 * stdout으로 출력한다. 명세서에 주어진 출력 예시와 완전히 동일할 필요는 없다.
 */
int make_symbol_table_output(const char *symbol_table_dir,
                             const symbol *symbol_table[],
                             int symbol_table_length) {
    FILE *fp;
    int err = -1;

    if(symbol_table_dir == NULL) {
        fp = stdout;
    } else {
        fp = fopen(symbol_table_dir, "w+");
        if(fp == NULL) {
            printf("symbol table output file create failure\n");
            return err;
        }
    }

    for(int i = 0; i < symbol_table_length; i++) {
        if(symbol_table[i]->section[0] == '\0') {
            // CSECT or START: 섹션명 없이 출력
            fprintf(fp, "\t%-10s %X\n",
                    symbol_table[i]->name,
                    symbol_table[i]->addr);
        } else {
            // 일반 심볼: +1 섹션명 출력
            fprintf(fp, "\t%-10s %-10X +1 %s\n",
                    symbol_table[i]->name,
                    symbol_table[i]->addr,
                    symbol_table[i]->section);
        }
    }

    if(symbol_table_dir != NULL) fclose(fp);
    err = 0;

    return err;
}
/**
 * @brief 리터럴 테이블을 파일로 출력한다. `literal_table_dir`이 NULL인 경우
 * 결과를 stdout으로 출력한다.
 *
 * @param literal_table_dir 리터럴 테이블을 저장할 파일 경로, 혹은 NULL
 * @param literal_table 리터럴 테이블 주소
 * @param literal_table_length 리터럴 테이블 길이
 * @return 오류 코드 (정상 종료 = 0)
 *
 * @details
 * 리터럴 테이블을 파일로 출력한다. `literal_table_dir`이 NULL인 경우 결과를
 * stdout으로 출력한다. 명세서에 주어진 출력 예시와 완전히 동일할 필요는 없다.
 */
int make_literal_table_output(const char *literal_table_dir,
                              const literal *literal_table[],
                              int literal_table_length) {
    FILE *fp;
    int err = -1;

    if(literal_table_dir == NULL) {
        fp = stdout;
    } else {
        fp = fopen(literal_table_dir, "w+");
        if(fp == NULL) {
            printf("literal table output file create failure\n");
            return err;
        }
    }

    for(int i = 0; i < literal_table_length; i++) {
        fprintf(fp, "\t%-20s %X\n",
                literal_table[i]->literal,
                literal_table[i]->addr);
    }

    if(literal_table_dir != NULL) fclose(fp);
    err = 0;

    return err;
}

/**
 * @brief 오브젝트 코드를 파일로 출력한다. `objectcode_dir`이 NULL인 경우 결과를
 * stdout으로 출력한다.
 *
 * @param objectcode_dir 오브젝트 코드를 저장할 파일 경로, 혹은 NULL
 * @param obj_code 오브젝트 코드에 대한 정보를 담고 있는 구조체 주소
 * @return 오류 코드 (정상 종료 = 0)
 *
 * @details
 * 오브젝트 코드를 파일로 출력한다. `objectcode_dir`이 NULL인 경우 결과를
 * stdout으로 출력한다. 명세서의 주어진 출력 결과와 완전히 동일해야 한다.
 * 예외적으로 각 라인 뒤쪽의 공백 문자 혹은 개행 문자의 차이는 허용한다.
 */
int make_objectcode_output(const char *objectcode_dir,
                           const object_code *obj_code) {
    FILE *fp;
    int err = -1;

    if(objectcode_dir == NULL) {
        fp = stdout;
    } else {
        fp = fopen(objectcode_dir, "w+");
        if(fp == NULL) {
            printf("objectcode output file create failure\n");
            return err;
        }
    }

    for(int s = 0; s < obj_code->section_count; s++) {
        const section_code *sec = &obj_code->sections[s];

        // H 레코드
        fprintf(fp, "H%-6s%06X%06X\n",
                sec->section_name,
                sec->start_addr,
                sec->section_length);

        // D 레코드 (EXTDEF 있을 때만)
        if(sec->def_count > 0) {
            fprintf(fp, "D");
            for(int i = 0; i < sec->def_count; i++) {
                fprintf(fp, "%-6s%06X",
                        sec->def_symbols[i],
                        sec->def_addrs[i]);
            }
            fprintf(fp, "\n");
        }

        // R 레코드 (EXTREF 있을 때만)
        if(sec->ref_count > 0) {
            fprintf(fp, "R");
            for(int i = 0; i < sec->ref_count; i++) {
                fprintf(fp, "%-6s", sec->ref_symbols[i]);
            }
            fprintf(fp, "\n");
        }

        // T 레코드
        for(int i = 0; i < sec->text_record_count; i++) {
            const text_record *tr = &sec->text_records[i];
            fprintf(fp, "T%06X%02X%s\n",
                    tr->start_addr,
                    tr->length,
                    tr->code);
        }

        // M 레코드
        for(int i = 0; i < sec->mod_record_count; i++) {
            const modification_record *mr = &sec->mod_records[i];
            fprintf(fp, "M%06X%02X%c%s\n",
                    mr->addr,
                    mr->length,
                    mr->sign,
                    mr->symbol);
        }

        // E 레코드
        if(sec->has_start) {
            fprintf(fp, "E%06X\n", sec->exec_addr);
        } else {
            fprintf(fp, "E\n");
        }
    }

    if(objectcode_dir != NULL) fclose(fp);
    err = 0;

    return err;
}