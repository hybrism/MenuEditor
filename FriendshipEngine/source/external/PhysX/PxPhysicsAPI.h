// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2023 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef PX_PHYSICS_API_H
#define PX_PHYSICS_API_H
/** \addtogroup myPhysics
@{
*/

/**
This is the main include header for the Physics SDK, for users who
want to use a single #include file.

Alternatively, one can instead directly #include a subset of the below files.
*/

// Foundation SDK 
#include "PhysX\foundation/Px.h"
#include "PhysX\foundation/PxAlignedMalloc.h"
#include "PhysX\foundation/PxAlloca.h"
#include "PhysX\foundation/PxAllocatorCallback.h"
#include "PhysX\foundation/PxArray.h"
#include "PhysX\foundation/PxAssert.h"
#include "PhysX\foundation/PxAtomic.h"
#include "PhysX\foundation/PxBasicTemplates.h"
#include "PhysX\foundation/PxBitAndData.h"
#include "PhysX\foundation/PxBitMap.h"
#include "PhysX\foundation/PxBitUtils.h"
#include "PhysX\foundation/PxBounds3.h"
#include "PhysX\foundation/PxBroadcast.h"
#include "PhysX\foundation/PxErrorCallback.h"
#include "PhysX\foundation/PxErrors.h"
#include "PhysX\foundation/PxFlags.h"
#include "PhysX\foundation/PxFoundation.h"
#include "PhysX\foundation/PxFoundationConfig.h"
#include "PhysX\foundation/PxFPU.h"
#include "PhysX\foundation/PxHash.h"
#include "PhysX\foundation/PxHashMap.h"
#include "PhysX\foundation/PxHashSet.h"
#include "PhysX\foundation/PxInlineAllocator.h"
#include "PhysX\foundation/PxInlineArray.h"
#include "PhysX\foundation/PxIntrinsics.h"
#include "PhysX\foundation/PxIO.h"
#include "PhysX\foundation/PxMat33.h"
#include "PhysX\foundation/PxMat44.h"
#include "PhysX\foundation/PxMath.h"
#include "PhysX\foundation/PxMathIntrinsics.h"
#include "PhysX\foundation/PxMathUtils.h"
#include "PhysX\foundation/PxMemory.h"
#include "PhysX\foundation/PxMutex.h"
#include "PhysX\foundation/PxPhysicsVersion.h"
#include "PhysX\foundation/PxPlane.h"
#include "PhysX\foundation/PxPool.h"
#include "PhysX\foundation/PxPreprocessor.h"
#include "PhysX\foundation/PxProfiler.h"
#include "PhysX\foundation/PxQuat.h"
#include "PhysX\foundation/PxSimpleTypes.h"
#include "PhysX\foundation/PxSList.h"
#include "PhysX\foundation/PxSocket.h"
#include "PhysX\foundation/PxSort.h"
#include "PhysX\foundation/PxStrideIterator.h"
#include "PhysX\foundation/PxString.h"
#include "PhysX\foundation/PxSync.h"
#include "PhysX\foundation/PxTempAllocator.h"
#include "PhysX\foundation/PxThread.h"
#include "PhysX\foundation/PxTime.h"
#include "PhysX\foundation/PxTransform.h"
#include "PhysX\foundation/PxUnionCast.h"
#include "PhysX\foundation/PxUserAllocated.h"
#include "PhysX\foundation/PxUtilities.h"
#include "PhysX\foundation/PxVec2.h"
#include "PhysX\foundation/PxVec3.h"
#include "PhysX\foundation/PxVec4.h"
#include "PhysX\foundation/PxVecMath.h"
#include "PhysX\foundation/PxVecQuat.h"
#include "PhysX\foundation/PxVecTransform.h"


//Not myPhysics specific utilities and common code
#include "common/PxCoreUtilityTypes.h"
#include "common/PxPhysXCommonConfig.h"
#include "common/PxRenderBuffer.h"
#include "common/PxBase.h"
#include "common/PxTolerancesScale.h"
#include "common/PxTypeInfo.h"
#include "common/PxStringTable.h"
#include "common/PxSerializer.h"
#include "common/PxMetaData.h"
#include "common/PxMetaDataFlags.h"
#include "common/PxSerialFramework.h"
#include "common/PxInsertionCallback.h"

//Task Manager
#include "task/PxTask.h"

// Cuda Mananger
#if PX_SUPPORT_GPU_PHYSX
#include "gpu/PxGpu.h"
#endif

//Geometry Library
#include "geometry/PxBoxGeometry.h"
#include "geometry/PxBVH.h"
#include "geometry/PxBVHBuildStrategy.h"
#include "geometry/PxCapsuleGeometry.h"
#include "geometry/PxConvexMesh.h"
#include "geometry/PxConvexMeshGeometry.h"
#include "geometry/PxGeometry.h"
#include "geometry/PxGeometryHelpers.h"
#include "geometry/PxGeometryQuery.h"
#include "geometry/PxHeightField.h"
#include "geometry/PxHeightFieldDesc.h"
#include "geometry/PxHeightFieldFlag.h"
#include "geometry/PxHeightFieldGeometry.h"
#include "geometry/PxHeightFieldSample.h"
#include "geometry/PxMeshQuery.h"
#include "geometry/PxMeshScale.h"
#include "geometry/PxPlaneGeometry.h"
#include "geometry/PxSimpleTriangleMesh.h"
#include "geometry/PxSphereGeometry.h"
#include "geometry/PxTriangle.h"
#include "geometry/PxTriangleMesh.h"
#include "geometry/PxTriangleMeshGeometry.h"
#include "geometry/PxTetrahedron.h"
#include "geometry/PxTetrahedronMesh.h"
#include "geometry/PxTetrahedronMeshGeometry.h"

// PhysX Core SDK
#include "PxActor.h"
#include "PxAggregate.h"
#include "PxArticulationReducedCoordinate.h"
#include "PxArticulationJointReducedCoordinate.h"
#include "PxArticulationLink.h"
#include "PxClient.h"
#include "PxConeLimitedConstraint.h"
#include "PxConstraint.h"
#include "PxConstraintDesc.h"
#include "PxContact.h"
#include "PxContactModifyCallback.h"
#include "PxDeletionListener.h"
#include "PxFEMSoftBodyMaterial.h"
#include "PxFiltering.h"
#include "PxForceMode.h"
#include "PxLockedData.h"
#include "PxMaterial.h"
#include "PxParticleBuffer.h"
#include "PxParticleSystem.h"
#include "PxPBDParticleSystem.h"
#include "PxPBDMaterial.h"
#include "PxPhysics.h"
#include "PxPhysXConfig.h"
#include "PxQueryFiltering.h"
#include "PxQueryReport.h"
#include "PxRigidActor.h"
#include "PxRigidBody.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"
#include "PxScene.h"
#include "PxSceneDesc.h"
#include "PxSceneLock.h"
#include "PxShape.h"
#include "PxSimulationEventCallback.h"
#include "PxSimulationStatistics.h"
#include "PxSoftBody.h"
#include "PxVisualizationParameter.h"
#include "PxPruningStructure.h"
#if PX_ENABLE_FEATURES_UNDER_CONSTRUCTION
#include "PxFEMCloth.h"
#include "PxFEMClothMaterial.h"
#include "PxFLIPParticleSystem.h"
#include "PxFLIPMaterial.h"
#include "PxHairSystem.h"
#include "PxMPMMaterial.h"
#include "PxMPMParticleSystem.h"
#endif

//Character Controller
#include "characterkinematic/PxBoxController.h"
#include "characterkinematic/PxCapsuleController.h"
#include "characterkinematic/PxController.h"
#include "characterkinematic/PxControllerBehavior.h"
#include "characterkinematic/PxControllerManager.h"
#include "characterkinematic/PxControllerObstacles.h"
#include "characterkinematic/PxExtended.h"

//Cooking (data preprocessing)
#include "cooking/Pxc.h"
#include "cooking/PxConvexMeshDesc.h"
#include "cooking/PxCooking.h"
#include "cooking/PxTriangleMeshDesc.h"
#include "cooking/PxBVH33MidphaseDesc.h"
#include "cooking/PxBVH34MidphaseDesc.h"
#include "cooking/PxMidphaseDesc.h"

//Extensions to the SDK
#include "extensions/PxDefaultStreams.h"
#include "extensions/PxExtensionsAPI.h"

//Serialization
#include "extensions/PxSerialization.h"
#include "extensions/PxBinaryConverter.h"
#include "extensions/PxRepXSerializer.h"

//Vehicle Simulation
#include "vehicle2/PxVehicleAPI.h"
#include "vehicle/PxVehicleComponents.h"
#include "vehicle/PxVehicleDrive.h"
#include "vehicle/PxVehicleDrive4W.h"
#include "vehicle/PxVehicleDriveTank.h"
#include "vehicle/PxVehicleSDK.h"
#include "vehicle/PxVehicleShaders.h"
#include "vehicle/PxVehicleTireFriction.h"
#include "vehicle/PxVehicleUpdate.h"
#include "vehicle/PxVehicleUtil.h"
#include "vehicle/PxVehicleUtilControl.h"
#include "vehicle/PxVehicleUtilSetup.h"
#include "vehicle/PxVehicleUtilTelemetry.h"
#include "vehicle/PxVehicleWheels.h"
#include "vehicle/PxVehicleNoDrive.h"
#include "vehicle/PxVehicleDriveNW.h"

//Connecting the SDK to Visual Debugger
#include "pvd/PxPvdSceneClient.h"
#include "pvd/PxPvd.h"
#include "pvd/PxPvdTransport.h"
/** @} */
#endif
