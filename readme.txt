Digital Display LCD-Matrix control, Written in C++/WinAPI
Copyright (c) 2014  Daniel D Miller
web page: home.comcast.net/~derelict

This project incorporates parts of of Nic Wilson's C++/MFC control, available at:
http://www.codeproject.com/Articles/3174/Digital-Display-CStatic-control

This is free software.  It is distributed under the terms of the
Code Project Open License (CPOL) V1.02 :
http://www.codeproject.com/info/cpol10.aspx

The matrixstatic class (matrixstatic.cpp, matrixstatic.h) are:
//****************************************************************************
// Copyright (C) 2002 by Nic Wilson  http://www.nicsoft.com.au
// Written by Nic Wilson nicw@bigpond.net.au (dead website)
//
// Release Date and Version:
// Version: 1.0 November 2002
//****************************************************************************

###############################################################
Building the project

> With MinGW (I used V4.4.1)
In the main project directory, type "make"

> With Visual Studio 2010
Open LedScroll\LedScroll.sln and build

###############################################################
Accessing font files from the demo program.

The program expects to find these files in ..\fntcol relative
to the location of the executable file.

The font files can be obtained from my github:
git clone https://github.com/DerellLicht/fntcol.git

