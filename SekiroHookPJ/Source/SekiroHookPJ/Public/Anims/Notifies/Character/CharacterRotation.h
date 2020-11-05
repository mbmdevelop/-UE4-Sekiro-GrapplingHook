#pragma once

#include "CoreMinimal.h"
#include "Anims/Notifies/LerpNotifyStateBaseClass.h"

#include "CharacterRotation.generated.h"

UCLASS()
class SEKIROHOOKPJ_API UCharacterRotation : public ULerpNotifyStateBaseClass
{
 GENERATED_BODY()
 public:
	UCharacterRotation(const FObjectInitializer & ObjectInitializer);

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, 
                           UAnimSequenceBase * Animation, 
                           float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, 
                          UAnimSequenceBase * Animation, 
                          float FrameDeltaTime) override;

	virtual void Rotation(float FrameDeltaTime) override;
};
