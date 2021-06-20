# Computer-Graphics
> 기초컴퓨터그래픽스 수업입니다. 여태껏 들은 수업 중에 손에 꼽을 정도로 재미있었습니다. 물론 프로젝트나 강의 내용은 어려웠지만, 코딩하는 과정에서 한 줄마다 가지고 있는 의미가 눈에 보이는 애니메이션으로 표현되니 지루하지 않았습니다.
> 프로젝트는 OpenGL을 통한 3차원 그래픽스 프로그래밍입니다.
>

___

## 0. 구현 모습
- 백문이불여일견! <br><br>
![ezgif com-gif-maker](https://user-images.githubusercontent.com/32920566/122640809-56d56280-d13c-11eb-8a30-71eb8b79f1ff.gif)
## 1. 구현 내용

(a)-ⅰ
: 옵티머스, 버스, 아이언맨을 세상에 배치하였습니다.

(a)-ⅱ
: 사람, 울프, 거미를 세상에 배치하였습니다.

(a)-ⅲ
: 아이언맨을 선택하여 첫 시작 시 자율비행으로 공중으로 떠올라서 회전운동을 하도록 하였습니다.

(a)-ⅳ
: 거미를 선택하여 마치 버스 위로 올라타 움직이고, 마지막엔 버스 뒤로 사라지는 모습을 연출했습니다.

(b)-ⅰ
: 0,1번 광원은 켜져있고 2,3번 광원은 꺼져있습니다.

(b)-ⅱ
: 0번 광원은 세상 전체를 비추도록 회색에 가까운 색 지정, 1번 광원은 옵티머스 위를 비추고 파란색으로 지정하였습니다. 

(b)-ⅲ
: ‘2’를 누르면 켜지고, 광원은 빨간색으로 버스 위를 비춥니다.

(b)-ⅳ
: ‘3’을 누르면 켜지고, 광원은 노란색으로 아이언맨의 머리쪽에 붙어 있어 앞을 비춥니다.


※ F1 키를 누르면 카메라 시점이 가장 처음 상태로 돌아옵니다. 채점하실 때 사용하시면 좋을 것 같습니다.
(c)-ⅰ
: ‘u’ ‘i’로 카메라 u축,    ‘v’,‘b’로 카메라 v축,     ‘n’,‘m’으로 카메라 n축으로 이동 가능합니다.

(c)-ⅱ
:  ‘,(쉼표)’ ‘.(마침표)’ ‘/’ 키로 각각 u, v, n 축으로 회전 가능합니다. 반대로 회전 하는 것은 Rotation Matrix에서 –로 angle을 지정하면 되기 때문에 굳이 구현을 하지는 않았습니다.


(c)-ⅲ
: page up키를 누르면 줌 인, page donw키를 누르면 줌 아웃됩니다.

(d)-ⅰ
: ‘t’키를 누르면 제가 직접 만든 텍스쳐로 아이언맨이 옷을 바꿔 입습니다. 다시 누르면 원래 기본 제공코드의 텍스쳐인 호랑이로 옷을 바꿔입습니다.

(e)
: ‘w’키를 누르면 옵티머스를 비추고 있는 1번광원(옵티머스를 비추는 광원)에서 재미있는 광원 효과가 나타납니다.
