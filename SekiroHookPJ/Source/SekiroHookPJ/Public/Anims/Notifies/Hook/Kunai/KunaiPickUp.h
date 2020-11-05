#pragma once

#include "CoreMinimal.h"
#include "Anims/Notifies/LerpNotifyStateBaseClass.h"

#include "KunaiPickUp.generated.h"

UCLASS()
class SEKIROHOOKPJ_API UKunaiPickUp : public ULerpNotifyStateBaseClass
{
 GENERATED_BODY()
 public:
	UKunaiPickUp(const FObjectInitializer & ObjectInitializer);

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, 
                           UAnimSequenceBase * Animation, 
                           float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, 
                          UAnimSequenceBase * Animation, 
                          float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, 
                         UAnimSequenceBase * Animation) override;

 private:
	void PickUp(float FrameDeltaTime);
};