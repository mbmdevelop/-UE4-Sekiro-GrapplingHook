#pragma once

#include "CoreMinimal.h"
#include "Anims/Notifies/LerpNotifyStateBaseClass.h"

#include "KunaiSwing.generated.h"

UCLASS()
class SEKIROHOOKPJ_API UKunaiSwing : public ULerpNotifyStateBaseClass
{
 GENERATED_BODY()
 public:
	UKunaiSwing(const FObjectInitializer &ObjectInitializer);

	virtual void NotifyBegin(USkeletalMeshComponent *MeshComp, 
                           UAnimSequenceBase *Animation, 
                           float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent *MeshComp, 
                          UAnimSequenceBase *Animation, 
                          float FrameDeltaTime) override;

	virtual void Transform(float FrameDeltaTime) override;
};
