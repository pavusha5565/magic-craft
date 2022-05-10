// Pavlusha5565 copyright.


#include "Player/MCCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"

#include "Interfaces/InteractableInterface.h"

#include "Objects/Trees/InteractableTree.h"

#include "Camera/CameraComponent.h"

#include "DrawDebugHelpers.h"

// Sets default values
AMCCharacter::AMCCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	ArmLength = 2000.f;
}

// Called when the game starts or when spawned
void AMCCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void AMCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &AMCCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AMCCharacter::BeginInterract);
	
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMCCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMCCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}



// Called every frame
void AMCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMCCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMCCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMCCharacter::PrimaryAttack()
{
	/****************************/
	FVector StartTrace = CameraComponent->GetComponentLocation();
	FVector EndTrace = (CameraComponent->GetForwardVector() * 1500.f) + StartTrace;
	DrawDebugLine(
		GetWorld(),
		StartTrace,
		EndTrace,
		FColor(255, 0, 0),
		false,
		1.f,
		0,
		1
	);
	/****************************/

	FHitResult Hit = TraceLine(1500.f);

	AInteractableTree* Tree = Cast<AInteractableTree>(Hit.GetActor());
	UE_LOG(LogTemp, Warning, TEXT("Cast"));
	if (Tree) {
		UE_LOG(LogTemp, Warning, TEXT("Cast Success"));
		Tree->Chop(30);
	}
}

FHitResult AMCCharacter::TraceLine(float Distance)
{
	FHitResult HitResult;
	FVector StartTrace = CameraComponent->GetComponentLocation();
	FVector EndTrace = (CameraComponent->GetForwardVector() * Distance) + StartTrace;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, QueryParams);

	return HitResult;
}

void AMCCharacter::BeginInterract()
{
	/****************************/
	FVector StartTrace = CameraComponent->GetComponentLocation();
	FVector EndTrace = (CameraComponent->GetForwardVector() * ArmLength) + StartTrace;
	DrawDebugLine(
		GetWorld(),
		StartTrace,
		EndTrace,
		FColor(255, 0, 0),
		false,
		1.f,
		0,
		1
	);
	/****************************/

	FHitResult HitResult = TraceLine(ArmLength);

	IInteractableInterface* IActor = Cast<IInteractableInterface>(HitResult.GetActor());

	if (IActor) {
		IInteractableInterface::Execute_Interact(HitResult.GetActor());
	}
}

void AMCCharacter::EndInterract()
{
	IInteractableInterface* IActorFocused = Cast<IInteractableInterface>(FocusedItem);

	if (IActorFocused) {
		IInteractableInterface::Execute_InteractRealesed(FocusedItem);
	}
}

void AMCCharacter::ChechInteractable(float Distance)
{
	FHitResult HitResult = TraceLine(Distance);

	AActor* Actor = HitResult.GetActor();
	IInteractableInterface* IActor = Cast<IInteractableInterface>(Actor);
	IInteractableInterface* IActorFocused = Cast<IInteractableInterface>(FocusedItem);

	if (IActor != nullptr) {
		if (IActorFocused != IActor && Actor != nullptr) {
			if (IActorFocused) {
				IInteractableInterface::Execute_UnfocusItem(FocusedItem);
				IInteractableInterface::Execute_InteractRealesed(FocusedItem);
				FocusedItem = nullptr;
			}
			FocusedItem = Actor;
			IInteractableInterface::Execute_FocusItem(Actor);
		}
		return;
	}
	if (IActorFocused) {
		IInteractableInterface::Execute_UnfocusItem(FocusedItem);
		IInteractableInterface::Execute_InteractRealesed(FocusedItem);
	}
	FocusedItem = nullptr;
}