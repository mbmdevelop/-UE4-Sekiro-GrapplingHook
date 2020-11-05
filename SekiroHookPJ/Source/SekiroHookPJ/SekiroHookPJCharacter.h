#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Engine/DataTable.h"
#include "Components/TimelineComponent.h"

#include "SekiroHookPJCharacter.generated.h"

class APlayerCameraManager;
class UAnimMontage;
class UCableComponent;
class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;

class AHookeableActor;
class AHookeablePoint;

// NOTE: Struct that forms our custom UDataTable with the UAnimationMontage
// to play and their UCurveFloat.
USTRUCT(BlueprintType)
struct FHookJumpMontage : public FTableRowBase
{       
  GENERATED_BODY();

  UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	  UAnimMontage *anim_montage;

	///Curves

	//Character
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Curves")
	  UCurveFloat *height_curve;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")           
	  UCurveFloat *speed_curve;

	//Kunai
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")           
	  UCurveFloat *throw_speed_curve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	  UCurveFloat *pickup_speed_curve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	  UCurveFloat *swing_curve;

	//Rope
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	  UCurveFloat *length_curve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	  UCurveFloat *solver_iterations_curve;
};

UCLASS(config=Game)
class ASekiroHookPJCharacter : public ACharacter
{
 GENERATED_BODY()
 public:
  ASekiroHookPJCharacter();

  UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                        bool bFromSweep, const FHitResult& SweepResult);

  UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return camera_boom_; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return follow_camera_; }
	FORCEINLINE void SetFocusAllowed(bool new_val) { focus_allowed_ = new_val;}
  FORCEINLINE const FTransform& CharacterTransform() const
  { 
    return GetActorTransform(); 
  }
	FORCEINLINE const FTransform& KunaiTransform() const
  { 
    return kunai_->GetComponentTransform(); 
  }
	FORCEINLINE const FTransform& KunaiIdleSocketTransform() const 
  { 
    return GetMesh()->GetSocketTransform("Idle_Kunai_Socket"); 
  }
	FORCEINLINE const FTransform& KunaiSwingSocketTransform() const
  { 
    return GetMesh()->GetSocketTransform("Swing_Kunai_Socket"); 
  }
	FORCEINLINE const FVector& LastFocusedPointTrackerLoc() const
  { 
    return last_focused_point_tracker_loc_;
  }
	FORCEINLINE const FVector& LastFocusedPointLandLoc() const
  { 
    return last_focused_point_land_loc_; 
  }

  void HookThrow();
  void StopHookThrowing();
	bool CheckHookVisibility() const;
	void ChangeHookVisibility(bool new_value);

  void AttachKunai();
	void CheckKunaiCollision(const FVector& next_frame_loc);
	void SetRopeStiffness(bool new_value);

  void SetCharacterMovement(bool new_value);
	void CustomJump();

	void LerpCharacterMovement(const FVector& start_loc, const FVector& end_loc, 
                             float time);
	void LerpCharacterRotation(const FRotator& start_rot, const FRotator& end_rot, 
                             float alpha);
	void LerpKunaiTransform(const FTransform& start_trans, const FTransform& end_trans, 
                          float time);
	void LerpKunaiRotation(const FRotator& start_rot, const FRotator& end_rot, 
                         float alpha);

	void KunaiThrow(const FVector& start_loc, const FVector& end_loc, float time,
                  float next_frame_time);
	void KunaiPickUp(const FVector& start_loc, const FVector& end_loc, float time);

	void UpdateRopeLength(float time);

	UPROPERTY(EditDefaultsOnly, Category = Camera)
	  UCurveFloat* camera_fade_curve_;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
	  float start_camera_fade_height_;

	UPROPERTY(EditDefaultsOnly, Category = Character_Movement)
	  float line_trace_distance_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	  float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
  	float BaseLookUpRate;

 protected:
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(
    class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

 private:
  UFUNCTION()
    void CameraFade();

  void UpdateHookeablePoints();
  bool IsCharacterWithinPointBounds(const AHookeablePoint *hookeable_point) const;
  bool IsCharacterInFrontOfThePoint(const AHookeablePoint *hookeable_point) const;
  bool IsPointVisible(AHookeablePoint *hookeable_point) const;

  float CharacterPointDistance(const AHookeablePoint *hookeable_point) const;	
	void CalcPointFacingRate(AHookeablePoint *hookeable_point, 
							             TMap<int,float>& points_facing_rate) const;
	void CalcBestFacingPointRate(TMap<int,float>& points_facing_rate);
	
  void PlayHookJumpMontage();
	void CanWalkOffMeshLedges();
	void RestartCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components | Hook", 
            meta = (AllowPrivateAccess = "true"))
	  UStaticMeshComponent *kunai_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components| Hook", 
            meta = (AllowPrivateAccess = "true"))
	  UCableComponent *rope_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, 
            meta = (AllowPrivateAccess = "true"))
	  USpringArmComponent* camera_boom_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, 
            meta = (AllowPrivateAccess = "true"))
	  UCameraComponent* follow_camera_;

  UPROPERTY()
    APlayerCameraManager *camera_manager_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, 
            meta = (AllowPrivateAccess = "true"))
	  UDataTable *hook_jump_montages_;
		
	UPROPERTY()
	  TArray<AHookeablePoint*> hookeable_points_;
	
	UPROPERTY()
	  AHookeablePoint *focused_point_;

  UPROPERTY()
    AActor* last_stepped_actor_;

  FHookJumpMontage *current_montage_;

  FTimeline camera_fade_timeline_;

	FVector last_focused_point_tracker_loc_;
	FVector last_focused_point_land_loc_;
  
  FTransform character_init_trans_;

  FVector kunai_hitted_loc_;
  bool kunai_hit_;

  bool can_jump_;
  bool hook_launched_;
  bool focus_allowed_;
  
  float capsule_half_height_;
};