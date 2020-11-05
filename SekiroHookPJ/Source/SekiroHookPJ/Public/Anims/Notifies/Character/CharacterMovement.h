#pragma once

#include "CoreMinimal.h"
#include "Anims/Notifies/LerpNotifyStateBaseClass.h"

#include "CharacterMovement.generated.h"

UCLASS()
class SEKIROHOOKPJ_API UCharacterMovement : public ULerpNotifyStateBaseClass
{
 GENERATED_BODY()
 public:
	UCharacterMovement(const FObjectInitializer & ObjectInitializer);

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, 
                           UAnimSequenceBase * Animation, 
                           float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, 
                          UAnimSequenceBase * Animation, 
                          float FrameDeltaTime) override;

	virtual void Location(float FrameDeltaTime) override;
};
