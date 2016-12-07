# zcc
> **zcc**是一个用C++写的C99的编译器.
<br><br>

## 一 、当前效果
        C语言例子：`simple/hello.c`
        中间代码输出:`build/hello.qd`
        AT&T汇编输出:`build/hello.s`

![效果](https://github.com/ffiirree/zcc/blob/master/simple/dis.png)

## 二、 支持语法
### 2.1 数据类型
        字符串常量、int、short、char、long 、一维数组
        float，double

        自定义struct类型，和使用typedef定义的struct类型
### 2.2 运算
```
        +  -  +=  -=  *  *=  /  /=  %  %=
        <  >  >=  <=  ==  !=
        ||  &&
        算术和逻辑 >>  << 
        bit  ^  |  & 
        ++  --
        前置& * ~
        .
        []

#### 不支持
        ? :
        前置+ -(常数前置可以)
        !
```
### 2.3 支持函数调用
### 2.4 支持参数传递，支持函数重载
        支持可变参数
        <br>
### 2.5 支持函数重载
        支持基本变量作为参数时函数重载

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
```

