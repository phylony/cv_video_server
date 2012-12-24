// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
//#define BOOST_ALL_DYN_LINK  1
//#define BOOST_ALL_NO_LIB    1   // 所有 Boost 库不使用 auto-link  
//#define BOOST_XXX_NO_LIB    1   // 指定某个库 XXX 不使用 auto-link  
//   
//// 然后用 VC 工程选项, 或 #pragma 指定 Boost 库文件  
//// 实际上 Boost 就依靠下面的 #pragma 实现 VC 的 auto-link  
//#pragma comment(lib, "libboost_thread-vc90-mt-gd-1_52.lib")  

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <opencv2/opencv.hpp>


#include <boost/thread/thread.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

// TODO: 在此处引用程序需要的其他头文件
