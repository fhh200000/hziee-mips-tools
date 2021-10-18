#include <main.h>
#include <stdio.h>
#include <process.h>
#include <utility.h>
#include <string.h>
#include <limits.h>
extern U64 instructions[];
extern char buf[];

U64 instruction;
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s inputfile\n", argv[0]);
        return 0;
    }
    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        fprintf(stderr, "Error: File not found:%s.\n", argv[1]);
        return 0;
    }
    //打开输出文件。 
    #ifndef PATH_MAX //防止MSVC不写……
    #define PATH_MAX 4095
    #endif
    static char filename_buf[PATH_MAX];
    strcpy(filename_buf,argv[1]);
    filename_buf[strlen(filename_buf)-1] = 0;
    strcat(filename_buf,"disasm.s");
    FILE* out_fp = fopen(filename_buf,"w");
    while(1) {
        instruction = 0;
        fread(((char*)&instruction+4),sizeof(U32),1,fp);
        if(feof(fp)) //Bug Fix：多读一次。
            break;
        instruction = fix_byte_order(instruction);
        if(process(instruction)==ACCEPTED) {
            fprintf(out_fp,"%s\n",buf);
        }
    }
    fclose(out_fp);
    fclose(fp);
    printf("Output file has been saved to:%s.\n",filename_buf);
    printf("Bye.\n");
}