/******************************************************************************
 * C++ source of RPX-100-TX
 *
 * File:   SDRsamples-TX.h
 * Author: Bernhard Isemann
 *
 * Created on 06 Jan 2022, 12:37
 * Updated on 20 Feb 2022, 17:00
 * Version 2.00
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <syslog.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdio>
#include <ctime>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <chrono>
#include <cstring>
#include <bitset>
#include "ini.h"
#include "log.h"
#include "lime/LimeSuite.h"
#include <chrono>
#include <math.h>
#include "alsa/asoundlib.h"
#include "liquid/liquid.h"
#include "sockets/ServerSocket.h"
#include "sockets/SocketException.h"
#include <iterator>
#include <signal.h>
#include <pthread.h>
#include "Util.h"
#include "WebSocketServer.h"


#pragma once

#define NUM_CONNECTS 5 // max number of sockets connections
#define PORT_NUMBER 8084
#define TIMEOUT 500
#define SUBCARRIERS 1024
#define DATACARRIERS 480
#define DEFAULT_PHY_MODE 1
#define DEFAULT_CYCL_PREFIX 4

extern pthread_mutex_t SDRmutex;

// SDR facility
lms_device_t *device = NULL;
int SDRinitTX(double frequency, double sampleRate, int modeSelector, double normalizedGain);
int SDRset(double frequency, double sampleRate, int modeSelector, double normalizedGain);
string exec(string command);
int startSDRTXStream();
int OFDMframeAssemble();
void *sendBeacon(void *threadID);
int error();
extern double frequency;
extern double sampleRate;
extern int modeSelector;
extern double normalizedGain;


// Radio Frontend - Define GPIO settings for CM4 hat module
uint8_t setRX = 0x18;       // GPIO0=LOW - RX, GPIO3=HIGH - PTT off,
uint8_t setTXDirect = 0x0F; // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=HIGH, GPIO2=HIGH
uint8_t setTX6m = 0x0D;     // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=LOW, GPIO2=LOW
uint8_t setTX2m = 0x09;     // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=LOW, GPIO2=HIGH
uint8_t setTX70cm = 0x0B;   // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=HIGH, GPIO2=LOW

uint8_t setTXDirectPTT = 0x07; // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=HIGH, GPIO2=HIGH
uint8_t setTX6mPTT = 0x05;     // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=LOW, GPIO2=LOW
uint8_t setTX2mPTT = 0x01;     // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=LOW, GPIO2=HIGH
uint8_t setTX70cmPTT = 0x03;   // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=HIGH, GPIO2=LOW

string modeName[9] = {"RX", "TXDirect", "TX6m", "TX2m", "TX70cm", "TXDirectPTT", "TX6mPTT", "TX2mPTT", "TX70cmPTT"};
uint8_t modeGPIO[9] = {setRX, setTXDirect, setTX6m, setTX2m, setTX70cm, setTXDirectPTT, setTX6mPTT, setTX2mPTT, setTX70cmPTT};

// Log facility
void print_gpio(uint8_t gpio_val);
std::stringstream msgSDR;
std::stringstream HEXmsgSDR;


// Initialize sdr buffers
const int sampleCnt = 1048;  // complex samples per buffer --> a "sample" is I + Q values in float or int
float buffer[sampleCnt * 2]; // buffer to hold samples (each I + Q) --> buffer size = 2 * no of samples
liquid_float_complex c_buffer[sampleCnt]; // complex buffer to hold SDR sample in complex domain
liquid_float_complex complex_i(0,1);
int samplesRead = 1048;

bool rxON = true;
bool txON = true;

//Beacon frame parameters
unsigned int cp_len;
unsigned int taper_len;

int startSDRTXStream(int *tx_buffer, int FrameSampleCnt);
int BeaconFrameAssemble(int *symbols, int *r_frame_buffer);
void subcarrier_allocation (unsigned char *array);
int DefineFrameGenerator (int dfg_cycl_pref, int dfg_PHYmode, ofdmflexframegen *generator, unsigned int *dfg_c_buffer_len, unsigned int *dfg_payload_len);