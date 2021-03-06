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
#include "liquid/liquid.h"
#include "sockets/ServerSocket.h"
#include "sockets/SocketException.h"
#include <iterator>
#include <signal.h>
#include <pthread.h>
#include "Util.h"
#include "WebSocketServer.h"

#pragma once

// external variables
extern pthread_mutex_t SDRmutex;

extern std::stringstream msgSDR;

extern double frequency;
extern double sampleRate;
extern double normalizedGain;
extern string mode;
extern int modeSelector;

lms_device_t *device = NULL;
int SDRinit(double freq, double sampleR, int modeSel, double normGain);
int SDRset(double freq, double sampleR, int modeSel, double normGain);
void *sendBeacon(void *threadID);
int startSDRTXStream(string message);
int error();
void print_gpio(uint8_t gpio_val);


// Radio Frontend - Define GPIO settings for CM4 hat module
uint8_t setRX = 0x08;       // GPIO0=LOW - RX, GPIO3=HIGH - PTT off,
uint8_t setTXDirect = 0x09; // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=HIGH, GPIO2=HIGH
uint8_t setTX6m = 0x0D;     // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=LOW, GPIO2=LOW
uint8_t setTX2m = 0x0F;     // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=LOW, GPIO2=HIGH
uint8_t setTX70cm = 0x0B;   // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=HIGH, GPIO2=LOW

uint8_t setTXDirectPTT = 0x01; // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=HIGH, GPIO2=HIGH
uint8_t setTX6mPTT = 0x05;     // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=LOW, GPIO2=LOW
uint8_t setTX2mPTT = 0x07;     // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=LOW, GPIO2=HIGH
uint8_t setTX70cmPTT = 0x03;   // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=HIGH, GPIO2=LOW

string modeName[9] = {"RX", "TXDirect", "TX6m", "TX2m", "TX70cm", "TXDirectPTT", "TX6mPTT", "TX2mPTT", "TX70cmPTT"};
uint8_t modeGPIO[9] = {setRX, setTXDirect, setTX6m, setTX2m, setTX70cm, setTXDirectPTT, setTX6mPTT, setTX2mPTT, setTX70cmPTT};

// Initialize sdr buffers
const int sampleCnt = 1024;               // complex samples per buffer --> a "sample" is I + Q values in float or int
float buffer[sampleCnt * 2];              // buffer to hold samples (each I + Q) --> buffer size = 2 * no of samples
liquid_float_complex c_buffer[sampleCnt]; // complex buffer to hold SDR sample in complex domain
liquid_float_complex complex_i(0, 1);
int samplesRead = 1024;

bool rxON = true;
bool txON = true;

// Beacon frame parameters
float sig[sampleCnt];
float kf = 0.1f;                 // modulation factor
float SNRdB = 30.0f;             // signal-to-noise ratio [dB]
string beaconMessage = "WRAN FSK Beacon at 52.8 MHz";
int tx_time = 20;
float modFactor = 0.8f;
float toneFrequency = 2e3;