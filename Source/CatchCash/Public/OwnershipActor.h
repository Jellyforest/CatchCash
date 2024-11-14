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

	//��ȯ�ڷ����� ����, �Ű������� ���� void �Լ������� Replicated�� ����� �� �ִ�
	//������ Public���� �Լ� �������ְ� �� �Ʒ����� ������ �������ָ� �������� �ȶ��
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
