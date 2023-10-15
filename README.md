<h1 align="center">ZCC</h1>

<p>
    <p align="center">
        ZCC是一个用C++写的C99的编译器。<br>
        原2017年本科《编译原理》免考课设。
    </p>
</p>

## Usage

### ENV

- `CMake >= 3.16`
- `C++ 20`

```bash
cd zcc 
mkdir build && cd build

cmake ..
cmake --build .
```

### 命令

```bash
Options:
        -P                only preprocessed
        -E                print preprocessed source code
        -o filename       Output to the specified file
        -h                help
        -V                use virtual machine to run the program.
        -D                Debug(virtual machine mode)
        -A                Print all data
Debug:
        n                 next, print next assembly instruction and register data.
        x <addr>          print the data of the address.
        q                 exit
Output:
        .q                四元式文件
        .s                AT&T汇编
        .st               输出的符号表文件
        .ts               输出的Token序列文件
```

### 头文件

当前zcc使用的不是标准头文件，所以想要使用头文件，把头文件放到以下位置:

```bash
# Windows：
D:/zcc/include/
C:/zcc/include/

# Linux:
/usr/include/zcc/
```

## 效果

![效果](/zcc.gif)

## 支持语法

### 预处理指令

```C
#include 
#define
#undef
#if
#ifndef
#ifdef
#elif
#else
#endif
```

### 数据类型

```C
        string

INTEGER:
        char, short, int, long 

FLOAT:
        float, double

OTHERS:
        array
        enum
        struct
        使用typedef定义的struct类型
```

### 运算符

```C
+  -  *  /  %
+=  -=   *=   /=   %=
<  >  >=  <=  ==  !=
||  &&  >>  <<  ~  ^  |  & 
++  -- . []  ? :  ->
sizeof
```

#### 不支持

```C
!
```

### 支持函数(递归)调用

### 支持参数传递， 可变参数

### 支持函数重载

支持基本变量作为参数时函数重载

### 控制流

```C
if () {} else{ }

while() {}

do {} while ()

for (;;) {}

break

continue

return

goto

switch() { case: break; default: } 
```

## 执行

### Default Mode

默认模式下，会自动生成`AT&T`汇编代码，并调用`gcc`汇编得到可执行文件

### Virtual-Machine Mode

`Virtual-Machine`模式下，生成汇编代码，不调用`gcc`，而是在*虚拟机*中运行

## 注意

1. ~~函数必须`return`~~
2. 碍于寄存器分配不完善，多使用中间变量，防止寄存器溢出。
3. 只支持生成32位汇编代码。

## 已知BUG

1. 打开函数重载后，无法使用函数调用做函数的参数，暂时无法获取作为参数的函数的信息;
2. `d=max(sum(a,b), max(c,b));`这样的也不行，执行顺序为`sum -> max ->max`，最后调用max的时候，寄存器的值已经被调用前两个的时候修改了;
3. ~~指针运算除了前置自增自减，别的不行;~~
4. ~~`bool`表达式溢出的问题，准别后面使用AST做的时候解决这个问题;(回填算法解决)~~
5. 浮点比较有整数，报浮点数据错误;
6. 一个表达式中使用多次多维数组，寄存器溢出问题;
7. `##`粘贴后的内容不一定为字符串，需要重新解析。
8. ~~`y--`的减运算的位置会有错误，比如在跳转中。~~
9. ~~重定义宏时不会覆盖。~~
10. 数组做参数，并没有作为指针传值。
11. 不支持结构体做函数参数。
12. 比较运算没有中间变量保存结果，所以比较运算不能作为操作数参与运算。
13. 逻辑运算两侧必须是比较运算，不能出现单独的操作数，例如: `a && b` 类似的运算。
14. 不能进行`a ^= b ^= a ^= b`类似的连续运算。
15. 在Linux上运行经常出现未查明BUG。
