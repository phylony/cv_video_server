// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
//#define BOOST_ALL_DYN_LINK  1
//#define BOOST_ALL_NO_LIB    1   // ���� Boost �ⲻʹ�� auto-link  
//#define BOOST_XXX_NO_LIB    1   // ָ��ĳ���� XXX ��ʹ�� auto-link  
//   
//// Ȼ���� VC ����ѡ��, �� #pragma ָ�� Boost ���ļ�  
//// ʵ���� Boost ����������� #pragma ʵ�� VC �� auto-link  
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

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
