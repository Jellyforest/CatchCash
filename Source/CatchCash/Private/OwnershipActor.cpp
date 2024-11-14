// Fill out your copyright notice in the Description page of Project Settings.


#include "OwnershipActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "../CatchCashCharacter.h"


// Sets default values
AOwnershipActor::AOwnershipActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicateMovement(true);
	SetReplicates(true);

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(boxComp);
	boxComp->SetBoxExtent(FVector(50));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AOwnershipActor::BeginPlay()
{
	Super::BeginPlay();

	//Mesh�� ������ ����Ÿ�� ���͸��� �ν��Ͻ��� �����Ѵ�
	UMaterialInterface* base_mat = meshComp->GetMaterial(0);
	if (base_mat != nullptr)
	{
		mat = UMaterialInstanceDynamic::Create(base_mat, this);
		meshComp->SetMaterial(0, mat);
	}

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AOwnershipActor::OnOverlap);
	
	/*�������� ������ ��ġ�� �̵��ϵ��� �������ְ� Ŭ���̾�Ʈ������ replicated �� ���̴�*/
	//if(GetLocalRole() == ROLE_Authority) �̷��� �ᵵ �ǰ�, �Ʒ�ó�� �ᵵ �ȴ�
	if (HasAuthority())
	{
		//������ ������ �����Ѵ�. �� �������� �̵���Ű�°� Tick���� �� ���̴�.
		moveDirection = FMath::VRand();
		moveDirection.Z = FMath::Abs(moveDirection.Z); //������ ������ �ʰԲ� ���밪?���� ó��

		//�����϶��� ��ȭ �ִ� ��
		//5�ʿ� �ѹ��� testNumber�� ������ �����Ѵ�.
		FTimerHandle repeatTimer;
		GetWorldTimerManager().SetTimer(repeatTimer, FTimerDelegate::CreateLambda([&]()
			{testNumber = FMath::Rand(); }), 5.0f, true);
	}

	

}

// Called every frame
void AOwnershipActor::Tick(float DeltaTime)
{
	//���� ������ ����Ѵ�
	DrawDebugString(GetWorld(), GetActorLocation(), PrintInfo(), nullptr, FColor::White, 0.0f, true, 1.0f);

	//SetActorLocation(GetActorLocation() + moveDirection * DeltaTime * 100);

	if (HasAuthority())
	{
		CheckOwner();
	}

	//Ŭ���̾�Ʈ������ ���̰� CheckOwner �ۿ��� ����׸� �׷��ش�.
	DrawDebugSphere(GetWorld(), GetActorLocation(), 500.0f, 30, FColor::Emerald, false, 0.0f, 0, 1);
}

FString AOwnershipActor::PrintInfo()
{
#pragma region RandomVector
	FString infoText = FString::Printf(TEXT("%.2f, %.2f, %.2f"), moveDirection.X, moveDirection.Y, moveDirection.Z);

#pragma endregion

	return infoText;
}


void AOwnershipActor::ScreenLog()
{
	//Ŭ���̾�Ʈ ����ŭ ������ �̷������ �αװ� ��µȴ�. ������ Ŭ���̾�Ʈ������ �̷������
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::FromInt(testNumber), true, FVector2D(1.5f));
}

//���� ����ȭ ���� �Լ�(������ ���� ����ϱ� ���� �Լ�)
//Actor �ȿ��� �̹� Override �� �Լ��̱� ������ ����� �������� �ʾƵ� ���⼭ �Լ��� ������ָ� �ȴ�.
void AOwnershipActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//�ʰ� ������� �Ϳ� ���� �̽� �ذ��� ���ؼ�
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//�� Ŭ������ �ִ� �� ������ �������� �����ϰ�ʹ� �� ��
	DOREPLIFETIME(AOwnershipActor, moveDirection);

	DOREPLIFETIME(AOwnershipActor, testNumber);

	//���͸��� ����ȭ
	DOREPLIFETIME(AOwnershipActor, matColor);
}

//������ ���� �ȿ� �÷��̾ �ִٸ�, ���� ����� �÷��̾� ĳ���͸� Owner�� �����Ѵ�.
void AOwnershipActor::CheckOwner()
{
	
	//������ ������ ��
	float maxDistance = 500;
	//Owner�� ������ ��
	ACatchCashCharacter* player = nullptr; //���� �ʱ�ȭ

	for (TActorIterator<ACatchCashCharacter> it(GetWorld()); it; ++it)
	{
		float distance = this->GetDistanceTo(*it);

		if (distance < maxDistance)
		{
			maxDistance = distance;
			player = *it;
			//���� ����� �ְ� �� ���� ���̴�.
			//���� �ȿ����̰� �־ ��� ����� ���̴�
		}
	}
	//player�� null�� ���·� ������ �ʵ��� �������
	if (player != nullptr && GetOwner() != nullptr)
	{
		//Owner�� �����ض�
		SetOwner(player);
	}
	
}

void AOwnershipActor::ChangeColor()
{
	//"MyColor" �Ķ���� ���� matColor ������ �����Ѵ�.
	mat->SetVectorParameterValue(FName("MyColor"), (FLinearColor)matColor);
}

void AOwnershipActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (HasAuthority())
	{
		//�浹�� ����� �÷��̾���
		if (OtherActor->IsA<ACatchCashCharacter>())
		{
			matColor = FMath::VRand();
			matColor = matColor.GetAbs(); //���밪�� �Կ� ����� ���� �� �ֵ��� �Ѵ�
		}
	}
	

}
