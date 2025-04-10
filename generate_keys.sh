#!/bin/bash
set -e

# --- 서버 키/인증서 (keco) ---
echo "생성 중: keco.key (EC Private Key)"
openssl ecparam -name prime256v1 -genkey -noout -out keco.key

echo "생성 중: keco.pem (Self-signed Certificate)"
openssl req -new -x509 -key keco.key -out keco.pem -days 365 -subj "/CN=keco"

# --- 클라이언트 키/인증서 (charger) ---
echo "생성 중: charger.key (EC Private Key)"
openssl ecparam -name prime256v1 -genkey -noout -out charger.key

echo "생성 중: charger.pem (Self-signed Certificate)"
openssl req -new -x509 -key charger.key -out charger.pem -days 365 -subj "/CN=charger"

echo "모든 키/인증서 파일이 생성되었습니다."

