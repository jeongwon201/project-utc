# project-utc | 무인 열 감지 카메라
> 코로나 팬대믹의 인력난 해소를 위한 무인 체온 즉정기

<div>
  <img src="https://img.shields.io/badge/utc-1.0 ver-blue"/>
  <img alt="GitHub License" src="https://img.shields.io/github/license/jeongwon201/project-stmp">
</div>
<br />
<br />

<div align="center">
   <image src="images/result.png" style="float: left;"/>
</div>
<br />

이 무인 체온 측정기는 코로나 팬대믹의 방역 인력 부족 문제를 해결하기 위한 대안으로 개발되었습니다.  
독자적인 발열 판단 알고리즘으로, 보다 정확하고 정밀한 판단이 가능합니다.  
이 알고리즘은 발열 상태를 신속하게 파악하고 대응하는 데 큰 도움이 됩니다.  

이 무인 체온 측정기는 개인과 사회적 건강을 보호하는 데 큰 도움이 됩니다.  
적용된 기술을 통해 코로나와 같은 전염병으로부터 안전을 유지하는 데 기여합니다.  
<br />

## 시작하기

### 설치 방법

부품:

```sh
AMG8833, HC-SR04, ILI9341 2.2” TFT LCD, NodeMCU(ESP8266 Wi-Fi)
```


1. Arduino IDE 를 설치하세요.
<a href="https://www.arduino.cc/en/software">Arduino/Software</a>
> 이미 설치되어있다면 생략하세요.

2. ESP8266 Wi-Fi 라이브러리 설치 및 설정을 하세요.

> - Preferences > Additional Boards Manager URLs: 텍스트 박스에  
>   ```http://arduino.esp8266.com/stable/package_esp8266com_index.json``` 입력 후 OK  
> - Tools > Board > Boards Manager... > esp8266 검색 후 설치  
> - 설치가 완료되었다면, Tools > Board > NodeMCU 1.0 (ESP-12E Module) 선택  


3. 부품을 활용하여 회로</a>를 구성하세요.
4. <a href="https://github.com/jeongwon201/project-utc/blob/main/arduino/utc.ino">utc.ino</a> 파일을 NodeMCU 에 업로드하세요.
<br />

### 사용 예제

화면에 표시된 가이드 라인에 얼굴을 맞추어 서서 체온을 측정합니다.  

체온을 측정할 수 없는 상태일 경우, 화면에 다음과 같은 결과가 출력됩니다.

- **Go back a little**
   - 측정자가 너무 앞에 있을 경우
- **Come a little bit forward**
   - 측정자가 너무 뒤에 있을 경우
- **Not Detected**
   - 측정자가 없을 경우

비정상적인 체온이 감지될 경우, 손목의 온도를 측정합니다.  
이 때, 화면의 가이드 라인은 손목 형태의 가이드 라인으로 변경됩니다.  

체온 측정이 완료되면, 화면에 다음과 같은 결과가 출력됩니다.

- **＠℃, Pass**
   - 정상 체온일 경우
- **＠℃, Non-Pass**
   - 비정상 체온일 경우

## 업데이트 내역

* 1.0.0
    * 첫 출시
* 0.0.1
    * 작업 진행 중

## 작성자
- 이정원 - jeongwon201@naver.com
- 정홍준
- 정하민

## 라이센스

이 프로젝트는 MIT 라이센스를 준수하며 <a href="https://github.com/jeongwon201/project-utc/blob/main/LICENSE">LICENSE</a>에서 자세한 정보를 확인할 수 있습니다.

## 기여 방법

1. (<https://github.com/jeongwon201/project-utc/fork>)을 포크합니다.
2. (`git checkout -b feature/fooBar`) 명령어로 새 브랜치를 만드세요.
3. (`git commit -am 'Add some fooBar'`) 명령어로 커밋하세요.
4. (`git push origin feature/fooBar`) 명령어로 브랜치에 푸시하세요. 
5. 풀리퀘스트를 보내주세요.

<!-- Markdown link & img dfn's -->
[npm-image]: https://img.shields.io/npm/v/datadog-metrics.svg?style=flat-square
[npm-url]: https://npmjs.org/package/datadog-metrics
[npm-downloads]: https://img.shields.io/npm/dm/datadog-metrics.svg?style=flat-square
[travis-image]: https://img.shields.io/travis/dbader/node-datadog-metrics/master.svg?style=flat-square
[travis-url]: https://travis-ci.org/dbader/node-datadog-metrics
[wiki]: https://github.com/yourname/yourproject/wiki
