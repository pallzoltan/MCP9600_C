#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

void main() {
  // Create I2C bus
  int file;
  char *bus = "/dev/i2c-1";
  if ((file = open(bus, O_RDWR)) < 0)
  {
    // Failed to open the bus.
    printf("0 0");
    exit(1);
  }

  int temp1 = getTemp(file, 0x60);
  int temp2 = getTemp(file, 0x67);

  printf("%d %d", temp1, temp2);

  exit(0);
}

int getTemp(int file, int address) {

  ioctl(file, I2C_SLAVE, address);

  char config[2] = {0};
  config[0] = 0x05;
  config[1] = 0x00;
  write(file, config, 2);

  config[0] = 0x06;
  config[1] = 0x00;
  write(file, config, 2);

  char reg[1] = {0};
  reg[1] = 0x04;
  write(file, reg, 1);

  char stat[1] = {0};

  do {
    read(file, stat, 1);
    // printf("%d\n", stat[0]);
  } while(stat[0] != 1);

  char data[2] = {0};

  if(read(file, data, 2) != 2) {
    return 0;
  } else {

    int low_temp = data[1] & 0x80;
    if(low_temp) {
      return data[0] * 16 + data[1] / 16 - 4096;
    } else {
      return data[0] *16 + data[1] * 0.0625;
    }
  }
}
