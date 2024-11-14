/*
#include <mbed.h>
#include <vector>
#include <ArduinoEigenDense.h> // Use the Eigen library for matrix operations

using namespace mbed;
using namespace rtos;

using namespace std::chrono_literals;

extern Queue<bool,16> pressureServiceRXQueue;

using namespace Eigen;

// Constants
const double deltaTime = 0.01; // Time step (100 Hz)
const double gravity = 9.81;   // Gravity constant

// Define state dimensions
const int STATE_SIZE = 6;      // [position, velocity, angle] each in x, y, z directions
const int MEASURE_SIZE = 3;    // Measurement includes [acceleration in x, y, z]

// Initialize Kalman Filter variables
VectorXd x(STATE_SIZE);            // State vector
MatrixXd FT(STATE_SIZE, STATE_SIZE); // State transition matrix
MatrixXd B(STATE_SIZE, 3);          // Control matrix
MatrixXd P(STATE_SIZE, STATE_SIZE); // Error covariance matrix
MatrixXd Q(STATE_SIZE, STATE_SIZE); // Process noise covariance
MatrixXd H(MEASURE_SIZE, STATE_SIZE); // Measurement matrix
MatrixXd R(MEASURE_SIZE, MEASURE_SIZE); // Measurement noise covariance

void initializeKF() {
    // Initialize state (x) and covariance (P)
    x.setZero();
    P = MatrixXd::Identity(STATE_SIZE, STATE_SIZE) * 0.1;

    // Define state transition matrix F (with simple integration model)
    FT.setIdentity();
    FT.block<3, 3>(0, 3) = MatrixXd::Identity(3, 3) * deltaTime; // Position updates with velocity

    // Define control matrix B (acceleration affects velocity directly)
    B.setZero();
    B.block<3, 3>(3, 0) = MatrixXd::Identity(3, 3) * deltaTime;

    // Define measurement matrix H (only position is observed from acceleration)
    H.setZero();
    H.block<3, 3>(0, 0) = MatrixXd::Identity(3, 3);

    // Process noise and measurement noise
    Q = MatrixXd::Identity(STATE_SIZE, STATE_SIZE) * 0.01;
    R = MatrixXd::Identity(MEASURE_SIZE, MEASURE_SIZE) * 0.1;
}

void kalmanFilterUpdate(const Vector3d& accelData, const Vector3d& gyroData) {
    // ** Prediction Step **
    // Predict state and error covariance
    x = FT * x + B * accelData; // Using linear acceleration for position/velocity prediction
    P = FT * P * FT.transpose() + Q;

    // ** Update Step **
    // Measurement from accelerometer (position derived by double integration) and orientation from gyro
    VectorXd z = accelData; // Measurement vector (acceleration directly for simplicity)

    // Calculate Kalman gain
    MatrixXd S = H * P * H.transpose() + R;
    MatrixXd K = P * H.transpose() * S.inverse();

    // Update state estimate and covariance
    x = x + K * (z - H * x);
    P = (MatrixXd::Identity(STATE_SIZE, STATE_SIZE) - K * H) * P;
}

std::vector<Vector3d> acc = {{0.0,0.0,0.0}};
std::vector<Vector3d> gyro = {{0.0,0.0,0.0}};

void TaskPosition() 
{
  int count = 0;
  double x_bias = 0;
  double y_bias = 0;
  double z_bias = 0;
  while (1) {
    if (IMU.accelerationAvailable()) {
      count++;
      IMU.readAcceleration(acc[0][0], acc[0][1], acc[0][2]);
      x_bias += acc[0][0];
      y_bias += acc[0][1];
      z_bias += acc[0][2];
      Serial.println(x_bias/count);
      Serial.println(y_bias/count);
      Serial.println(z_bias/count);
    }
    ThisThread::sleep_for(10ms);
  }
}
*/