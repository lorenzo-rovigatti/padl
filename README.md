# padl

## Compilation

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

At the end of the compilation two executables, `client` and `server`, will be placed in the folder where you run `make`. From here on only `client` will be discussed.

## Usage

```
./client  [--mode <serial mode>] [-b <bauds>] [-p <COM port number (e.g. 0)>] [-s <milliseconds>] [-d] [--] [--version] [-h] <an IP address (e.g. 192.168.0.1)> <a port number (e.g. 6000)>
```

Here is a rundown of the options:

* `--mode <serial mode>` Mode of the serial connection, defaults to 8N1
* `-b <bauds>,  --baudrate <bauds>` Baudrate of the serial connection, defaults to 9600
* `-p <COM port number (e.g. 0)>,  --serial-port <COM port number (e.g. 0)>` The COM port number of the serial port to which the output will be printed
* `-s <milliseconds>,  --sleep <milliseconds>` Sleeping time between sendings (in milliseconds)
* `-d,  --dummy` Generate synthetic data
* `--,  --ignore_rest` Ignores the rest of the labeled arguments following this flag.
* `--version` Displays version information and exits.
* `-h,  --help` Displays usage information and exits.
* `<an IP address (e.g. 192.168.0.1)>` (required) The IP address of the DL device
* `<a port number (e.g. 6000)>` (required)  The TCP port of the DL device

## Obtain the readings

`client` polls a network address over the given port to obtain the readings. By default, those are printed to the standard output. If the DL device has IP address `192.168.2.10` then communication over the port `64000` can be established with the following command:

`./client 192.168.10.2 64000`

By default, the client polls the device continuously. Use the `-s <milliseconds>` option to decrease the output. For instance, the following command will print readings every second:

`./client 192.168.10.2 64000 -s 1000`

The default output has the following format:

`delta_time current_time reading1 reading2 ...`

where `delta_time` is the elapsed time (in microseconds), `current_time` is the current time in `HH:MM:SS.XXX` format, where `XXX` are milliseconds.

## Write to a serial port

If you use the `-p <COM port number>` switch, `client` will write the readings to the given serial port. See [below](#list-of-supported-com-ports) for a mapping between Linux and Windows serial ports and the `<COM port number>` argument.

When writing to a serial port, the output format is:

`n_readings reading1 reading2 ...`

where `n_readings` is the number of readings.

**Nota Bene**: you can use the `-d` switch to make `client` print 3 random integers to test your Arduino code without having to connect your computer to a proper DL device.

### A simple Arduino code

The following code can be used to obtain the readings from an Arduino controller connected to the serial port:

```
#define MAX_N_VALUES 16

void setup() {
  Serial.begin(9600);
}

void loop() {
  int values[MAX_N_VALUES];
  
  while(Serial.available() > 0) {
    //process_byte(Serial.read());   
    int n_values = Serial.parseInt();
    for(int i = 0; i < n_values; i++) {
      values[i] = Serial.parseInt();
      Serial.println(values[i]);
    }
  }
}
```

**Nota Bene:** the argument of the `Serial.begin()` function must match the number passed to the `-b` switch (which defaults to `9600`), which supports the baud rates specified [below](#list-of-supported-com-baud-rates).

### List of supported COM ports

List of comport numbers, possible baudrates and modes:

|COM port number |    Linux    | windows|
| --- | ----- | -----|
|0   |ttyS0   |  COM1|
|1   |ttyS1   |  COM2|
|2   |ttyS2   |  COM3|
|3   |ttyS3   |  COM4|
|4   |ttyS4   |  COM5|
|5   |ttyS5   |  COM6|
|6   |ttyS6   |  COM7|
|7   |ttyS7   |  COM8|
|8   |ttyS8   |  COM9|
|9   |ttyS9   |  COM10|
|10  |ttyS10  |  COM11|
|11  |ttyS11  |  COM12|
|12  |ttyS12  |  COM13|
|13  |ttyS13  |  COM14|
|14  |ttyS14  |  COM15|
|15  |ttyS15  |  COM16|
|16  |ttyUSB0 |  COM17|
|17  |ttyUSB1 |  COM18|
|18  |ttyUSB2 |  COM19|
|19  |ttyUSB3 |  COM20|
|20  |ttyUSB4 |  COM21|
|21  |ttyUSB5 |  COM22|
|22  |ttyAMA0 |  COM23|
|23  |ttyAMA1 |  COM24|
|24  |ttyACM0 |  COM25|
|25  |ttyACM1 |  COM26|
|26  |rfcomm0 |  COM27|
|27  |rfcomm1 |  COM28|
|28  |ircomm0 |  COM29|
|29  |ircomm1 |  COM30|
|30  |cuau0   |  COM31|
|31  |cuau1   |  COM32|
|32  |cuau2   |  n.a.|
|33  |cuau3   |  n.a.|
|34  |cuaU0   |  n.a.|
|35  |cuaU1   |  n.a.|
|36  |cuaU2   |  n.a.|
|37  |cuaU3   |  n.a.|

### List of supported COM baud rates

|Linux   |  Windows |
| ---- | ------ |
|50     |   n.a.|
|75     |   n.a.|
|110    |   110|
|134    |   n.a.|
|150    |   n.a.|
|200    |   n.a.|
|300    |   300|
|600    |   600|
|1200   |   1200|
|1800   |   n.a.|
|2400   |   2400|
|4800   |   4800|
|9600   |   9600|
|19200  |   19200|
|38400  |   38400|
|57600  |   57600|
|115200 |   115200|
|230400 |   128000|
|460800 |   256000|
|500000 |   500000|
|576000 |   n.a.|
|921600 |   921600|
|1000000|   1000000|
|1152000|   n.a.|
|1500000|   1500000|
|2000000|   2000000|
|2500000|   n.a.|
|3000000|   3000000|
|3500000|   n.a.|
|4000000|   n.a.|

### List of supported COM modes

|Modes|
|---|
|8N1|
|8O1|
|8E1|
|8N2|
|8O2|
|8E2|
|7N1|
|7O1|
|7E1|
|7N2|
|7O2|
|7E2|
|6N1|
|6O1|
|6E1|
|6N2|
|6O2|
|6E2|
|5N1|
|5O1|
|5E1|
|5N2|
|5O2|
|5E2|