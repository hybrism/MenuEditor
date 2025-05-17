#pragma once
#include "pch.h"
#include "PlayerConstants.h"


// Graphics
float PlayerConstants::meshHeight = cameraHeight + 10.0f/* + 40.0f*/;
float PlayerConstants::meshCrouchHeight = cameraCrouchHeight + 10.0f/* + 40.0f*/;

// Ground Movement
float PlayerConstants::groundMaxSpeed = 15.0f; // (x * 6) / 10 = m/s, 10 * m/s / 6 = cm/frame,			7.25, 8.7, 11.6
float PlayerConstants::airMaxSpeed = groundMaxSpeed * 1.25f;
float PlayerConstants::godSpeedMultiplier = 1.1f;
float PlayerConstants::godSpeedThreshhold = groundMaxSpeed * 0.7f;



//float PlayerConstants::godSpeedTimer = 0.f;
float PlayerConstants::accelerationSpeed = 2.625f; // groundMaxSpeed * 1.5f / 6.0f * 0.7f
float PlayerConstants::jumpSpeed = 15.0f;
float PlayerConstants::friction = accelerationSpeed * 0.85f;
float PlayerConstants::groundedMaxSpeedCorrectionAmount = groundMaxSpeed / 5.0f;
float PlayerConstants::crouchSpeedMultiplier = 0.5f;
//float PlayerConstants::coyoteJumpTimer = 0.0f;
float PlayerConstants::coyoteJumpMaxTime = 0.3f;

// Air Movement
float PlayerConstants::gravity = -45.0f;
float PlayerConstants::airFriction = accelerationSpeed * 0.05f;
float PlayerConstants::airMobilityMultiplier = 0.2f;

// Wall run
float PlayerConstants::wallRunWallDetectionMagnetSpeed = 1.0f;
float PlayerConstants::wallRunRaycastLength = 100.0f;
float PlayerConstants::wallRunDetectionRaycastLength = 200.0f;
float PlayerConstants::minWallRunSpeed = groundMaxSpeed * 0.25f;
float PlayerConstants::wallRunFriction = accelerationSpeed * 0.95f;
//float PlayerConstants::wallRunFriction = 0.95f;
float PlayerConstants::wallJumpSpeed = 2.0f;
float PlayerConstants::wallJumpHeight = 18.0f;
float PlayerConstants::wallJumpOutSpeed = 10.0f; //Amount players is pushed away from wall when jumping during wallrun
float PlayerConstants::wallRunGravityMultiplier = 0.15f;
float PlayerConstants::wallRunFallDamping = 0.5f;

// Slide
float PlayerConstants::slideSpeedBoost = 5.26f;
float PlayerConstants::slideFriction = 0.440f;
float PlayerConstants::slideSpeedThreshold = groundMaxSpeed / 2.0f;
//float PlayerConstants::slideTimer = 0;
float PlayerConstants::slideTimeUntilFriction = 0.15f;


// Vault
float PlayerConstants::vaultRange = 35.0f;

// Camera
float PlayerConstants::cameraHeight = 160.0f/* + 40.0f*/;
float PlayerConstants::cameraCrouchHeight = 90.0f/* + 40.0f*/;

