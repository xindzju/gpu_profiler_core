/*
* All the internal MACRO and class/struct/enum/variable definitions
*/
#pragma once

#define GPC_VRESION "0.1.0"

#define MAX_PATH_LEN 512
#define MIN_PATH_LEN 128

//#define USE_CUSTOM_BACKEND

#define PRINT_MEMBER_FUNC_NAME  std::cout << typeid(*this).name() << "::" << __FUNCTION__ << std::endl;
