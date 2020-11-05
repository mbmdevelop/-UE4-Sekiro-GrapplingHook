#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "../SekiroHookPJCharacter.h"

#include "CharacterAnimInstance.generated.h"

UCLASS(transient,Blueprintable, hideCategories = AnimInstance, BlueprintType)
class SEKIROHOOKPJ_API UCharacterAnimInstance : public UAnimInstance
{
 GENERATED_BODY()
 public:
  UCharacterAnimInstance();

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
		float speed_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
		bool is_in_air_;

 private:
	UPROPERTY()
	  AActor *owning_actor_;

	UPROPERTY()
	  ASekiroHookPJCharacter *character_;
};
