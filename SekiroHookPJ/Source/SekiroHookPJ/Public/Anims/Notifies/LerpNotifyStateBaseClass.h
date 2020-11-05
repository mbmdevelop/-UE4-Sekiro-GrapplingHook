#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "LerpNotifyStateBaseClass.generated.h"

 class ASekiroHookPJCharacter;

UCLASS()
class SEKIROHOOKPJ_API ULerpNotifyStateBaseClass : public UAnimNotifyState
{
 GENERATED_BODY()
 public:
	ULerpNotifyStateBaseClass(const FObjectInitializer & ObjectInitializer);

  virtual void Transform(float FrameDeltaTime) PURE_VIRTUAL(
      ULerpNotifyStateBaseClass::Transform(float FrameDeltaTime), ;);
  virtual void Location(float FrameDeltaTime) PURE_VIRTUAL(
      ULerpNotifyStateBaseClass::Location(float FrameDeltaTime), ;);
  virtual void Rotation(float FrameDeltaTime) PURE_VIRTUAL(
      ULerpNotifyStateBaseClass::Rotation(float FrameDeltaTime), ;);

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, 
                           UAnimSequenceBase * Animation, 
                           float TotalDuration) override;
	
 protected:
	UPROPERTY()
		ASekiroHookPJCharacter *character_;

	float notify_total_time_;

	FVector start_loc_;
	FVector end_loc_;
	float current_movement_time_;

	FRotator start_rot_;
	FRotator end_rot_;
	float current_rotation_time_;	
};
