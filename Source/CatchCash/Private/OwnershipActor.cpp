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

	//Mesh의 재질을 다이타믹 머터리얼 인스턴스로 변경한다
	UMaterialInterface* base_mat = meshComp->GetMaterial(0);
	if (base_mat != nullptr)
	{
		mat = UMaterialInstanceDynamic::Create(base_mat, this);
		meshComp->SetMaterial(0, mat);
	}

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AOwnershipActor::OnOverlap);
	
	/*서버에서 랜덤한 위치로 이동하도록 설정해주고 클라이언트에서는 replicated 할 것이다*/
	//if(GetLocalRole() == ROLE_Authority) 이렇게 써도 되고, 아래처럼 써도 된다
	if (HasAuthority())
	{
		//랜덤한 방향을 설정한다. 그 방향으로 이동시키는걸 Tick에서 할 것이다.
		moveDirection = FMath::VRand();
		moveDirection.Z = FMath::Abs(moveDirection.Z); //땅으로 꺼지지 않게끔 절대값?으로 처리

		//서버일때만 변화 주는 것
		//5초에 한번씩 testNumber의 변수를 변경한다.
		FTimerHandle repeatTimer;
		GetWorldTimerManager().SetTimer(repeatTimer, FTimerDelegate::CreateLambda([&]()
			{testNumber = FMath::Rand(); }), 5.0f, true);
	}

	

}

// Called every frame
void AOwnershipActor::Tick(float DeltaTime)
{
	//상태 정보를 출력한다
	DrawDebugString(GetWorld(), GetActorLocation(), PrintInfo(), nullptr, FColor::White, 0.0f, true, 1.0f);

	//SetActorLocation(GetActorLocation() + moveDirection * DeltaTime * 100);

	if (HasAuthority())
	{
		CheckOwner();
	}

	//클라이언트에서도 보이게 CheckOwner 밖에서 디버그를 그려준다.
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
	//클라이언트 수만큼 복제가 이루어지고 로그가 출력된다. 복제는 클라이언트에서만 이루어진다
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::FromInt(testNumber), true, FVector2D(1.5f));
}

//서버 동기화 변수 함수(서버에 복제 등록하기 위한 함수)
//Actor 안에서 이미 Override 된 함수이기 때문에 헤더에 선언하지 않아도 여기서 함수를 사용해주면 된다.
void AOwnershipActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//늦게 따라오는 것에 대한 이슈 해결을 위해서
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//이 클래스에 있는 이 변수를 서버에서 복사하고싶다 는 뜻
	DOREPLIFETIME(AOwnershipActor, moveDirection);

	DOREPLIFETIME(AOwnershipActor, testNumber);

	//머터리얼 동기화
	DOREPLIFETIME(AOwnershipActor, matColor);
}

//지정된 범위 안에 플레이어가 있다면, 가장 가까운 플레이어 캐릭터를 Owner로 설정한다.
void AOwnershipActor::CheckOwner()
{
	
	//지정된 범위의 값
	float maxDistance = 500;
	//Owner을 설정할 때
	ACatchCashCharacter* player = nullptr; //값을 초기화

	for (TActorIterator<ACatchCashCharacter> it(GetWorld()); it; ++it)
	{
		float distance = this->GetDistanceTo(*it);

		if (distance < maxDistance)
		{
			maxDistance = distance;
			player = *it;
			//가장 가까운 애가 들어가 있을 것이다.
			//내가 안움직이고 있어도 계속 계산할 것이다
		}
	}
	//player가 null인 상태로 들어오지 않도록 해줘야함
	if (player != nullptr && GetOwner() != nullptr)
	{
		//Owner을 지정해라
		SetOwner(player);
	}
	
}

void AOwnershipActor::ChangeColor()
{
	//"MyColor" 파라미터 값을 matColor 값으로 변경한다.
	mat->SetVectorParameterValue(FName("MyColor"), (FLinearColor)matColor);
}

void AOwnershipActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (HasAuthority())
	{
		//충돌한 대상이 플레이어라면
		if (OtherActor->IsA<ACatchCashCharacter>())
		{
			matColor = FMath::VRand();
			matColor = matColor.GetAbs(); //절대값을 먹여 양수만 나올 수 있도록 한다
		}
	}
	

}
