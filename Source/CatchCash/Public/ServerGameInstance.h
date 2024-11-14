// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "ServerGameInstance.generated.h"

/**
 * 
 */


 //델리게이트는 F로 시작해야한다.
//인덱스를 담아 sessionslot으로 넘길 변수를 하나 더 만들어주었다 idx
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnSearchResult, FString, roomName, int32, currentPlayers, int32, maxPlayers, int32, ping, int32, idx);

//다시 활성화 시키는 타이밍 : 리스트 추가가 끝난 후(방 추가가 모두 완료되었을 때)
//Refresh 버튼 활성화를 위한 Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSerachFinished);

USTRUCT()
struct FSessionInfo
{
	GENERATED_BODY()
	FString roomName;
	int32 currentPlayers;
	int32 maxPlayers;
	int32 ping;
	int32 idx;
};

//델리게이트는 F로 시작해야한다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSearchResult, FSessionInfo, sessionInfo);

UCLASS()
class CATCHCASH_API UServerGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:	
	UServerGameInstance();
	virtual void Init() override;

	/*전역 변수*/
	IOnlineSessionPtr sessionInterface;

	//세션 ID에 넣어줄 이름을 변수로 만들어준다
	FName sessionID;

	//찾으려는 세션 쿼리를 생성
	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	//BP가 아닌 순수 클래스의 형태이기 때문에 Deligate를 만들어준다
	UPROPERTY(EditAnywhere)
	TSubclassOf<class USessionSlotWidget> sessionSlot;
	FOnSearchResult searchResultDele;

	//델리게이트 변수 생성
	FOnSerachFinished serachFinishedDelegate;

	/*함수*/
	//세션 정보를 만들 때 매개변수를 통해 세션 정보를 만들테니까 매개변수에 값을 넣어준다.
	void CreateMySession(FString roomName, int32 playerCount);

	UFUNCTION()
	void OnCreateSessionComplete(FName sessionName, bool bIsSuccess);
	
	//세션 목록을 띄울 함수를 만들 것이다.
	void FindMySession();

	//OnFindSessionsCompleteDelegates에서 받아오는 변수를 검색해주었다
	UFUNCTION()
	void OnFindSessionComplete(bool bWasSuccessful);

	//방에 들어갈 것이다.
	//몇번째 방을 들어갈 것인지에 대한 정보가 필요하다(방 정보에 대한 배열의 인덱스를 인자로 받을 것)
	void JoinMySession(int32 sessionIdx);

	//실제 들어오는 이벤트 내가 조인하고자 했던 세션의 이름을 가져오고
	//result 타입은 조인했을 때 조인에 성공했는지 결과 여부를 enum 형태로 반환해줄것이다
													  //join되는 세션 이름 
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type joinResult);



};
