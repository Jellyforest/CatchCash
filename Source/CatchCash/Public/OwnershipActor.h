// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "OwnershipActor.generated.h"

UCLASS()
class CATCHCASH_API AOwnershipActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOwnershipActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere,Category=MyActor)
	class UBoxComponent* boxComp;
	UPROPERTY(EditAnywhere,Category=MyActor)
	class UStaticMeshComponent* meshComp;
	
	class UMaterialInstanceDynamic* mat;
	

private:
	UPROPERTY(Replicated)
	FVector moveDirection;

	//반환자료형이 없고, 매개변수가 없는 void 함수에서만 Replicated를 사용할 수 있다
	//위에서 Public으로 함수 선언해주고 그 아래에서 변수를 선언해주면 빨간줄이 안뜬다
	UPROPERTY(ReplicatedUsing = ScreenLog)
	int32 testNumber = 0;

	UPROPERTY(ReplicatedUsing = ChangeColor)
	FVector matColor;


	UFUNCTION()
	FString PrintInfo();

	UFUNCTION()
	void ScreenLog();

	UFUNCTION()
	void CheckOwner();

	UFUNCTION()
	void ChangeColor();

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


};
