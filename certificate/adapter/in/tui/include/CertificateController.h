#pragma once

#include <string>

namespace playground {
// 여기서는 tui로 폴더를 명명하였습니다만,
// 외부 컴포넌트에 의해 호출될 수 있는 최상단 API를 두는 곳이라고 이해해도
// 됩니다 udp, tcp, websocket, ipc 등, 입력 기술이 달라질 경우, tui 폴더에
// 대응되는 레벨에 해당 기술에 대한 폴더를 추가합니다
bool addCertificate(std::string cert);
} // namespace playground
