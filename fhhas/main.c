#include <main.h>
#include <stdio.h>
#include <stdlib.h>
#include <utility.h>
#include <string.h>
#include <automata.h>
extern int current_instruction_position;
char read_buff[100];
char output_coe_file_path[100];
int read_length;
seq_list token_list,token_val_list;
FILE *out_fp,*out_coe_fp;
int main(int argc,char** argv)
{
    if(argc!=2) {
        printf("Usage: %s inputfile\n",argv[0]);
        return 0;
    }
    FILE* fp = fopen(argv[1],"rb");
    if(!fp) {
        fprintf(stderr,"Error: File not found:%s.\n",argv[1]);
        return 0;
    }

    seq_list_init(&token_list);
    seq_list_init(&token_val_list);

    //第一遍：扫描整个文件，检测标签。
    printf("PASS 1: labels & constant value definitions.\n");
    process = process_pass_1;
    while(!feof(fp)) {
        fscanf(fp,"%[^\n]",read_buff);
        read_length = strlen(read_buff);
        fgetc(fp);
        //应用一些源代码patch以简化汇编器代码。
        
        for(int i=0;i<read_length;i++) {
            //patch 1:删除注释（';'->0x00/'#'->0x00）
            if(read_buff[i]==';'||read_buff[i]=='#') {
                read_buff[i]=0x00;
                read_length = i+1;
                break;
            }
            //patch 2:在行号的冒号后加上空格（':'->": "）
            if(read_buff[i]==':') {
                memmove(read_buff+i+2,read_buff+i+1,read_length-i);
                read_buff[i+1]=' ';
                read_length++;
            }
        }

        process(read_buff,read_length);
    }
    printf("Detected constant values:\n");
    for(int i=0;i<token_list.used;i++) {
        printf("%s->%d\n",(char*)token_list.data[i],*((int*)token_val_list.data[i]));
    }
    printf("Done.\n");
    //第二遍：正式汇编。
    printf("PASS 2: Real assembly.\n");
    int length = strlen(argv[1]);
    argv[1][length-1] = '\0';
    strcpy(output_coe_file_path,argv[1]);
    strcat(output_coe_file_path,"coe");
    argv[1][length-1] = 'o';
    out_fp = fopen(argv[1],"wb+");
    out_coe_fp = fopen(output_coe_file_path,"w+");
    process = process_pass_2;
    fseek(fp,0,SEEK_SET);
    fprintf(out_coe_fp,"memory_initialization_radix = 16;\nmemory_initialization_vector = ");
    while(!feof(fp)) {
        *read_buff = 0;
        fscanf(fp,"%[^\n]",read_buff);
        fgetc(fp);
        read_length = strlen(read_buff);

        //应用一些源代码patch以简化汇编器代码。
        
        for(int i=0;i<read_length;i++) {
            //patch 1:删除注释（';'->0x00/'#'->0x00）
            if(read_buff[i]==';'||read_buff[i]=='#') {
                read_buff[i]=0x00;
                read_length = strlen(read_buff);
                break;
            }
            //patch 2:在行号的冒号后加上空格（':'->": "）
            if(read_buff[i]==':') {
                memmove(read_buff+i+2,read_buff+i+1,read_length-i);
                read_buff[i+1]=' ';
                read_length++;
            }
        }

        //如果不是空行，那么继续。
        if(*read_buff) {
            process(read_buff,strlen(read_buff));
        }
    }
    fprintf(out_coe_fp,";\n");
    printf("Output file has been saved to:%s & %s\n",argv[1],output_coe_file_path);
    printf("Bye.\n");
    for(int i=0;i<token_list.used;i++) {
        free(token_list.data[i]);
        free(token_val_list.data[i]);
    }
    seq_list_delete(&token_list);
    seq_list_delete(&token_val_list);
    fclose(out_coe_fp);
    fclose(out_fp);
    fclose(fp);

    return 0;
}
U32 instruction_buffer;
U8 instruction_type = 0;
U8 remaining_parameter_count=0;
U8 reverse_op = 0;
U8 jump_method = 0;
U8 is_JR=0;
char buf[100];
U32 instruction_position = 0;
status token_store(int type,status result,char* pointer,int length)
{
    memcpy(buf,pointer,length);
    buf[length]=0;
    int base;
    switch (type)
    {
    case 0:
        //如果上一条语句还有未识别完成的参数，给出错误提示。
        if(remaining_parameter_count) {
            fprintf(stderr,"Error: missing parameters before %s.\n",buf);
        }
        instruction_type = (result&0x300000000)>>32;
        instruction_buffer = (U32)result;
        remaining_parameter_count = (result&0xC00000000)>>34;
        reverse_op = (result&0x1000000000)>>36;
        jump_method = (result&0x6000000000)>>37;
        is_JR=(result&0x8000000000)?1:0;
        instruction_position++;       
        break;
    case 1:
        //方法：假装自己是数字寄存器。
        sprintf(buf,"$%d",(int)(result&0b111111));
    case 2:
        if(instruction_type==3) { //J Type
            fprintf(stderr,"Error: J type command cannot have registers.\n");
            return REJECTED;
        }
        if(instruction_type==2) { //I Type
            if(!reverse_op) {
                if(remaining_parameter_count==1){
                    fprintf(stderr,"Error: Too many registers for I Type.\n");
                    return REJECTED;
                }
                if(jump_method==1) { //相对跳转rs,st全反，这里偷懒了。
                    instruction_buffer|=((atoi(buf+1))<<(5*remaining_parameter_count+6));                    
                }
                else {
                    instruction_buffer|=((atoi(buf+1))<<(-5*remaining_parameter_count+31));
                }
            }
            else {
                //FIXME:Better register position calculations.
                if(remaining_parameter_count==3)
                    instruction_buffer|=((atoi(buf+1))<<16);
                if(remaining_parameter_count==1)
                    instruction_buffer|=((atoi(buf+1))<<21); 
                if(remaining_parameter_count==2) {
                    fprintf(stderr,"Error: Incorrect position for registers.\n");
                    return REJECTED;
                }
            }
            remaining_parameter_count--;
        }
        if(instruction_type==1) { //R Type
            if(!reverse_op) {
                //FIXME:Better register position calculations.
                if(remaining_parameter_count==3&&!is_JR) {
                    instruction_buffer|=((atoi(buf+1))<<11);
                    goto out_R;
                }
                if(remaining_parameter_count==2|| is_JR) {
                    instruction_buffer|=((atoi(buf+1))<<21);
                    goto out_R;
                }
                if(remaining_parameter_count==1)
                    instruction_buffer|=((atoi(buf+1))<<16);               
            }
            else {
                instruction_buffer|=((atoi(buf+1))<<((3-remaining_parameter_count)*5+11));
            }
            out_R:
            remaining_parameter_count--;
        }
        break;
    case 3:
        base = (((buf[1]=='x')||(buf[0]=='-'&&buf[2]=='x'))?16:10);
        if((int)strtol(buf,nullptr,base)&(instruction_type==2?(~0xFFFFULL):(~0x3FFFFFFULL))) {
            fprintf(stderr,"Warning:Constant value overflow:0x%x ==> 0x%x.\n",
                    (int)strtol(buf,nullptr,base),
                    ((int)strtol(buf,nullptr,base)&(instruction_type==2?0xFFFF:0x3FFFFFF)));
        }
        instruction_buffer|=((int)strtol(buf,nullptr,base)&(instruction_type==2?0xFFFF:0x3FFFFFF));
        remaining_parameter_count--;
        break;
    case 4:
        //我们不需要考虑标签--在第一次扫描的时候已经分析过了。
        instruction_type = 0;
        break;
    case 5:
        //去表里查找值。
        for(int i=0;i<token_list.used;i++) {
            if(!strcmp(buf,token_list.data[i])) {
                //如果是相对跳转，那么需要计算相对偏移。
                if(jump_method==1) {
                    instruction_buffer |= ((*((int*)token_val_list.data[i]))-instruction_position)&0xFFFF;
                }
                else {
                    instruction_buffer |= *((int*)token_val_list.data[i]);
                }
                remaining_parameter_count--; 
                return 0;
            }
        }
        fprintf(stderr,"Cannot found constant value:%s.Stop.\n",buf);
        return REJECTED;
        break;
    default:
        break;
    }
    return 0;
}
