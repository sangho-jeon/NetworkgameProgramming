># Network Game Programming
>Direct X 기반 싱글플레이 게임
### 프로젝트 기간: _(2019 1학기)_

>## Feature

주인공인 고양이 플레이어가 상단에서 떨어지는 물체를 피하는 게임</br>
방향키 좌/우로 캐릭터를 조종, 떨어지는 물체에 닿으면 상단의 체력이 감소</br>
체력이 0 이 되면 초기의 화면으로 돌아가게 됨

<img src="https://user-images.githubusercontent.com/59248591/73937777-29ae5880-4929-11ea-8b9d-aaec095259ef.PNG" width = 300px height =100%>
<img src="https://user-images.githubusercontent.com/59248591/73937772-27e49500-4929-11ea-9f99-813e61317c18.PNG" width = 300px height =100%>
<img src="https://user-images.githubusercontent.com/59248591/73937774-2915c200-4929-11ea-9961-cd26e52161d2.PNG" width = 300px height =100%>

> ## How does it Work?
> 소켓 통신 방법을 이용하여 master client가 server에 접속</br>
> slave 프로그램도 동일하게 server에 접속 </br>
> master 프로그램의 상태를 서버를 통해 slave 프로그램에 전송, slave프로그램에는 동일한 상태의 화면을 출력

</br>
</br>
</br>
</br>
</br>

_현재는 master프로그램의 화면을 slave프로그램에서 보여주는 프로그램_

</br>

_추가적으로 멀티플레이를 지원하게 위해서는 master와 slave 프로그램의 기능을 동시에 가지는 프로그램을 개발해야함_
