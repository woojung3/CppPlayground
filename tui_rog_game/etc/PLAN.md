## **TUI-ROG: AI Dungeon Master 프로젝트 계획**

### 1. TUI-ROG 게임 기획

**컨셉:** 플레이어가 절차적으로 생성된 던전을 탐험하는 싱글 플레이어 로그라이크. 각 장소의 묘사는 AI(ChatGPT)가 실시간으로 생성하여, 매번 새로운 탐험 경험을 제공한다.

*   **게임 목표:** 던전의 출구(`>`)를 찾는다.
*   **플레이어 (`Player` Entity, Aggregate Root):**
    *   상태: `id`, `level`, `xp`, `hp`, `stats(Value Object)`, `position(Value Object)`, `inventory(List<Item>)`
    *   행동: 이동, 아이템 사용, 게임 저장/불러오기, 종료.
*   **적 (`Enemy` Entity):** `Orc`, `Goblin` 등. 각각 다른 `stats`를 가짐.
*   **아이템 (`Item` Entity/Value Object):** `HealthPotion`, `StrengthScroll` 등 소모성 아이템.
*   **맵:** 2차원 배열로 구현된 던전.
    *   타일 종류: 벽(`#`), 바닥(`.`), 출구(`>`), 적(`E`), 아이템(`I`).
    *   게임 시작 시 간단한 알고리즘으로 맵을 절차적으로 생성하며, 적과 아이템을 특정 위치에 배치한다.
*   **전투:** 맵에 배치된 적(`E`) 타일로 이동 시 턴제 전투가 시작된다.
*   **성장:** 전투 승리 시 XP 획득, 레벨업 시 `stats` 성장.
*   **패배:** HP 0 이하 시 `PlayerDied` 이벤트 발생, 맵 시작 지점에서 부활.
*   **UI (TUI):**
    *   **맵 뷰:** 게임 맵의 일부를 보여준다.
    *   **메시지 로그:** AI가 생성한 장소 묘사, 게임 이벤트 등을 출력한다.
    *   **상태 바:** 플레이어의 HP 등 주요 정보를 표시한다.
*   **AI 연동:** 플레이어가 새로운 바닥(`.`) 타일로 이동할 때마다, 해당 위치의 특징(주변 타일 등)을 기반으로 AI가 장소에 대한 문학적 묘사를 생성하여 메시지 로그에 출력한다.
*   **영속성:** 플레이어의 턴(행동) 종료 시마다 **자동 저장**하여 강제 종료 시에도 복구 가능.

### 2. 아키텍처 계획

*   **Core (도메인 계층):**
    *   `GameEngine`: 게임의 모든 규칙과 상태를 관리. **행동의 결과로 `List<DomainEvent>`를 반환.**
    *   **Domain Model:**
        *   **Entities:** `Player`, `Enemy`, `Item`
        *   **Value Objects:** `Stats`, `Position`
        *   **Domain Events:** `PlayerMoved`, `ItemFound`, `CombatStarted`, `PlayerLeadedUp`, `PlayerDied` 등
*   **Ports (인터페이스):**
    *   `IGameUI` (Inbound/Outbound): UI를 위한 양방향 포트.
        *   `GetPlayerAction()`: 플레이어의 입력을 받음.
        *   `Render(gameState)`: 게임 상태를 화면에 그림.
    *   `IDescriptionGenerator` (Outbound): 장소 묘사를 생성하기 위한 포트.
        *   `GenerateDescription(context)`: 현재 상황 정보를 받아 묘사 텍스트를 반환.
    *   `IPersistence` (Outbound): 게임 상태를 저장하고 불러오기 위한 포트.
        *   `SaveGame(gameState)` / `LoadGame()`
*   **Adapters (인프라 계층):**
    *   `TUIAdapter`: `IGameUI`를 구현. **FTXUI**를 사용하여 터미널에 UI를 렌더링하고 키보드 입력을 처리. `GameEngine`이 반환한 `List<DomainEvent>`를 해석하여 화면에 메시지를 출력.
    *   `ChatGPTAdapter`: `IDescriptionGenerator`를 구현. **cpp-httplib**과 **nlohmann/json**을 사용하여 OpenAI API와 통신.
    *   `HardcodedDescAdapter`: `IDescriptionGenerator`를 구현. "방입니다." 같은 고정된 텍스트를 반환 (발표 시연용).
    *   `LevelDBAdapter`: `IPersistence`를 구현. **LevelDB**에 게임 상태를 저장.
    *   `InMemoryAdapter`: `IPersistence`를 구현. `std::map` 같은 메모리 내 객체에 게임 상태를 임시 저장 (테스트 및 시연용).

### 3. 기술 스택

*   **언어:** C++17
*   **빌드 시스템:** CMake
*   **TUI:** FTXUI
*   **AI/HTTP:** cpp-httplib, nlohmann/json
*   **Persistence:** LevelDB
*   **패키지 매니저 (macOS):** Homebrew

### 4. 완료된 항목

-   **[완료] 0단계: 프로젝트 환경 설정**
-   **[완료] 1단계: 기본 게임 루프 및 `DomainEvent` 시스템 완성**
    -   `GameEngine` 초기화 흐름 개선 (Adapter가 `INITIALIZE` 커맨드 통해 요청)
    -   `GameState`를 `GameStateDTO`로 변경 및 Port 계층으로 이동

-   **[완료] 맵(`Map`) 시스템 확장 및 구현**
    -   `Tile` 종류에 적(`ENEMY`), 아이템(`ITEM`) 추가 및 TUI 렌더링 구현
    -   `Map` 모델에 `Enemy`와 `Item` 객체 저장 구조 (`std::unique_ptr` 사용) 및 관리 메서드 구현
    -   `Map::generate()`를 통한 임시 맵 및 엔티티 배치 로직 구현
    -   `GameEngine::processPlayerMove`에 벽 충돌 감지 로직 추가
    -   `GameEngine::processPlayerMove`에 타일 이동 시 적/아이템 감지 로직 및 `CombatStartedEvent`, `ItemFoundEvent` 생성 로직 추가
    -   `CombatStartedEvent`와 `ItemFoundEvent`를 Clean Architecture 의존성 규칙에 맞춰 분리된 헤더에 DTO 형태로 정의 (enum 재정의 및 `static_assert`를 통한 일관성 검증 포함)
    -   `TuiAdapter`에서 `CombatStartedEvent`, `ItemFoundEvent`를 받아 메시지 로그에 출력하도록 구현

-   **[부분 완료] 2단계: 성장 및 AI 연동**
-   **[부분 완료] 3단계: 아이템 및 영속성 구현**

---

### 5. 앞으로 해야 할 일 (개발 순서)

1.  **전투 및 아이템 시스템 구현**
    -   `Enemy` 클래스에 체력, 공격력, 이름과 같은 속성 추가 및 `takeDamage()` 구현
    -   `Player` 클래스에 `takeDamage()`, `addXp()` 구현
    -   `GameEngine`에 `CombatStarted` 이벤트에 따른 턴제 전투 로직 구현 (`handlePlayerAction`의 `ATTACK` 케이스).
    -   `GameEngine`에 `ItemFound` 이벤트에 따른 아이템 획득 로직 구현 (플레이어 인벤토리에 추가).
    -   `TuiAdapter`가 `CombatStarted`, `ItemFound`, `PlayerAttacked`, `EnemyDefeated` 등의 이벤트를 수신하여 UI(메시지 로그, 상태 바 등)를 업데이트하도록 구현.
    -   플레이어의 아이템 사용 기능(`UseItem` 커맨드) 및 관련 로직 구현.

2.  **성장 시스템 구현**
    -   전투 승리 시 XP 획득 및 레벨업 시스템 구현 (`PlayerLeveledUp` 이벤트 생성).
    -   `TuiAdapter`가 `PlayerLeveledUp` 이벤트를 수신하여 상태 바(Status Bar)의 레벨 정보를 갱신.

3.  **AI 연동 기능 활성화**
    -   플레이어가 새로운 바닥(`.`) 타일로 이동했을 때(`PlayerMoved` 이벤트 발생 시), `GameEngine`이 `description_port_`를 호출하여 장소 묘사를 얻어오도록 구현.
    -   가져온 묘사를 `DescriptionGenerated` 같은 새로운 `DomainEvent`로 만들어 `TuiAdapter`에 전달.
    -   `TuiAdapter`는 `DescriptionGenerated` 이벤트를 수신하여 메시지 로그에 묘사를 출력.

4.  **영속성 기능 활성화**
    -   플레이어의 턴이 끝날 때마다(`handlePlayerAction` 처리 완료 후), `GameEngine`이 `persistence_port_`를 호출하여 현재 게임 상태를 저장하도록 구현 (자동 저장).
    -   게임 시작 시 `persistence_port_`를 통해 저장된 게임을 불러오는 로직 추가.

5.  **최종 다듬기**
    -   TUI 레이아웃 개선 (맵 뷰, 메시지 로그, 상태 바).
    -   게임 시작 화면, 조작법 안내 등 추가.
    -   `HardcodedDescAdapter`와 `ChatGPTAdapter`를 쉽게 교체하며 시연할 수 있도록 준비.