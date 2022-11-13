# Self-learning yaw misalignment control intelligent entity interface based on DRC_Fortran wind turbine baseline
Self-learning yaw misalignment control intelligent entity is a Kalman filter and deep reinforcement learning based yaw control model for horizontal axis wind turbine. Using [Delft Research Controller (DRC) baseline wind turbine controller](https://github.com/TUDelft-DataDrivenControl/DRC_Fortran)[1], the DISCON to interface with the remote self-learning yaw misalignment control intelligent entity has been being developed. So with DNV GL BLADED, OpenFAST and etc, you can simulate or test the self-learning yaw misalignment control intelligent entity.

## Building and installing

### Linux:

This DISCON module is built, installed, and tested on Ubuntu 20.04.1. To build this module, prerequisites are mandatory as below.

```
sudo apt install -y libcurl4-openssl-dev libcjson-dev
```

Before building, in makefile, SERVO_DATA_DIR must be defined to copy the DISCON shared library file to the appropriate directory, and then, in the Source folder, you can build and install it.

```
make install
```

The share library file (libbaram.so) interfacing with the self-learning yaw misalignment control intelligent entity is copied to /usr/local/lib, so LD_LIBRARY_PATH should defined as below.

```
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH:+LD_LIBRARY_PATH:}/usr/local/lib
```

## Configuring
To interface with the self-learning yaw misalignment control intelligent entity, in DISCON.IN, Y_ControlMode must be defined as 3.

```
!------- CONTROLLER FLAGS -------------------------------------------------
2					! F_LPFType			- 1 = first-order low-pass filter, 2 = second-order low-pass filter
0					! F_NotchType		- 0 = disable, 1 = enable: notch on the measured generator speed, 
0					! IPC_ControlMode	- Turn Individual Pitch Control (IPC) for fatigue load reductions (pitch contribution) 0 = off / 1 = (1P reductions) / 2 = (1P+2P reductions)
1					! VS_ControlMode	- Generator torque control mode in above rated conditions, 0 = constant torque / 1 = constant power
3					! Y_ControlMode		- Yaw control mode: (0 = no yaw control, 1 = yaw rate control, 2 = yaw-by-IPC, 3 = yaw rate control by baram)
```

### Reference
1. Mulders, S.P. and van Wingerden, J.W. "Delft Research Controller: an open-source and community-driven wind turbine baseline controller." Journal of Physics: Conference Series. Vol. 1037. No. 3. IOP Publishing, 2018. [Link to the paper](https://iopscience.iop.org/article/10.1088/1742-6596/1037/3/032009/meta)

