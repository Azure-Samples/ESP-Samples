
#ifndef SENSOR_H
#define SENSOR_H

void init_ambient_light_sensor();
float get_ambientLight();
void initialize_sensors();
float get_temperature();
void init_humidity_temp_sensor();
void init_humidity_temp_sensor();
float get_humidity();
void get_pitch_roll(int *pitch, int *roll);
void get_pressure_altitude(float *pressure, float *altitude);
void get_magnetometer(int *magnetometerX, int *magnetometerY, int *magnetometerZ);

#endif

