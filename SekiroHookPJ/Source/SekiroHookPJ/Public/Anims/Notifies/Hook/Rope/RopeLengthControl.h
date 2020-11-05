#pragma once

#include "CoreMinimal.h"
#include "Anims/Notifies/LerpNotifyStateBaseClass.h"
#include "RopeLengthControl.generated.h"

UCLASS()
class SEKIROHOOKPJ_API URopeLengthControl : public ULerpNotifyStateBaseClass
{
 GENERATED_BODY()
 public: 
	URopeLengthControl(const FObjectInitializer &ObjectInitializer);
	
	virtual void NotifyBegin(USkeletalMeshComponent *MeshComp, 
                           UAnimSequenceBase *Animation, 
                           float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent *MeshComp, 
                          UAnimSequenceBase *Animation, 
                          float FrameDeltaTime) override;
 
 private:
	void UpdateRopeLength(float FrameDeltaTime);	
};
