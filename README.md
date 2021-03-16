# Hello DDS

## Pre-requisites
1. Setup 2 VM with Ubuntu (18.04)
2. Download and build OpenDDS
3. Source the generated **setenv.sh** file

Tips:
- Configure a common folder between the 2 VMs
- Configure VMs network to communicate together

## How to build
```
$ cd hello-dds
$ mwc.pl -type gnuace Hello.mwc
$ make
```

## Usage
### VM A
Open 2 terms.

1st term [DSCPInfoRepo]
```
$ DCPSInfoRepo -ORBListenEndpoints iiop://:12345
```

2nd term
```
$ ./publisher
```
### VM B
Open a term.

```
$ ./subscriber
```

