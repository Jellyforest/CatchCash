// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Kismet/GameplayStatics.h"

//생성자 하나 만들어주면 좋다
UServerGameInstance::UServerGameInstance()
{
	sessionID = "Test Session";
}

//런타임에 최초 실행되는 함수
void UServerGameInstance::Init()
{
	Super::Init();
	//세션을 생성할 것이다.

	

	//온라인 세션 기능이 구현되어있는 IOnlineSubsystem 클래스를 가져온다.
	IOnlineSubsystem* subsys = IOnlineSubsystem::Get();
	if (subsys)
	{

		//델리게이트 사용한다. 델리게이트를 통해서 서버에서 정보를 받아올 것이다.
		sessionInterface = subsys->GetSessionInterface();
		if (sessionInterface != nullptr)
		{
			sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UServerGameInstance::OnCreateSessionComplete);

			//델리게이트 함수이므로 UFUNCTION 써준다
			//비동기액션(한프레임에 끝나는 액션이 아니다. 요청시간과 행위가 끝나는 시간이 동일하지 않을 때 사용하는 단어)
			//Find 과정 중 오류 없이 검색이 완료되면 true, 그렇지 않으면 false를 도출해주겠다.
			sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UServerGameInstance::OnFindSessionComplete);

			//세션에 합류했다고 처리하는 delegate함수
			sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UServerGameInstance::OnJoinSessionComplete);
		}

	}

	//연결 플랫폼의 이름을 출력한다
	FString platformName = subsys->GetSubsystemName().ToString();
	UE_LOG(LogTemp, Warning, TEXT("PlatformName : %s"), *platformName);
}

//세션 생성 함수
void UServerGameInstance::CreateMySession(FString roomName, int32 playerCount)
{
	if (sessionInterface != nullptr)
	{
		//세션을 만든다
		FOnlineSessionSettings sessionSettings;

		//초대가 가능한가
		sessionSettings.bAllowInvites = true;

		//세션이 진행 중에도 난입이 가능한가?
		sessionSettings.bAllowJoinInProgress = true;

		//게임 중에도 외부인이 들어올 수 있도록 할 것인가
		sessionSettings.bAllowJoinViaPresence = true;
		sessionSettings.bIsDedicated = false; //우리는 Lan방식을 사용할것인가

		//온라인 서브시스템의 이름이 "NULL"이면 LAN 매칭을 하고, "Steam"이면 스팀 서버로 매칭한다.
		sessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
		//내가 만든 세션에 접속할 수 있는 최대의 숫자 (나를 포함한 숫자이다, 최소 2)
		//변수로 값을 받아오도록 매개변수 이름을 넣어주었다.
		sessionSettings.NumPublicConnections = playerCount;

		//방 이름 저장한다
		sessionSettings.Set(FName("KEY_RoomName"), roomName);
		//세션의 이름을 검색할 수 있게 할 것인지
		sessionSettings.bShouldAdvertise = true;


		//세션을 생성한다
		sessionInterface->CreateSession(0, sessionID, sessionSettings);

		UE_LOG(LogTemp, Warning, TEXT("Create Session Try"));
	}
	else
	{

	}
}

//세션이 만들어졌을 때 호출된 함수
void UServerGameInstance::OnCreateSessionComplete(FName sessionName, bool bIsSuccess)
{
	//                               true일 때  이게 뜰 것             false일 때 이게 뜰 것
	FString result = bIsSuccess ? TEXT("Create Session Success!") : TEXT("Create Session Failed...");
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s = %s"), *result, *sessionName.ToString()));
	UE_LOG(LogTemp, Warning, TEXT("%s = %s"), *result, *sessionName.ToString());

	//세션 생성에 성공했다면, 전투맵 쪽으로 세션에 접속한 모든 인원을 이동시킨다.
	if (bIsSuccess)
	{
		//본인이 리슨서버라면 방장을 포함한 모든 인원이 이동할것이다 라는 것
		GetWorld()->ServerTravel("/Game/JY/Maps/MainMap?Listen");
	}

	
}

//만들어져있는 세션을 찾는 함수
void UServerGameInstance::FindMySession()
{
	//찾으려는 세션 쿼리를 생성한다
	//native c++ 함수를 변환하기 위한 작업
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	//LanQuery가 null이면 true가 되어서 lanquery로 접속한다. false면 "STEAM"이고 STEAM으로 접속하라 
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	sessionSearch->MaxSearchResults = 30;
	//특정 키로 검색 조건을 필터링하고싶을 때추가하는 쿼리
	//sessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, roomName, EOnlineComparisonOp::GreaterThanEquals);

	//Presense로 생성된 세션을 필터링하고싶을 때
	//SEARCH_PRESENCE : 조건을 걸어주는 매크로를 사용할 것이다. 거의 기본으로 쿼리 조건에 들어가도록 해야한다.
	//value는 true : 내가 가지고 있는 레벨하고 동일한거만 찾는다
	//false : 내가 갖고있지 않은 레벨도 검색하도록 열어놓는다
	//내가 갖고있는 레벨과 일치할 경우에만 검색에 뜨도록 하면 되기 때문에 Equal을 적어주었다.
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	//앞에서 만든 쿼리를 이용해서 세션을 찾는다.       sessionSearch가 포인터 형태인데 참조자 형태로 캐스팅해주었다.
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());

}

//세션 검색이 끝났을 때 호출될 함수
void UServerGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	
	if (bWasSuccessful) //여기서 해야할게 많다
	{
		if (sessionSearch)
		{

			//총 몇개의 방이 찾아졌는지 결과를 보자
			TArray<FOnlineSessionSearchResult> searchResults = sessionSearch->SearchResults;
			UE_LOG(LogTemp, Warning, TEXT("Find Session count = %d"), searchResults.Num());
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Find Session count = %d"), searchResults.Num()));

			//검색 결과들을 모두 순회한다.
			for (int32 i = 0; i < searchResults.Num(); i++)
			{
				FSessionInfo searchSessionInfo;
				FString roomName;

				searchResults[i].Session.SessionSettings.Get(FName("KEY_RoomName"), roomName);
				searchSessionInfo.roomName = roomName;

				//maxPlayer의 수 가져오기
				searchSessionInfo.maxPlayers = searchResults[i].Session.SessionSettings.NumPublicConnections;
				//현재 플레이어의 수 가져오기
				searchSessionInfo.currentPlayers = searchSessionInfo.maxPlayers - searchResults[i].Session.NumOpenPublicConnections;
				searchSessionInfo.ping = searchResults[i].PingInMs;

				searchSessionInfo.idx = i;

				//슬롯에 필요한 정보를 이벤트로 송출한다.
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

	// 새로고침 버튼을 다시 활성화한다.
	serachFinishedDelegate.Broadcast();
}

//찾은 세션 리스트 중에서 특정 세션에 들어가고자 할 때 사용할 함수
void UServerGameInstance::JoinMySession(int32 sessionIdx)
{
	//세션 결과가 나올 것이다. 인덱스로 세션을 선택하고 그 세션으로 조인한다.
	FOnlineSessionSearchResult selectedSession = sessionSearch->SearchResults[sessionIdx];

	//세션에 들어갈 것이다          나의 sessionID(이름입력하는 UI)
	sessionInterface->JoinSession(0, sessionID, selectedSession);
}

//다른 세션에 합류 처리가 끝났을 때 호출되는 이벤트 바인딩 함수
void UServerGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type joinResult)
{
	//어디 세션에 들어갔는지 이름이 뜰 것이다. 그 세션의 IP주소와 port번호를 받을 것이다.
		//주소에 대한 string 변수를 받아와야한다. (문자열 참조형태로 되어있다.  포트는 기본 7777포트로 되어있다)
	FString joinAddress;
	//세션 이름으로 IP주소를 획득한다
	sessionInterface->GetResolvedConnectString(sessionName, joinAddress);

	UE_LOG(LogTemp, Warning, TEXT("Join Adress : %s"), *joinAddress);

	//나는 클라이언트이기 때문에 클라이언트 travel로 들어갈 것이다
	if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
	{
		pc->ClientTravel(joinAddress, ETravelType::TRAVEL_Absolute);
		//서버가 서버트레블로 이동한 맵을 찾아야하기 떄문에
		//그사람의 서버를 찾아서 서버트레블로 들어간 것이다.
	}
}
