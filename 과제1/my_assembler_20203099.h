/*
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
 */
#define MAX_INST_TABLE_LENGTH 256
#define MAX_INPUT_LINES 5000
#define MAX_TABLE_LENGTH 5000
#define MAX_OPERAND_PER_INST 3
#define MAX_OBJECT_CODE_STRING 74
#define MAX_OBJECT_CODE_LENGTH 5000

/*
 * instruction 목록을 저장하는 구조체이다.
 * instruction 목록 파일로부터 정보를 받아와서 생성한다.
 * instruction 목록 파일에는 라인별로 하나의 instruction을 저장한다.
 */
typedef struct _inst {
    char str[10];
    unsigned char op;
    int format;
    int ops;
} inst;

/*
 * 어셈블리 할 소스코드를 토큰으로 변환하여 저장하는 구조체 변수이다.
 * operator 변수명은 renaming을 허용한다.
 */
typedef struct _token {
    char *label;
    char *operator;
    char *operand[MAX_OPERAND_PER_INST];
    char *comment;
    // char nixbpe; // 다음 과제에 사용될 변수
} token;

/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
 * 추후 과제에 사용 예정
 */
typedef struct _symbol {
    char name[10];
    int addr;
} symbol;

/*
 * 리터럴을 관리하는 구조체이다.
 * 리터럴 테이블은 리터럴의 이름, 리터럴의 위치로 구성된다.
 * 추후 과제에 사용 예정
 */
typedef struct _literal {
    char literal[20];
    int addr;
} literal;

//--------------

int init_inst_table(inst **inst_table, int *inst_table_length,
                    const char *inst_table_dir);
int init_input(char **input, int *input_length, const char *input_dir);
int assem_pass1(const inst **inst_table, int inst_table_length,
                const char **input, int input_length, token **tokens,
                int *tokens_length, symbol **symbol_table,
                int *symbol_table_length, literal **literal_table,
                int *literal_table_length);
int token_parsing(const char *input, token *tok);
int search_opcode(const char *str, const inst **inst_table,
                  int inst_table_length);
int make_opcode_output(const char *output_dir, const token **tokens,
                       int tokens_length, const inst **inst_table,
                       int inst_table_length);
int assem_pass2(const token **tokens, int tokens_length,
                const symbol **symbol_table, int symbol_table_length,
                const literal **literal_table, int literal_table_length,
                char object_code[][MAX_OBJECT_CODE_STRING],
                int *object_code_length);
int make_symbol_table_output(const char *symtab_dir,
                             const symbol **symbol_table,
                             int symbol_table_length);
int make_literal_table_output(const char *literal_table_dir,
                              const literal **literal_table,
                              int literal_table_length);
int make_objectcode_output(const char *objectcode_dir,
                           const char object_code[][MAX_OBJECT_CODE_STRING],
                           int object_code_length);