# zcc
> **zcc**是一个用C++写的C99的编译器.
<br><br>

## 一 、当前效果
        C语言例子：`simple/hello.c`
        中间代码输出:`build/hello.qd`
        AT&T汇编输出:`build/hello.s`

![效果](https://github.com/ffiirree/zcc/blob/master/simple/dis.gif)

## 二、 支持语法
### 2.0 预处理器
```
        include 
        define
        undef
        宏展开

部分支持：
        ifndef
        ifdef
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
        ||  &&
        >>  << 
        ~  ^  |  & 
        ++  --
        .
        []
        ? :
        ->
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

## 三、执行
### 3.1  default mode
```
        默认模式下，会自动生成AT&T汇编代码，并调用gcc编译得到可执行文件
```
<br>

### 3.2  virtual machine mode
```
        virtual machine mode 下，暂时也会产生汇编代码，不过不会调用gcc，而是在虚拟机中运行
```