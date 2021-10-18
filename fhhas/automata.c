#include <automata.h>
#include <table.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <utility.h>
extern status (*check_type[])(char* begin,int length);
extern U32 check_type_length;
extern seq_list token_list,token_val_list;
extern FILE *out_fp,*out_coe_fp;
extern U32 instruction_position;
U64 (*process)(char* in,int length);
int current_instruction_position = -1;
status naive_dfa(transrule* instruct,int rulelength,int inputlength,char* in)
{
    char* pointer = in;
    status current_status = 0;
    status previous_status = 0;
    while(pointer<in+inputlength)
    {
        int i=-1;
        //跳转到与当前状态匹配的表项入口
        while(instruct[++i].curr!=current_status && i<rulelength);
        i--;
        //跳转到与当前状态相匹配的字符
        while(instruct[++i].input!=*pointer && i<rulelength);
        //判断是否越界
        if(i>=rulelength)
            return REJECTED;
        //判断当前项目是否匹配
        if(instruct[i].input==*pointer&&instruct[i].curr==current_status)
        {
            current_status = instruct[i].next;
            pointer++;
        }
        else //不匹配
        {
            return REJECTED;
        }
    }
    if(current_status&ACCEPTED)
        return current_status&(~ACCEPTED);//去除accepted标记
    else
        return REJECTED;
}
extern U32 instruction_buffer;
extern U8 instruction_type;
extern U8 remaining_parameter_count;
static char* types[] = {"R type command","I type command","J type command"};
U64 process_pass_2(char* in,int length)
{
    U8 has_something = 0;
    char* pointer = in;
    int piece_length;
    while(length) {
        //跳过开头的空格或逗号。
        while(length&&(*pointer==0x20||*pointer==','||*pointer=='('||*pointer==')'))
        {
            length--;
            pointer++;
        }
        piece_length = 0;
        //寻找后面的空格或逗号以及前后括号,确定长度。
        while((length-piece_length)>0
            &&(*(pointer+piece_length)!=0x20)
            &&(*(pointer+piece_length)!=',')
            &&(*(pointer+piece_length)!='(')
            &&(*(pointer+piece_length)!=')'))
        {
            piece_length++;
        }
        //长度为0-->空行。
        if(!piece_length) {
            if(!has_something) {
                return 0;
            }
            else {
                goto output;
            }
        }
        status result;
        for(U32 i=0;i<check_type_length;i++)
        {
            result = check_type[i](pointer,piece_length);
            if(result!=REJECTED)
            {
                has_something = 1;
                if(token_store(i,result,pointer,piece_length)==REJECTED) {
                    return REJECTED;
                }
                pointer+=piece_length;
                length -=piece_length;
                break;
            }
        }
        if(result==REJECTED) 
        {
            //没找到
            printf("Failed to process %s.\n",pointer);
            return REJECTED;
        }
    }
output:
    if(instruction_type) {
        printf("0x%08x\t%-28s%s.\n",instruction_buffer,in,types[instruction_type-1]);
        U32 byte_order_fixed = fix_byte_order((int)instruction_buffer);
        fwrite(&byte_order_fixed,sizeof(U32),1,out_fp);
        if(instruction_position!=1) {
            fprintf(out_coe_fp,",");
        }
        fprintf(out_coe_fp,"%08x",(int)instruction_buffer);
    }
    else {
        printf("%s\n",in);
    }
    return 0;
}
char token_buff[100];
U64 process_pass_1(char* in,int length)
{
    char* pointer = in;
    int piece_length = -1;
    //第一遍扫描很简单：只需要判断是否是标号/指令即可。
    while(length&&(*pointer==0x20||*pointer==','))
    {
        length--;
        pointer++;
    }
    piece_length = 0;
    //寻找后面的空格或逗号,确定长度。
    while((length-piece_length)>0&&(*(pointer+piece_length)!=0x20)&&(*(pointer+piece_length)!=','))
    {
        piece_length++;
    }
    //长度为0-->空行。
    if(!piece_length) {
        return 0;
    }
    //如果是指令，把当前指令位置+1.
    if(is_instruction(pointer,piece_length)!=REJECTED) {
        current_instruction_position++;
    }
    //如果是标签，将标签存入顺序表。
    if(is_label(pointer,piece_length)!=REJECTED) {
        char* back = pointer+piece_length;
        sscanf(pointer,"%[^:]",token_buff);
        void* allocated = malloc(sizeof(char*)*(strlen(token_buff)+1));
        memcpy(allocated,token_buff,strlen(token_buff)+1);
        seq_list_append(&token_list,allocated);
        allocated = malloc(sizeof(int));
        *(int*)allocated = current_instruction_position+1;
        seq_list_append(&token_val_list,allocated);
        //标签分为两种格式。
        //如果标签不换行，那么需要手动去判断后面跟的是不是命令。
        while(*back) {
            if(*back!=' ') {
                current_instruction_position+=1;
                break;
            }
            back++;
        }
    }
    return 0;
}
