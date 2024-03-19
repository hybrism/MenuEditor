#pragma once
#include "pch.h"
#include "PlayerConstants.h"


// Graphics
float PlayerConstants::meshHeight = cameraHeight + 10.0f/* + 40.0f*/;
float PlayerConstants::meshCrouchHeight = cameraCrouchHeight + 10.0f/* + 40.0f*/;

// Ground Movement
float PlayerConstants::maxSpeed = 15.0f; // (x * 6) / 10 = m/s, 10 * m/s / 6 = cm/frame,			7.25, 8.7, 11.6
float PlayerConstants::accelerationSpeed = 2.625f; // maxSpeed * 1.5f / 6.0f * 0.7f
float PlayerConstants::jumpSpeed = 15.0f;
float PlayerConstants::friction = accelerationSpeed * 0.85f;
float PlayerConstants::groundedMaxSpeedCorrectionAmount = maxSpeed / 5.0f;
float PlayerConstants::crouchSpeedMultiplier = 0.5f;

// Air Movement
float PlayerConstants::gravity = -45.0f;
float PlayerConstants::airFriction = accelerationSpeed * 0.05f;
float PlayerConstants::airMobilityMultiplier = 0.2f;

// Wall run
float PlayerConstants::wallRunWallDetectionMagnetSpeed = 1.0f;
float PlayerConstants::wallRunRaycastLength = 100.0f;
float PlayerConstants::wallRunDetectionRaycastLength = 200.0f;
float PlayerConstants::minWallRunSpeed = maxSpeed * 0.25f;
float PlayerConstants::wallRunFriction = accelerationSpeed * 0.95f;
float PlayerConstants::wallJumpSpeed = 7.5f;
float PlayerConstants::wallJumpHeight = 17.5f;
float PlayerConstants::wallRunGravityMultiplier = 0.15f;

// Slide
float PlayerConstants::slideSpeedBoost = 10.0f;
float PlayerConstants::slideFriction = 1.0f;
float PlayerConstants::slideSpeedThreshold = maxSpeed / 2.0f;

// Vault
float PlayerConstants::vaultRange = 35.0f;

// Camera
float PlayerConstants::cameraHeight = 160.0f/* + 40.0f*/;
float PlayerConstants::cameraCrouchHeight = 90.0f/* + 40.0f*/;

