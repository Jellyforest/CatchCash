// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Kismet/GameplayStatics.h"

//������ �ϳ� ������ָ� ����
UServerGameInstance::UServerGameInstance()
{
	sessionID = "Test Session";
}

//��Ÿ�ӿ� ���� ����Ǵ� �Լ�
void UServerGameInstance::Init()
{
	Super::Init();
	//������ ������ ���̴�.

	

	//�¶��� ���� ����� �����Ǿ��ִ� IOnlineSubsystem Ŭ������ �����´�.
	IOnlineSubsystem* subsys = IOnlineSubsystem::Get();
	if (subsys)
	{

		//��������Ʈ ����Ѵ�. ��������Ʈ�� ���ؼ� �������� ������ �޾ƿ� ���̴�.
		sessionInterface = subsys->GetSessionInterface();
		if (sessionInterface != nullptr)
		{
			sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UServerGameInstance::OnCreateSessionComplete);

			//��������Ʈ �Լ��̹Ƿ� UFUNCTION ���ش�
			//�񵿱�׼�(�������ӿ� ������ �׼��� �ƴϴ�. ��û�ð��� ������ ������ �ð��� �������� ���� �� ����ϴ� �ܾ�)
			//Find ���� �� ���� ���� �˻��� �Ϸ�Ǹ� true, �׷��� ������ false�� �������ְڴ�.
			sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UServerGameInstance::OnFindSessionComplete);

			//���ǿ� �շ��ߴٰ� ó���ϴ� delegate�Լ�
			sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UServerGameInstance::OnJoinSessionComplete);
		}

	}

	//���� �÷����� �̸��� ����Ѵ�
	FString platformName = subsys->GetSubsystemName().ToString();
	UE_LOG(LogTemp, Warning, TEXT("PlatformName : %s"), *platformName);
}

//���� ���� �Լ�
void UServerGameInstance::CreateMySession(FString roomName, int32 playerCount)
{
	if (sessionInterface != nullptr)
	{
		//������ �����
		FOnlineSessionSettings sessionSettings;

		//�ʴ밡 �����Ѱ�
		sessionSettings.bAllowInvites = true;

		//������ ���� �߿��� ������ �����Ѱ�?
		sessionSettings.bAllowJoinInProgress = true;

		//���� �߿��� �ܺ����� ���� �� �ֵ��� �� ���ΰ�
		sessionSettings.bAllowJoinViaPresence = true;
		sessionSettings.bIsDedicated = false; //�츮�� Lan����� ����Ұ��ΰ�

		//�¶��� ����ý����� �̸��� "NULL"�̸� LAN ��Ī�� �ϰ�, "Steam"�̸� ���� ������ ��Ī�Ѵ�.
		sessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
		//���� ���� ���ǿ� ������ �� �ִ� �ִ��� ���� (���� ������ �����̴�, �ּ� 2)
		//������ ���� �޾ƿ����� �Ű����� �̸��� �־��־���.
		sessionSettings.NumPublicConnections = playerCount;

		//�� �̸� �����Ѵ�
		sessionSettings.Set(FName("KEY_RoomName"), roomName);
		//������ �̸��� �˻��� �� �ְ� �� ������
		sessionSettings.bShouldAdvertise = true;


		//������ �����Ѵ�
		sessionInterface->CreateSession(0, sessionID, sessionSettings);

		UE_LOG(LogTemp, Warning, TEXT("Create Session Try"));
	}
	else
	{

	}
}

//������ ��������� �� ȣ��� �Լ�
void UServerGameInstance::OnCreateSessionComplete(FName sessionName, bool bIsSuccess)
{
	//                               true�� ��  �̰� �� ��             false�� �� �̰� �� ��
	FString result = bIsSuccess ? TEXT("Create Session Success!") : TEXT("Create Session Failed...");
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s = %s"), *result, *sessionName.ToString()));
	UE_LOG(LogTemp, Warning, TEXT("%s = %s"), *result, *sessionName.ToString());

	//���� ������ �����ߴٸ�, ������ ������ ���ǿ� ������ ��� �ο��� �̵���Ų��.
	if (bIsSuccess)
	{
		//������ ����������� ������ ������ ��� �ο��� �̵��Ұ��̴� ��� ��
		GetWorld()->ServerTravel("/Game/JY/Maps/MainMap?Listen");
	}

	
}

//��������ִ� ������ ã�� �Լ�
void UServerGameInstance::FindMySession()
{
	//ã������ ���� ������ �����Ѵ�
	//native c++ �Լ��� ��ȯ�ϱ� ���� �۾�
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	//LanQuery�� null�̸� true�� �Ǿ lanquery�� �����Ѵ�. false�� "STEAM"�̰� STEAM���� �����϶� 
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	sessionSearch->MaxSearchResults = 30;
	//Ư�� Ű�� �˻� ������ ���͸��ϰ���� ���߰��ϴ� ����
	//sessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, roomName, EOnlineComparisonOp::GreaterThanEquals);

	//Presense�� ������ ������ ���͸��ϰ���� ��
	//SEARCH_PRESENCE : ������ �ɾ��ִ� ��ũ�θ� ����� ���̴�. ���� �⺻���� ���� ���ǿ� ������ �ؾ��Ѵ�.
	//value�� true : ���� ������ �ִ� �����ϰ� �����ѰŸ� ã�´�
	//false : ���� �������� ���� ������ �˻��ϵ��� ������´�
	//���� �����ִ� ������ ��ġ�� ��쿡�� �˻��� �ߵ��� �ϸ� �Ǳ� ������ Equal�� �����־���.
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	//�տ��� ���� ������ �̿��ؼ� ������ ã�´�.       sessionSearch�� ������ �����ε� ������ ���·� ĳ�������־���.
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());

}

//���� �˻��� ������ �� ȣ��� �Լ�
void UServerGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	
	if (bWasSuccessful) //���⼭ �ؾ��Ұ� ����
	{
		if (sessionSearch)
		{

			//�� ��� ���� ã�������� ����� ����
			TArray<FOnlineSessionSearchResult> searchResults = sessionSearch->SearchResults;
			UE_LOG(LogTemp, Warning, TEXT("Find Session count = %d"), searchResults.Num());
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Find Session count = %d"), searchResults.Num()));

			//�˻� ������� ��� ��ȸ�Ѵ�.
			for (int32 i = 0; i < searchResults.Num(); i++)
			{
				FSessionInfo searchSessionInfo;
				FString roomName;

				searchResults[i].Session.SessionSettings.Get(FName("KEY_RoomName"), roomName);
				searchSessionInfo.roomName = roomName;

				//maxPlayer�� �� ��������
				searchSessionInfo.maxPlayers = searchResults[i].Session.SessionSettings.NumPublicConnections;
				//���� �÷��̾��� �� ��������
				searchSessionInfo.currentPlayers = searchSessionInfo.maxPlayers - searchResults[i].Session.NumOpenPublicConnections;
				searchSessionInfo.ping = searchResults[i].PingInMs;

				searchSessionInfo.idx = i;

				//���Կ� �ʿ��� ������ �̺�Ʈ�� �����Ѵ�.
				searchResultDele.Broadcast(searchSessionInfo);
			}

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("sessionSearch is null"));

		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Sessions Failed"));
	}

	// ���ΰ�ħ ��ư�� �ٽ� Ȱ��ȭ�Ѵ�.
	serachFinishedDelegate.Broadcast();
}

//ã�� ���� ����Ʈ �߿��� Ư�� ���ǿ� ������ �� �� ����� �Լ�
void UServerGameInstance::JoinMySession(int32 sessionIdx)
{
	//���� ����� ���� ���̴�. �ε����� ������ �����ϰ� �� �������� �����Ѵ�.
	FOnlineSessionSearchResult selectedSession = sessionSearch->SearchResults[sessionIdx];

	//���ǿ� �� ���̴�          ���� sessionID(�̸��Է��ϴ� UI)
	sessionInterface->JoinSession(0, sessionID, selectedSession);
}

//�ٸ� ���ǿ� �շ� ó���� ������ �� ȣ��Ǵ� �̺�Ʈ ���ε� �Լ�
void UServerGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type joinResult)
{
	//��� ���ǿ� ������ �̸��� �� ���̴�. �� ������ IP�ּҿ� port��ȣ�� ���� ���̴�.
		//�ּҿ� ���� string ������ �޾ƿ;��Ѵ�. (���ڿ� �������·� �Ǿ��ִ�.  ��Ʈ�� �⺻ 7777��Ʈ�� �Ǿ��ִ�)
	FString joinAddress;
	//���� �̸����� IP�ּҸ� ȹ���Ѵ�
	sessionInterface->GetResolvedConnectString(sessionName, joinAddress);

	UE_LOG(LogTemp, Warning, TEXT("Join Adress : %s"), *joinAddress);

	//���� Ŭ���̾�Ʈ�̱� ������ Ŭ���̾�Ʈ travel�� �� ���̴�
	if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
	{
		pc->ClientTravel(joinAddress, ETravelType::TRAVEL_Absolute);
		//������ ����Ʈ����� �̵��� ���� ã�ƾ��ϱ� ������
		//�׻���� ������ ã�Ƽ� ����Ʈ����� �� ���̴�.
	}
}
