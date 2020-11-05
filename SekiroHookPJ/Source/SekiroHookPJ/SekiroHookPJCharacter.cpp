#include "SekiroHookPJCharacter.h"

#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "CableComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Actors/HookeableActor.h"
#include "Actors/HookeablePoint.h"
#include "Actors/WalkablePlatforms.h"

constexpr bool g_draw_debug_helpers = false;

constexpr float g_land_height_offset = 5.0f;
constexpr float g_last_max_rate_offset = 0.5e-3f;

ASekiroHookPJCharacter::ASekiroHookPJCharacter()
  : hookeable_points_(),
  focused_point_(nullptr),
  last_stepped_actor_(nullptr),
  current_montage_(nullptr),
  camera_fade_timeline_(),
  last_focused_point_tracker_loc_(FVector::ZeroVector),
  last_focused_point_land_loc_(FVector::ZeroVector),
  character_init_trans_(FTransform::Identity),
  kunai_hitted_loc_(FVector::ZeroVector),
  kunai_hit_(false),
  can_jump_(true),
  hook_launched_(false),
  focus_allowed_(true),
  capsule_half_height_(0.0f)
{
  kunai_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Kunai Mesh"));
  rope_ = CreateDefaultSubobject<UCableComponent>(TEXT("Rope Cable"));
  camera_boom_ = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
  follow_camera_ = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

  kunai_->SetupAttachment(GetMesh(), "Idle_Kunai_Socket");
  rope_->SetupAttachment(GetMesh(), "Start_Rope_Socket");
  camera_boom_->SetupAttachment(RootComponent);
  follow_camera_->SetupAttachment(camera_boom_, USpringArmComponent::SocketName);

  ChangeHookVisibility(false);
  kunai_->SetEnableGravity(false);

  camera_boom_->TargetArmLength = 350.0f;
  camera_boom_->bUsePawnControlRotation = true;

  follow_camera_->bUsePawnControlRotation = false;

  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  BaseTurnRate = 45.f;
  BaseLookUpRate = 45.f;

  GetCharacterMovement()->bOrientRotationToMovement = true;
  GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
  GetCharacterMovement()->JumpZVelocity = 600.f;
  GetCharacterMovement()->AirControl = 0.2f;
  GetCharacterMovement()->PerchRadiusThreshold = 50.0f;

  static ConstructorHelpers::FObjectFinder<UDataTable>hook_jump_montages_object(
    TEXT("DataTable'/Game/Sekiro_Hook_PJ/Characters/UE4_Mannequin/Animations/Hook_Throw/DT_HookJumpMontage.DT_HookJumpMontage'"));

  if (hook_jump_montages_object.Succeeded())
    hook_jump_montages_ = hook_jump_montages_object.Object;
}

void ASekiroHookPJCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
  AActor* OtherActor,
  UPrimitiveComponent* OtherComp,
  int32 OtherBodyIndex,
  bool bFromSweep,
  const FHitResult& SweepResult)
{
  if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
  {
    if (OtherActor->IsA(AHookeablePoint::StaticClass()))
    {
      AHookeablePoint *tmp_hookeable_point = Cast<AHookeablePoint>(OtherActor);

      if (tmp_hookeable_point->IsValidPoint())
        hookeable_points_.AddUnique(tmp_hookeable_point);
    }
  }
}

void ASekiroHookPJCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
  AActor* OtherActor,
  UPrimitiveComponent* OtherComp,
  int32 OtherBodyIndex)
{
  if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
  {
    if (OtherActor->IsA(AHookeablePoint::StaticClass()))
    {
      AHookeablePoint *tmp_hookeable_point = Cast<AHookeablePoint>(OtherActor);

      if (tmp_hookeable_point == focused_point_)
        focused_point_ = nullptr;

      tmp_hookeable_point->HidePoint();
      hookeable_points_.RemoveSingleSwap(tmp_hookeable_point);
    }
  }
}

void ASekiroHookPJCharacter::HookThrow()
{
  if (!hook_launched_)
  {
    if (focused_point_)
    {
      if (focused_point_->IsHookeable())
      {
        hook_launched_ = true;
        focus_allowed_ = false;

        last_focused_point_tracker_loc_ = focused_point_->GetTrackerLoc();
        last_focused_point_land_loc_ = focused_point_->GetLandLoc();
        last_focused_point_land_loc_.Z += g_land_height_offset + capsule_half_height_;

        PlayHookJumpMontage();
        focused_point_->PlayInputAnimation();

        SetCharacterMovement(false);
      }
    }
  }
}

void ASekiroHookPJCharacter::StopHookThrowing()
{
  hook_launched_ = false;
}

bool ASekiroHookPJCharacter::CheckHookVisibility() const
{
  if (kunai_->GetVisibleFlag() || rope_->GetVisibleFlag())
    return true;
  else
    return false;
}

void ASekiroHookPJCharacter::ChangeHookVisibility(bool new_value)
{
  kunai_->SetVisibility(new_value);
  rope_->SetVisibility(new_value);
}

void ASekiroHookPJCharacter::AttachKunai()
{
  kunai_->AttachToComponent(
    GetMesh(),
    FAttachmentTransformRules::SnapToTargetIncludingScale,
    "Idle_Kunai_Socket");
}

void ASekiroHookPJCharacter::CheckKunaiCollision(const FVector& next_frame_loc)
{
  if (kunai_->IsValidLowLevel())
  {
    FCollisionQueryParams trace_params(TEXT("KunaiCollisionTrace"), true);
    trace_params.bReturnPhysicalMaterial = false;

    FHitResult hit_details = FHitResult(ForceInit);

    bool is_hit = GetWorld()->LineTraceSingleByChannel(hit_details,
      kunai_->GetComponentLocation(),
      next_frame_loc,
      ECC_GameTraceChannel2,
      trace_params);

    if (is_hit)
    {
      kunai_hit_ = true;

      FVector min_kunai_bounds = FVector::ZeroVector;
      FVector max_kunai_bounds = FVector::ZeroVector;
      kunai_->GetLocalBounds(min_kunai_bounds, max_kunai_bounds);

      // NOTE: Kunai half width minus the collision loc
      kunai_hitted_loc_ = hit_details.Location - (max_kunai_bounds * 0.5f);
    }
  }
}

void ASekiroHookPJCharacter::SetRopeStiffness(bool new_value)
{
  if (rope_)
    rope_->bEnableStiffness = new_value;
}

void ASekiroHookPJCharacter::SetCharacterMovement(bool new_value)
{
  if (new_value)
  {
    can_jump_ = true;
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
  }
  else
  {
    can_jump_ = false;
    GetCharacterMovement()->MaxWalkSpeed = 0.0f;
    GetCharacterMovement()->RotationRate = FRotator::ZeroRotator;
  }
}

void ASekiroHookPJCharacter::CustomJump()
{
  if (can_jump_)
  {
    Jump();
  }
}

void ASekiroHookPJCharacter::LerpCharacterMovement(const FVector& start_loc,
  const FVector& end_loc,
  float time)
{
  if (current_montage_)
  {
    if (current_montage_->height_curve->IsValidLowLevel() &&
      current_montage_->speed_curve->IsValidLowLevel())
    {

      FVector lerp_loc = UKismetMathLibrary::VLerp(start_loc,
        end_loc, current_montage_->speed_curve->GetFloatValue(time));

      lerp_loc.Z += current_montage_->height_curve->GetFloatValue(time);

      SetActorLocation(lerp_loc);
    }
  }
}

void ASekiroHookPJCharacter::LerpCharacterRotation(const FRotator& start_rot,
  const FRotator& end_rot,
  float alpha)
{
  FRotator lerp_rot = UKismetMathLibrary::RLerp(start_rot, end_rot, alpha, true);

  SetActorRotation(lerp_rot);
}

void ASekiroHookPJCharacter::LerpKunaiTransform(const FTransform& start_trans,
  const FTransform& end_trans,
  float time)
{
  if (current_montage_)
  {
    if (current_montage_->swing_curve->IsValidLowLevel())
    {
      FTransform lerp_transform = UKismetMathLibrary::TLerp(start_trans,
        end_trans, current_montage_->swing_curve->GetFloatValue(time));

      kunai_->SetWorldTransform(lerp_transform);
    }
  }
}

void ASekiroHookPJCharacter::LerpKunaiRotation(const FRotator& start_rot,
  const FRotator& end_rot,
  float alpha)
{
  FRotator lerp_rot = UKismetMathLibrary::RLerp(start_rot, end_rot, alpha, false);

  kunai_->SetWorldRotation(lerp_rot);
}

void ASekiroHookPJCharacter::KunaiThrow(const FVector& start_loc,
  const FVector& end_loc,
  float time,
  float next_frame_time)
{
  if (current_montage_)
  {
    if (current_montage_->throw_speed_curve->IsValidLowLevel())
    {
      if (time != 0.0f)
        kunai_->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

      if (!kunai_hit_)
      {
        FVector lerp_loc = UKismetMathLibrary::VLerp(start_loc, end_loc,
          current_montage_->throw_speed_curve->GetFloatValue(time));

        kunai_->SetWorldLocation(lerp_loc);

        FVector next_frame_lerp_loc = UKismetMathLibrary::VLerp(start_loc,
          end_loc, current_montage_->throw_speed_curve->GetFloatValue(next_frame_time));

        CheckKunaiCollision(next_frame_lerp_loc);
      }
      else
      {
        kunai_->SetWorldLocation(kunai_hitted_loc_);
      }
    }
  }
}

void ASekiroHookPJCharacter::KunaiPickUp(const FVector& start_loc,
  const FVector& end_loc,
  float time)
{
  if (current_montage_)
  {
    kunai_hit_ = false;

    if (current_montage_->pickup_speed_curve->IsValidLowLevel())
    {
      if (time >= 0.25f)
      {
        FAttachmentTransformRules rot_attachment = FAttachmentTransformRules(
          EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget,
          EAttachmentRule::KeepWorld, true);
        kunai_->AttachToComponent(GetMesh(), rot_attachment, "Idle_Kunai_Socket");
      }

      FVector lerp_loc = UKismetMathLibrary::VLerp(start_loc, end_loc,
        current_montage_->pickup_speed_curve->GetFloatValue(time));

      kunai_->SetWorldLocation(lerp_loc);
    }
  }
}

void ASekiroHookPJCharacter::UpdateRopeLength(float time)
{
  if (current_montage_)
  {
    if (current_montage_->length_curve->IsValidLowLevel() &&
      current_montage_->solver_iterations_curve->IsValidLowLevel())
    {
      rope_->CableLength = current_montage_->length_curve->GetFloatValue(time);

      rope_->SolverIterations = (int)current_montage_->solver_iterations_curve->GetFloatValue(time);
    }
  }
}

void ASekiroHookPJCharacter::BeginPlay()
{
  Super::BeginPlay();

  character_init_trans_ = GetActorTransform();
  capsule_half_height_ = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

  camera_manager_ = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
  camera_manager_->ViewPitchMax = 10.0f;

  if (camera_fade_curve_)
  {
    FOnTimelineFloat fade_callback;
    fade_callback.BindUFunction(this, FName("CameraFade"));

    camera_fade_timeline_.AddInterpFloat(camera_fade_curve_, fade_callback);
  }

  GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this,
    &ASekiroHookPJCharacter::OnOverlapBegin);
  GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this,
    &ASekiroHookPJCharacter::OnOverlapEnd);
}

void ASekiroHookPJCharacter::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  UpdateHookeablePoints();
  CanWalkOffMeshLedges();

  camera_fade_timeline_.TickTimeline(DeltaTime);

  if (!camera_fade_timeline_.IsPlaying() &&
    GetActorLocation().Z < start_camera_fade_height_)
  {
    camera_fade_timeline_.PlayFromStart();
  }
}

void ASekiroHookPJCharacter::SetupPlayerInputComponent(
  class UInputComponent* PlayerInputComponent)
{
  check(PlayerInputComponent);

  PlayerInputComponent->BindAction("Jump", IE_Pressed, this,
    &ASekiroHookPJCharacter::CustomJump);
  PlayerInputComponent->BindAction("Jump", IE_Released, this,
    &ACharacter::StopJumping);

  PlayerInputComponent->BindAxis("MoveForward", this,
    &ASekiroHookPJCharacter::MoveForward);
  PlayerInputComponent->BindAxis("MoveRight", this,
    &ASekiroHookPJCharacter::MoveRight);

  PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
  PlayerInputComponent->BindAxis("TurnRate", this,
    &ASekiroHookPJCharacter::TurnAtRate);
  PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
  PlayerInputComponent->BindAxis("LookUpRate", this,
    &ASekiroHookPJCharacter::LookUpAtRate);

  PlayerInputComponent->BindAction("Hook_Launch", IE_Pressed, this,
    &ASekiroHookPJCharacter::HookThrow);
}

void ASekiroHookPJCharacter::MoveForward(float Value)
{
  if ((Controller != NULL) && (Value != 0.0f))
  {
    // find out which way is forward
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    // get forward vector
    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    AddMovementInput(Direction, Value);
  }
}

void ASekiroHookPJCharacter::MoveRight(float Value)
{
  if ((Controller != NULL) && (Value != 0.0f))
  {
    // find out which way is right
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    // get right vector 
    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    // add movement in that direction
    AddMovementInput(Direction, Value);
  }
}

void ASekiroHookPJCharacter::TurnAtRate(float Rate)
{
  // calculate delta for this frame from the rate information
  AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASekiroHookPJCharacter::LookUpAtRate(float Rate)
{
  // calculate delta for this frame from the rate information
  AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASekiroHookPJCharacter::CameraFade()
{
  float fade_value = camera_fade_curve_->GetFloatValue(
    camera_fade_timeline_.GetPlaybackPosition());

  if (fade_value >= 1.0f)
    RestartCharacter();

  camera_manager_->SetManualCameraFade(fade_value, FLinearColor::Black, false);
}

void ASekiroHookPJCharacter::UpdateHookeablePoints()
{
  if (hookeable_points_.Num() >= 1)
  {
    TMap<int, float> points_facing_rate;

    for (AHookeablePoint* hookeable_point : hookeable_points_)
    {
      if (!hookeable_point) return;

      if (IsCharacterWithinPointBounds(hookeable_point))
      {
        hookeable_point->CustomUpdate(CharacterPointDistance(hookeable_point));

        if (IsCharacterInFrontOfThePoint(hookeable_point) &&
            IsPointVisible(hookeable_point))
        {
          CalcPointFacingRate(hookeable_point, points_facing_rate);
        }
      }
      else
      {
        hookeable_point->HidePoint();
      }
    }

    if (focus_allowed_)
    {
      if (points_facing_rate.Num() > 0)
      {
        CalcBestFacingPointRate(points_facing_rate);
      }
      else
      {
        if (focused_point_ != nullptr)
          focused_point_->SetCharacterFocused(false);
        focused_point_ = nullptr;
      }
    }
  }
}

bool ASekiroHookPJCharacter::IsCharacterWithinPointBounds(
  const AHookeablePoint *hookeable_point) const
{
  if (CharacterPointDistance(hookeable_point) <= hookeable_point->max_update_distance_ &&
    CharacterPointDistance(hookeable_point) >= hookeable_point->min_update_distance_)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool ASekiroHookPJCharacter::IsCharacterInFrontOfThePoint(
  const AHookeablePoint *hookeable_point) const
{
  if (hookeable_point->GetActorForwardVector().X * (
     (hookeable_point->GetActorLocation().X + hookeable_point->depth_loc_offset_) - GetActorLocation().X) >= 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool ASekiroHookPJCharacter::IsPointVisible(AHookeablePoint *hookeable_point) const
{
  // NOTE: Explain in CalcBestFacingPoint()
  FVector camera_forward = UKismetMathLibrary::GetForwardVector(
    camera_boom_->GetTargetRotation() + follow_camera_->GetRelativeRotation());

  FCollisionQueryParams trace_params(TEXT("VisibilityHookeablePointTrace"), true);
  trace_params.bReturnPhysicalMaterial = false;

  FHitResult hit_details = FHitResult(ForceInit);

  bool hited = GetWorld()->LineTraceSingleByChannel(hit_details,
    follow_camera_->GetComponentLocation(),
    hookeable_point->GetTrackerLoc(),
    ECC_Visibility, trace_params);

  if (hited)
  {
    if (hit_details.GetActor() == hookeable_point)
    {
      return true;
    }
    else
    { 
      return false;
    }
  }
  else
  {
    return false;
  }
}

float ASekiroHookPJCharacter::CharacterPointDistance(
  const AHookeablePoint *hookeable_point) const
{
  FVector character_to_point = hookeable_point->GetTrackerLoc() - GetActorLocation();

  return character_to_point.Size();
}

void ASekiroHookPJCharacter::CalcPointFacingRate(AHookeablePoint *hookeable_point,
  TMap<int, float>& points_facing_rate) const
{
  // NOTE: Calc the facing rate of the hookeable point doing the dot product between
  //the camera forward vector and the direction unit vector from the
  //camera world loc and the world tracker point loc of the hookeable point. 

  FVector camera_point_direction = UKismetMathLibrary::GetDirectionUnitVector(
    follow_camera_->GetComponentLocation(), hookeable_point->GetTrackerLoc());


  // NOTE: Due to a slight change in the camera rotation when the character
  // changes its direction we make our own camera forward vector getting
  // the rotation applied to the camera by the spring arm and adding the
  // relative offset rotation of the camera.That give us a fixed camera
  // forward.
  FVector camera_forward = UKismetMathLibrary::GetForwardVector(
    camera_boom_->GetTargetRotation() + follow_camera_->GetRelativeRotation());

  float point_facing_rate = FVector::DotProduct(camera_forward,
    camera_point_direction);

  if (point_facing_rate > 0.5f)
    points_facing_rate.Add(hookeable_points_.IndexOfByKey(hookeable_point),
      point_facing_rate);
}

void ASekiroHookPJCharacter::CalcBestFacingPointRate(
  TMap<int, float>& points_facing_rate)
{
  // NOTE: Compare all the facing point rates and set the bigger the new
  // focused point.
  TPair<int, float> max_rate(0, 0.0f);

  for (TPair<int, float>& it : points_facing_rate)
  {
    if (it.Value > max_rate.Value)
      max_rate = it;
  }

  int new_focused_point_index = max_rate.Key;
  static TPair<int, float> last_max_rate(0, 0.0f);

  if (focused_point_)
  {
    if (hookeable_points_[new_focused_point_index] != focused_point_)
    {
      // NOTE: Since the facing_rate of the focused_point is discarded when
      // the focused point becomes non visible we need to make an equal comprobation
      // without the offset since the max_rate and the lat_max rate of the next frame 
      // will be equal.
      if (max_rate.Key == last_max_rate.Key ||
        max_rate.Value >= last_max_rate.Value + g_last_max_rate_offset)
      {
        focused_point_->SetCharacterFocused(false);
        focused_point_ = hookeable_points_[new_focused_point_index];
        focused_point_->SetCharacterFocused(true);
      }
    }
  }
  else
  {
    focused_point_ = hookeable_points_[new_focused_point_index];
    focused_point_->SetCharacterFocused(true);
  }

  last_max_rate = max_rate;
}

void ASekiroHookPJCharacter::PlayHookJumpMontage()
{
  // NOTE: Firstly, select the montage to play depending on the height 
  //difference between the focused point and the character and if the 
  //character is falling or not

  FString montage_name = "";

  if (GetCharacterMovement()->IsFalling())
    montage_name = "AirJump_";
  else
    montage_name = "GroundJump_";

  if (last_focused_point_tracker_loc_.Z - GetActorLocation().Z <= 0)
    montage_name.Append("LowAltitude");
  else
    montage_name.Append("HighAltitude");

  if (hook_jump_montages_->IsValidLowLevel())
  {
    static const FString context_string(TEXT("Hook Jump Montage Context"));
    current_montage_ = hook_jump_montages_->FindRow<FHookJumpMontage>(
      FName(montage_name), context_string, true);

    if (current_montage_)
    {
      PlayAnimMontage(current_montage_->anim_montage);
    }
  }
}

void ASekiroHookPJCharacter::CanWalkOffMeshLedges()
{
  FCollisionQueryParams trace_params(TEXT("WalkOffLedgeTester"), true, this);

  FVector trace_end_offset = FVector(0.0f, 0.0f, -1.0f) * line_trace_distance_;

  FVector trace_start = GetActorLocation();
  trace_start.Z -= capsule_half_height_;
  FVector trace_end = trace_start + trace_end_offset;
  FHitResult trace_hit_details = FHitResult(ForceInit);

  bool trace_hited = GetWorld()->LineTraceSingleByChannel(trace_hit_details,
    trace_start,
    trace_end,
    ECC_WorldStatic,
    trace_params);

  if (trace_hited)
  {
    if (trace_hit_details.Actor->IsA(AWalkablePlatforms::StaticClass()))
    {
      AWalkablePlatforms *tmp_platform = Cast<AWalkablePlatforms>(trace_hit_details.Actor);
      trace_hited = tmp_platform->can_walk_off_ledges_;
    }

    if (trace_hit_details.Actor->IsA(AHookeableActor::StaticClass()))
    {
      AHookeableActor *tmp_hookeable_actor = Cast<AHookeableActor>(trace_hit_details.Actor);
      trace_hited = tmp_hookeable_actor->can_walk_off_ledges_;
    }
  }
  else
  {
    trace_hited = false;
  }

  if (trace_hit_details.Actor.Get() && trace_hit_details.Actor != last_stepped_actor_)
  {
    GetCharacterMovement()->bCanWalkOffLedges = trace_hited;
    last_stepped_actor_ = trace_hit_details.Actor.Get();
  }

  // DEBUG: CanWalkOffLedges
  if (g_draw_debug_helpers)
    if (trace_hited)
      DrawDebugLine(GetWorld(), trace_start, trace_end, FColor::Green, false,
        -1.0f, 0.0f, 2.0f);
    else
      DrawDebugLine(GetWorld(), trace_start, trace_end, FColor::Red,
        false, -1.0f, 0.0f, 2.0f);
}

void ASekiroHookPJCharacter::RestartCharacter()
{
  SetActorTransform(character_init_trans_);
  GetController()->SetControlRotation(FRotator::ZeroRotator);
}