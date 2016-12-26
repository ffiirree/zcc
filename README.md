# zcc
> **zcc**是一个用C++写的C99的编译器.
<br><br>

## 一、 使用
### 1.1 控制命令
```
命令:
        -P                only preprocessed
        -E                print preprocessed source code
        -o filename       Output to the specified file
        -h                help
        -V                use virtual machine to run the program.
        -D                Debug(virtual machine mode)
Debug:
        n                 next, print next assembly instruction and register data.
        x <addr>          print the data of the address.
        q                 exit
Output:
        .q                四元式文件
        .s                AT&T汇编
```
### 1.2 头文件
当前zcc使用的不是标准头文件，所以想要使用头文件，把头文件放到以下位置:
```
        Windows：
                "D:/zcc/include/"
                "C:/zcc/include/"

        Linux:
                "/usr/include/zcc/"
```

## 二 、当前效果
![效果](https://github.com/ffiirree/zcc/blob/master/simple/dis.gif)

## 三、 支持语法
### 2.0 预处理器
```
        include 
        define
        undef
        if
        ifndef
        ifdef
        elif
        else
        endif
```

### 2.1 数据类型
```
        string

INTEGER:
        char, short, int, long 

FLOAT:
        float, double

OTHER:
        array
        enum
        struct
        使用typedef定义的struct类型
```
### 2.2 运算
```
        +  -  *  /  %
        +=  -=   *=   /=   %=
        <  >  >=  <=  ==  !=
        ||  &&  >>  <<  ~  ^  |  & 
        ++  -- . []  ? :  ->
```

#### 不支持
```
        前置+ -(常数前置可以)
        !
```
### 2.3 支持函数(递归)调用

### 2.4 支持参数传递

### 2.5 支持函数重载
```
        支持基本变量作为参数时函数重载
```

### 2.6 控制流
```
        支持 if...else..
        支持 while
        支持 do..while..
        支持 for
        支持 goto
        支持 switch case
        支持 break
        支持 continue
        支持 default
        支持 return
        支持 case
```

## 四、执行
### 3.1  default mode
```
        默认模式下，会自动生成AT&T汇编代码，并调用gcc编译得到可执行文件
```
<br>

### 3.2  virtual machine mode
```
        virtual machine mode 下，暂时也会产生汇编代码，不过不会调用gcc，而是在虚拟机中运行
```
## 五、Attention
```
        1. 函数必须return。
        2. 碍于寄存器分配不完善，多使用中间变量，防止寄存器溢出。
        3. 只支持生成32位汇编代码。
```

## 六、BUG
```
        1. 打开函数重载后，无法使用函数调用做函数的参数，暂时无法获取作为参数的函数的信息;
        2. d=max(sum(a,b), max(c,b));这样的也不行，执行顺序为sum -> max ->max，最后调用max的时候，寄存器的值已经被
           调用前两个的时候修改了;
        3. 指针运算除了前置自增自减，别的不行;
        4. bool表达式溢出的问题，准别后面使用AST做的时候解决这个问题;
        5. 浮点比较有整数，报浮点数据错误;
        6. 一个表达式中使用多次多维数组，寄存器溢出问题;
        7. ##粘贴后的内容不一定为字符串，需要重新解析。
        8. y--的减运算的位置会有错误，比如在跳转中。
```