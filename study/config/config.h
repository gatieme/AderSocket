/**
*********************************************************************************
*   CopyRight  : 2013-2014, HerBinUnversity, GatieMe                            *
*   File Name  : BinaryTuple.h                                                  *
*   Description: CTools                                                         *
*   Author     : Gatie_Me                                                       *
*   Version    : Copyright 2013-2014                                            *
*   Data_Time  : 2013-3-10 21:29:24                                             *
*   Content    : CTools-Lexical                                                 *
*********************************************************************************
**/



#ifndef _CONFIG_H_INCLUDED
#define _CONFIG_H_INCLUDED







////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>          // 标准输入输出
#include <stdlib.h>         // 标准库函数
#include <stdarg.h>         // 可变参数列表
#include <assert.h>         // 断言函数
#include <limits.h>         // 宏信息库
#include <errno.h>          // 错误信息
#include <string.h>         // 字符串处理
#include <ctype.h>          // 字符处理
#include <stdbool.h>        // BOOL类型
#include <malloc.h>         // 堆空间处理
#include <time.h>
/////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////


///CTools宏函数信息
///////////////////////////////////////////////////////////////////////////////////////////
///CTools宏函数信息
///////////////////////////////////////////////////////////////////////////////////////////
#
#
/// 当前字符是数字
#define IsDigit(c)         (c >= '0' && c <= '9')
/// 当前字符是8进制数据
#define IsOctDigit(c)      (c >= '0' && c <= '7')
/// 当前字符是16进制数据
#define IsHexDigit(c)      (IsDigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
/// 当前字符是字母或者_
#define IsLetter(c)        ((c >= 'a' && c <= 'z') || (c == '_') || (c >= 'A' && c <= 'Z'))
/// 当前自负是否满足C的变量命名规则
#define IsLetterOrDigit(c) (IsLetter(c) || IsDigit(c))
#define IsIdentifier    IsLetterOrDigit
/// 当前字符是空白字符
#define IsSpace(c)  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')
///
#define ToUpper(c)		   (c & ~0x20)
/// 取出当前信息的高4位
#define HIGH_4BIT(v)       ((v) >> (8 * sizeof(int) - 4) & 0x0f)
/// 取出当前信息的高3位
#define HIGH_3BIT(v)       ((v) >> (8 * sizeof(int) - 3) & 0x07)
/// 取出当前信息的高1位
#define HIGH_1BIT(v)       ((v) >> (8 * sizeof(int) - 1) & 0x01)
#define ALIGN(size, align) ((size + align - 1) & (~(align - 1)))
#
#
///////////////////////////////////////////////////////////////////////////////////////////








/// CTools宏函数信息
//////////////////////////////////////////////////////////////////////////////////////////////
#define SHOW_FILE_NAME(fileName)     do                                                     \
                                    {   int i = strlen(fileName) - 1;                       \
                                        for( ; fileName[i] != '\\'                          \
                                        && fileName[i] != '/' && i >= 0; i--);              \
                                        char *fname = malloc(strlen(fileName) - i);         \
                                        strcpy(fname, fileName + i + 1);                    \
                                        printf("File: %s  ", fname);                        \
                                        free(fname);                                        \
                                    }while( 0 );
#
#
#define __file__ __FILE__           ///CTools文件名输出处理
#define FILENAME()                  do                                                      \
                                    {   int i = strlen(__FILE__) - 1;                       \
                                        for( ;                                              \
                                             __FILE__[i] != '\\' && __FILE__[i] != '/';     \
                                            i--);                                           \
                                        char *fname = malloc(strlen(__FILE__) - i);         \
                                        strcpy(fname, __FILE__ + i + 1);                    \
                                        printf("FileName: %s", fname);                      \
                                        free(fname);                                        \
                                    }while( 0 );
#
#
#define __line__ __LINE__           /// CTools行号输出处理
#define LINE( )                     do                                                      \
                                    {                                                       \
                                        printf("Line: %d", __LINE__);                       \
                                    }while( 0 );
#
#
#define __FUNC__ __func__           /// CTools函数名输出处理
#define FUNC( )                     do                                                      \
                                    {                                                       \
                                        printf("Function: %s", __FUNC__);                   \
                                    }while( 0 );
#
#
#define FILE_FUNC_LINE( )           do                                                      \
                                    {                                                       \
                                        FILENAME( );                                        \
                                        putchar(' ');                                       \
                                        FUNC( );                                            \
                                        putchar(' ');                                       \
                                        LINE( );                                            \
                                        putchar('\n');                                      \
                                    }while(0);
#
#
///////////////////////////////////////////////////////////////////////////////////////////////



/// 其他帮助函数信息
///////////////////////////////////////////////////////////////////////////////////////////////
#
#
// 延时函数
#define DELAY( )                    do                                                      \
                                    {                                                       \
                                        int i;                                              \
                                        for(i = 0; i < 1000000; i++);                       \
                                    }while( 0 );
#
#
#define PAUSE( )                    do                                                       \
                                    {                                                        \
                                        printf("Please enter any key to continue...\n");     \
                                        getchar( );                                          \
                                    }while( 0 );
#define STEP( ) PAUSE( )
#
// 版本输出函数
#define  VERSION( ) do                                                                              \
                    {   printf("\t\t**************************************************\n");         \
                        printf("\t\t**  Wacky Window (c) 2012 Wacky SoftWare. Inc.  **\n");         \
                        printf("\t\t**     Complied on %s at %s      **\n", __DATE__, __TIME__);    \
                        printf("\t\t**************************************************\n");         \
                        PAUSE( );                                                                   \
                    }while( 0 );
#
#
//////////////////////////////////////////////////////////////////////////////////////////////////////////////









#endif      // ENDIF __CONFIG_H_
