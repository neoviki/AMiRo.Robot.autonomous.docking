/*  AMiRo sensors wrapper 
 *
 *              Author: Viki (a) Vignesh Natarajan (vikiworks.io)
 */
#include <stdint.h>
#include <Types.h> 
#ifndef __SENSORS_H
#define __SENSORS_H

#define BlackFalling        0x1000
#define DOCK_PROXIMITY_HIGH 50000
#define DOCK_PROXIMITY_LOW  30000
#define PROXIMITY_SAFE_HIGH 3000
#define PROXIMITY_SAFE_LOW  1000

typedef enum{
    FloorWheelLeft = 2, 
    FloorWheelRight = 1,
    FloorFrontLeft = 3, 
    FloorFrontRight = 0 
}FloorSensorLocation_t;

typedef enum  {
    ProxFrontLeft = 3, 
    ProxFrontRight = 4, 
    ProxBackLeft = 0,
    ProxBackRight = 7, 
    ProxLeftBottom = 1, 
    ProxLeftTop = 2, 
    ProxRightTop = 5, 
    ProxRightBottom = 6, 
}ProxSensorLocation_t;

typedef enum{
    FloorBlack,
    FloorWhite /*White or Gray, some color other than black*/
}FloorSensorStatus_t;


typedef enum{
    MAG_X   = 0x00u,
    MAG_Y   = 0x01u,
    MAG_Z   = 0x02u,
}mag_axis_t;

typedef  enum {
    ACCL_X   = 0x00u,
    ACCL_Y   = 0x01u,
    ACCL_Z   = 0x02u,
}accel_axis_t;


extern FloorSensorStatus_t FloorSensorValue(FloorSensorLocation_t location);
extern uint16_t ProximitySensorValue(ProxSensorLocation_t location);
extern int32_t MagnetometerReading(mag_axis_t axis);

extern int16_t AccelReading(accel_axis_t axis);

extern types::position OdometerReading();
extern void OdometerReset();

/*Distance(in cm) to Approximate uint16_t [0.5 to 10 in 0.5 cm increments]*/
extern uint16_t ProxyDistance2uint16_t(float distance);
/*







typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
}sensor_accel_t

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
}sensor_gyro_t;

sensor_accel_t ReadAccelerometer();
void ReadGyroscope();
void ReadMagnetometer();
void ReadRingSensors();
void ReadFloorSensors();
*/
#endif
