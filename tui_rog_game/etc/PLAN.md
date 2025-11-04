## **TUI-ROG: AI Dungeon Master 프로젝트 계획 (v2)**

### 1. TUI-ROG 게임 기획 (v2)

**컨셉:** 플레이어가 절차적으로 생성된 던전을 탐험하는 싱글 플레이어 로그라이크. 각 장소의 묘사는 AI(ChatGPT)가 실시간으로 생성하여, 매번 새로운 탐험 경험을 제공한다.

*   **게임 목표:** 던전의 출구(`>`)를 찾는다.
*   **플레이어 (`Player` Entity, Aggregate Root):**
    *   상태: `id`, `level`, `xp`, `hp`, `stats(Value Object)`, `position(Value Object)`, `inventory(List<Item>)`
    *   행동: 이동, 아이템 사용, 게임 저장/불러오기, 종료.
*   **적 (`Enemy` Entity):** `Orc`, `Goblin` 등. 각각 다른 `stats`를 가짐.
*   **아이템 (`Item` Entity/Value Object):** `HealthPotion`, `StrengthScroll` 등 소모성 아이템.
*   **맵:** 2차원 배열로 구현된 던전.
    *   타일 종류: 벽(`#`), 바닥(`.`), 출구(`>`).
    *   게임 시작 시 간단한 알고리즘으로 맵을 절차적으로 생성한다.
*   **전투:** 새 타일 이동 시 확률적으로 Random Encounter. 턴제 전투.
*   **성장:** 전투 승리 시 XP 획득, 레벨업 시 `stats` 성장.
*   **패배:** HP 0 이하 시 `PlayerDied` 이벤트 발생, 맵 시작 지점에서 부활.
*   **UI (TUI):**
    *   **맵 뷰:** 게임 맵의 일부를 보여준다.
    *   **메시지 로그:** AI가 생성한 장소 묘사, 게임 이벤트 등을 출력한다.
    *   **상태 바:** 플레이어의 HP 등 주요 정보를 표시한다.
*   **AI 연동:** 플레이어가 새로운 바닥(`.`) 타일로 이동할 때마다, 해당 위치의 특징(주변 타일 등)을 기반으로 AI가 장소에 대한 문학적 묘사를 생성하여 메시지 로그에 출력한다.
*   **영속성:** 플레이어의 턴(행동) 종료 시마다 **자동 저장**하여 강제 종료 시에도 복구 가능.

### 2. 아키텍처 계획 (v2)

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

### 3. 기술 스택 (v2)

*   **언어:** C++17
*   **빌드 시스템:** CMake
*   **TUI:** FTXUI
*   **AI/HTTP:** cpp-httplib, nlohmann/json
*   **Persistence:** LevelDB
*   **패키지 매니저 (macOS):** Homebrew

### 4. 개발 단계 (v2)

*   **0단계: 프로젝트 환경 설정**
    1.  Git 저장소 초기화 및 `PLAN.md` 파일 생성.
    2.  `CMakeLists.txt` 기본 구조 작성.
    3.  CMake의 `FetchContent`를 사용하여 FTXUI, cpp-httplib, nlohmann/json 라이브러리 자동 연동 설정.
    4.  Homebrew로 설치한 LevelDB를 CMake에서 찾아 링크하도록 설정.
    5.  FTXUI를 사용한 "Hello TUI-ROG"를 화면에 띄워 모든 라이브러리 설정이 완료되었는지 검증.

*   **1단계: 기본 게임 루프 및 전투 구현**
    1.  `GameEngine`과 핵심 엔티티(`Player`, `Enemy`), 값 객체(`Stats`) 정의.
    2.  `TUIAdapter`로 맵, 플레이어, 적을 화면에 렌더링.
    3.  플레이어 이동 및 **간단한 턴제 전투 로직** 구현. `GameEngine`이 `DomainEvent`를 반환하고, `TUIAdapter`가 이를 출력하는 구조 완성.

*   **2단계: 성장 및 AI 연동**
    1.  전투 후 XP 획득 및 레벨업 시스템 구현.
    2.  `IDescriptionGenerator` 포트 및 어댑터들(`ChatGPT`, `Hardcoded`) 구현 및 연동.

*   **3단계: 아이템 및 영속성 구현**
    1.  아이템 획득 및 사용 기능 구현.
    2.  `IPersistence` 포트 및 `LevelDBAdapter` 구현.
    3.  **턴 종료 시 자동 저장** 로직 추가.

*   **4단계: 발표를 위한 최종 다듬기**
    1.  TUI 레이아웃 개선.
    2.  게임 시작 화면, 조작법 안내 등 추가.
    3.  발표의 하이라이트가 될 '어댑터 교체' 시연 부분을 집중적으로 연습.
