#include "mbed.h"
#include "SerialBuffered.h"
#include <cstdint>
#include <ctime>
#include <cstdlib>

#define UART_SW_USER  0
#define WIND_DATA_LEN 20

DigitalOut uart_sw(PA_3);
RawSerial pc(USBTX,USBRX,9600);
RawSerial rawserial(PA_9, PA_10, 38400);
SerialBuffered serial(PA_9, PA_10);

float get_wind_velocity(void);
float extract_wind_velocity(const char *input);
float get_wind_direction(void);
float extract_wind_direction(const char *input);

int main() {
  uart_sw = UART_SW_USER;
  serial._baud(38400);
  while(1) {
    float wind_velocity = get_wind_velocity();
    float wind_direction = get_wind_direction();

    pc.printf("\r\n---------- wind information ----------\r\n");
    pc.printf("wind speed: %.3f[m/s]\r\n", wind_velocity);
    pc.printf("wind direction: %.0f[deg]\r\n", wind_direction);
    wait(1.0);
  }
}

float get_wind_velocity(void) {
  rawserial.printf("<RM,>??\r\n");

  char old_data;
  char new_data;
  char rawdata[WIND_DATA_LEN] = {0};
  uint8_t len = 0;

  while(!serial.readable());
  for(uint8_t i = 0; i < WIND_DATA_LEN; i++) {
    new_data = serial.getc();
    rawdata[i] = new_data;
    if(old_data == '\r' && new_data == '\n') {
      len = i;
      break;
    }
    old_data = new_data;
  }

  return extract_wind_velocity(rawdata);
}

float get_wind_direction(void) {
  rawserial.printf("<RD,M05,>??\r\n");

  char old_data;
  char new_data;
  char rawdata[WIND_DATA_LEN] = {0};
  uint8_t len = 0;

  while(!serial.readable());
  for(uint8_t i = 0; i < WIND_DATA_LEN; i++) {
    new_data = serial.getc();
    rawdata[i] = new_data;
    if(old_data == '\r' && new_data == '\n') {
      len = i;
      break;
    }
    old_data = new_data;
  }

  return extract_wind_direction(rawdata);
}

float extract_wind_velocity(const char *input) {
  const char* start = strchr(input, ','); // 最初のカンマを探す
  if (start == NULL) return 0.0f;

  start += 1; // カンマの次の文字から開始

  char buffer[32];
  int i = 0;
  while (*start != ',' && *start != '\0' && i < (int)(sizeof(buffer) - 1)) {
      buffer[i++] = *start++;
  }
  buffer[i] = '\0';

  return strtof(buffer, NULL);
}

float extract_wind_direction(const char *input) {
  const char* start = strchr(input, '='); // 最初のカンマを探す
  if (start == NULL) return 0.0f;

  start += 1; // カンマの次の文字から開始

  char buffer[32];
  int i = 0;
  while (*start != ',' && *start != '\0' && i < (int)(sizeof(buffer) - 1)) {
      buffer[i++] = *start++;
  }
  buffer[i] = '\0';

  return strtof(buffer, NULL);

  // pc.printf("%s", input);
  // return 0;
}