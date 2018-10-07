/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2017  <>
 * 
 * motor-term is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * motor-term is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>

#define MY_PORT	    (2368)
#define SERVER_PORT (2368)


#define HEAD_LEN      (42)
#define FLAG_LEN       (2)
#define AZIMUTH_LEN    (2)
#define DATA_LEN       (3)
#define DISTANCE_LEN   (2)
#define REFLECT_LEN    (1)
#define TIMESTAMP_LEN  (4)
#define FACTORY_LEN    (2)
#define BLOCK_LEN    (100)

static int get_command();

static void  init_sock();

int sendbuf( char* buf, size_t len );

void recvsocket(void);

void parselaser(char* p, int len);

double getAzimuth(char* p, int azimuthpos);

double getDistance(char* p, int distancepos);

int getReflectivity(char* p, int reflectpos);

int getReturnMode(char* p);

int findstr(char* buf, int startpos, int endpos, char* subbuf, int sublen);
