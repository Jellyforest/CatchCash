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


 //��������Ʈ�� F�� �����ؾ��Ѵ�.
//�ε����� ��� sessionslot���� �ѱ� ������ �ϳ� �� ������־��� idx
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnSearchResult, FString, roomName, int32, currentPlayers, int32, maxPlayers, int32, ping, int32, idx);

//�ٽ� Ȱ��ȭ ��Ű�� Ÿ�̹� : ����Ʈ �߰��� ���� ��(�� �߰��� ��� �Ϸ�Ǿ��� ��)
//Refresh ��ư Ȱ��ȭ�� ���� Delegate
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

//��������Ʈ�� F�� �����ؾ��Ѵ�.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSearchResult, FSessionInfo, sessionInfo);

UCLASS()
class CATCHCASH_API UServerGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:	
	UServerGameInstance();
	virtual void Init() override;

	/*���� ����*/
	IOnlineSessionPtr sessionInterface;

	//���� ID�� �־��� �̸��� ������ ������ش�
	FName sessionID;

	//ã������ ���� ������ ����
	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	//BP�� �ƴ� ���� Ŭ������ �����̱� ������ Deligate�� ������ش�
	UPROPERTY(EditAnywhere)
	TSubclassOf<class USessionSlotWidget> sessionSlot;
	FOnSearchResult searchResultDele;

	//��������Ʈ ���� ����
	FOnSerachFinished serachFinishedDelegate;

	/*�Լ�*/
	//���� ������ ���� �� �Ű������� ���� ���� ������ �����״ϱ� �Ű������� ���� �־��ش�.
	void CreateMySession(FString roomName, int32 playerCount);

	UFUNCTION()
	void OnCreateSessionComplete(FName sessionName, bool bIsSuccess);
	
	//���� ����� ��� �Լ��� ���� ���̴�.
	void FindMySession();

	//OnFindSessionsCompleteDelegates���� �޾ƿ��� ������ �˻����־���
	UFUNCTION()
	void OnFindSessionComplete(bool bWasSuccessful);

	//�濡 �� ���̴�.
	//���° ���� �� �������� ���� ������ �ʿ��ϴ�(�� ������ ���� �迭�� �ε����� ���ڷ� ���� ��)
	void JoinMySession(int32 sessionIdx);

	//���� ������ �̺�Ʈ ���� �����ϰ��� �ߴ� ������ �̸��� ��������
	//result Ÿ���� �������� �� ���ο� �����ߴ��� ��� ���θ� enum ���·� ��ȯ���ٰ��̴�
													  //join�Ǵ� ���� �̸� 
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type joinResult);



};
