#pragma once

#include "CoreMinimal.h"
#include "Anims/Notifies/LerpNotifyStateBaseClass.h"

#include "KunaiThrow.generated.h"

UCLASS()
class SEKIROHOOKPJ_API UKunaiThrow : public ULerpNotifyStateBaseClass
{
 GENERATED_BODY()
 public:
	UKunaiThrow(const FObjectInitializer &ObjectInitializer);

	virtual void NotifyBegin(USkeletalMeshComponent *MeshComp, 
                           UAnimSequenceBase *Animation, 
                           float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent *MeshComp, 
                          UAnimSequenceBase *Animation, 
                          float FrameDeltaTime) override;

	virtual void Rotation(float FrameDeltaTime) override;

 private:
	void Throw(float FrameDeltaTime);
};
