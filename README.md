# TUI-ROG: C++ Hexagonal Architecture Example

`TUI-ROG`는 C++로 구현된 터미널 기반 로그라이크 게임으로, 헥사고날 아키텍처를 적용하여 설계되었습니다. 이 프로젝트는 핵심 비즈니스 로직을 UI, 데이터베이스, 외부 서비스와 같은 외부 기술로부터 분리하여, 유지보수성, 테스트 용이성, 그리고 유연성을 높이는 방법을 보여줍니다.

이 저장소는 "[CAM 정기세미나] C++로 배우는 헥사고날 아키텍처" 발표의 샘플 프로젝트로 사용되었습니다.

## 🚀 데모

![Gameplay Demo](assets/gameplay_demo.gif)

## ✨ 주요 기능

*   **터미널 기반 로그라이크 게임:** `ftxui` 라이브러리를 활용한 직관적인 터미널 UI.
*   **헥사고날 아키텍처 적용:** 핵심 도메인 로직과 외부 기술(UI, 영속성, 외부 API)의 명확한 분리.
*   **유연한 영속성 계층:** 인메모리(In-Memory) 또는 LevelDB를 통한 게임 상태 저장/로드.
*   **동적 설명 생성:** 하드코딩된 설명 또는 LLM API를 활용한 게임 상황 설명 생성.
*   **의존성 주입(DI):** 런타임에 어댑터 구현체를 쉽게 교체 가능.
*   **테스트 용이성:** `Google Test` 및 `Google Mock`을 활용한 도메인 로직 및 어댑터 테스트.
*   **아키텍처 경계 강제:** `CMake` 및 `Python` 스크립트를 통해 빌드 시점에 아키텍처 의존성 규칙을 검증하고 강제.
*   **풍부한 도메인 모델:** 비즈니스 규칙과 상태를 캡슐화하는 `Player`, `Map`, `Enemy` 등의 도메인 엔티티.

## 📐 아키텍처 개요
이 프로젝트는 헥사고날 아키텍처의 핵심 원칙인 **Ports & Adapters** 패턴을 따릅니다.

*   **Application Core (육각형):** 게임의 핵심 비즈니스 로직(도메인 모델, 도메인 서비스, 포트)을 포함합니다. 외부 기술에 대한 의존성 없이 순수한 비즈니스 규칙만을 다룹니다.
*   **Ports:** 애플리케이션 코어와 외부 세계 간의 계약(인터페이스)을 정의합니다.
    *   **In-Ports (Driving Ports):** 외부에서 애플리케이션 코어를 구동하기 위한 인터페이스 (예: `IGetPlayerActionUseCase`).
    *   **Out-Ports (Driven Ports):** 애플리케이션 코어가 외부 서비스를 사용하기 위한 인터페이스 (예: `ISaveGameStatePort`, `IGenerateDescriptionPort`).
*   **Adapters:** 포트 인터페이스를 구현하여 외부 기술(UI, 데이터베이스, LLM API)과 애플리케이션 코어를 연결합니다.
    *   **In-Adapters (Driving Adapters):** 사용자 입력(TUI)을 In-Port 호출로 변환합니다 (예: `TuiAdapter`).
    *   **Out-Adapters (Driven Adapters):** Out-Port 인터페이스를 구현하여 실제 외부 서비스를 호출합니다 (예: `LevelDbAdapter`, `LlmAdapter`).

## 📁 프로젝트 구조

```
tui_rog_game/
├───adapter/                 <- 외부 세계와의 통신 (UI, DB, 외부 API)
│   ├───in/
│   │   └───tui/             <- TUI 입력 어댑터 (사용자 입력 처리)
│   └───out/
│       ├───description/     <- 설명 생성 출력 어댑터 (하드코딩/LLM)
│       └───persistence/     <- 영속성 출력 어댑터 (인메모리/LevelDB)
├───application/             <- 핵심 비즈니스 로직 (육각형 안쪽)
│   ├───domain/
│   │   ├───event/           <- 도메인 이벤트
│   │   ├───model/           <- 도메인 모델 (플레이어, 맵, 몬스터 등)
│   │   └───service/         <- 도메인 서비스 (GameEngine)
│   └───port/
│       ├───in/              <- 입력 포트 (IGetPlayerActionUseCase)
│       └───out/             <- 출력 포트 (IRenderPort, ISaveGameStatePort 등)
├───assembly/                <- 애플리케이션 조립 (ApplicationBuilder)
├───common/                  <- 공통 유틸리티
├───scripts/                 <- 빌드 및 아키텍처 검증 스크립트
└───main.cc                  <- 애플리케이션 진입점
```

## 🛠️ 시작하기

### 요구사항

*   CMake (3.15 이상)
*   C++17 호환 컴파일러 (GCC, Clang 등)
*   Python 3 (아키텍처 검증 스크립트용)
*   (선택 사항) LLM 기반 설명 기능을 사용하려면 `GROQ_API_KEY` 환경 변수 설정이 필요합니다.

### 빌드 및 실행

1.  **저장소 클론:**
    ```bash
    git clone https://github.com/woojung3/CppPlayground.git
    cd tui_rog_game
    ```
2.  **빌드 디렉토리 생성 및 CMake 실행:**
    ```bash
    mkdir build
    cd build
    cmake ..
    ```
3.  **프로젝트 빌드:**
    ```bash
    cmake --build .
    ```
4.  **게임 실행:**
    ```bash
    ./tui_rog_game
    ```

### 아키텍처 경계 강제 검증

이 프로젝트는 `scripts/check_architecture.cmake` 스크립트를 통해 아키텍처 의존성 규칙을 자동으로 검증합니다. 빌드 과정에서 `check_architecture` 타겟이 실행되며, `allowed_architecture.dot` 파일과 현재 코드베이스에서 추출된 의존성 그래프를 비교합니다.

*   **의존성 그래프 생성:**
    ```bash
    cmake --build . --target generate_architecture_graph
    # 생성된 파일: build/current_architecture.dot
    ```
*   **아키텍처 검증:**
    ```bash
    cmake --build . --target check_architecture
    ```
    만약 `current_architecture.dot`이 `allowed_architecture.dot`과 다르면 빌드가 실패하여 아키텍처 변경을 강제적으로 검토하도록 합니다.

## 📚 주요 기술 스택

*   **C++17:** 모던 C++ 기능 활용
*   **CMake:** 빌드 시스템
*   **FTXUI:** 터미널 UI 라이브러리
*   **cpp-httplib:** HTTP 클라이언트 (LLM 연동용)
*   **nlohmann/json:** JSON 파싱 및 생성
*   **spdlog:** 로깅 라이브러리
*   **Google Test / Google Mock:** 단위 및 통합 테스트 프레임워크
*   **Google Benchmark:** 성능 벤치마킹
*   **LevelDB:** 영속성 저장소
*   **Python 3:** 아키텍처 의존성 추출 스크립트
