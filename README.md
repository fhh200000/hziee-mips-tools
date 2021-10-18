

# hziee-mips-tools

Simple&Naive MIPS tools for HZIEE' s  Computer Organization courses. 

为HZIEE的计组课程设计的young、simple&naive的MIPS工具。

Assembler tool(as), Disassembler tool(objdump) and dynamic debugger(db) are included.

套件包含了汇编器/反汇编器和动态调试器。

**NOTE: Only 19 instructions are implemented for class. Any requests about adding instructions will be IGNORED.**

**注意：程序只实现了教材要求的19条指令。关于增加更多指令的请求将会被  *忽略*  。 **

## Overview

`fhhas` : Converts MIPS assembly into binary code(\*.o) and Coefficient(\*.coe) files.

(将MIPS汇编代码转换为二进制文件(\*.o)和系数文件(\*.coe))

`fhhobjdump`：Convents binary code(\*.o) and Coefficient(\*.coe) files into MIPS assembly.

(将二进制文件(\*.o)和系数文件(\*.coe)转换为MIPS汇编代码)

`fhhdb`：Dynamically loads and executes code.

(动态加载并执行代码)

## Compilation

### fhhas & fhhobjdump

For MSVC users, please open development environment (depending your CPU Architecture) and use nmake to compile each project.

`nmake -f Makefile.windows`

对于MSVC（Visual Studio）用户而言，请根据您的CPU架构打开对应的开发人员工具，并使用上述命令借助nmake工具编译项目。

For MacOS / Linux / MinGW users, just execute `make` command in the project dir.

对于 MacOS / Linux / MinGW 用户而言，只需在项目目录下执行`make`命令即可。

The output executable will be placed in `build` dir.

输出程序将会放在`build`文件夹内。

### fhhdb

Use Qt 5 to load project and compile.

使用Qt 5加载项目并编译。

