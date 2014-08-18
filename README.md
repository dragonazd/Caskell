Caskell
======
Provides all kinds of functinal functions such as map,filter,fold...everything you need to perform functional progarmming
##CAUTION:Still in development, interfaces may change
###A demo
    #include "caskell.cpp"
    using namespace caskell;
    int main(){
      int s=last(sum(range(10000+1)));//s=1+2+..10000
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
