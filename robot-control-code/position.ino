#include <mbed.h>

// Kalman filter state for 3D positioning
float pos[3] = {0.0, 0.0, 0.0};        // Position (x, y, z)
float vel[3] = {0.0, 0.0, 0.0};        // Velocity (x, y, z)
float accel[3] = {0.0, 0.0, 0.0};      // Acceleration (x, y, z)
float dt = 0.01;                       // Time step (10ms)

// Kalman filter variables for each axis
float pos_error[3] = {1.0, 1.0, 1.0};  // Initial error in position estimate (x, y, z)
float pos_variance = 1.0;              // Position variance
float accel_variance = 1.0;            // Acceleration variance
float measurement_variance = 1.0;      // Measurement noise variance

// bais
float accel0_bias = 0.0102;
float accel1_bias = -0.0214;
float accel2_bias = 0.9875;
void kalmanPredict(float accel[3], float dt) {
  for (int i = 0; i < 3; i++) {
    vel[i] += accel[i] * dt;     // Predict velocity for axis i
    pos[i] += vel[i] * dt;       // Predict position for axis i

    // Increase the uncertainty
    pos_error[i] += accel_variance * dt * dt;  // Uncertainty grows over time
  }
}

void kalmanUpdate(float measured_position[3]) {
  for (int i = 0; i < 3; i++) {
    // Calculate Kalman gain
    float kalman_gain = pos_error[i] / (pos_error[i] + measurement_variance);

    // Update estimate with measurement
    pos[i] = pos[i] + kalman_gain * (measured_position[i] - pos[i]);

    // Update the error variance
    pos_error[i] = (1 - kalman_gain) * pos_error[i];
  }
}

void TaskPosition()
{
  while(1)
  {
    if (IMU.accelerationAvailable()) 
    {
    IMU.readAcceleration(accel[0], accel[1], accel[2]);
    accel[0] = accel[0]- accel0_bias;
    accel[1] = accel[1]- accel1_bias;
    accel[2] = accel[2]- accel2_bias;

    // Run the Kalman prediction step for 3D
    kalmanPredict(accel, dt);

    // Placeholder for measured positions (replace with actual measurements if available)
    float measured_position[3] = {pos[0], pos[1], pos[2]};  // No external measurement

    // Run the Kalman update step for 3D
    kalmanUpdate(measured_position);

    // Print estimated position
    Serial.print("Estimated Position: X=");
    Serial.print(pos[0]);
    Serial.print(" Y=");
    Serial.print(pos[1]);
    Serial.print(" Z=");
    Serial.println(pos[2]);
    }
    ThisThread::sleep_for(10ms);
  }
}