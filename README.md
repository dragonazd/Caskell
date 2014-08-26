Caskell
======
Provides all kinds of functional functions such as map,filter,fold...everything you need to perform functional programming
**News: r0.1 was recently released! If anyone have tried the lib, I'll be grateful to receive some feedback :-)**
###A demo
    #include "caskell.cpp"
    using namespace caskell;
    int main(){
      int s=sum(range(10000+1));//s=1+2+..10000
    }
  Isn't it elegant? On the contrary, in a traditional way:
  
    int main(){
      int ans=0;
      for(int i=0;i!=10000+1;++i)
        ans+=i;//dull :-(
    }
  Dull,right?
  
  
contact me at dragonazd@gmail.com
Or while you hear that PRC has blocked gmail:757887477@qq.com
