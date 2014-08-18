#include <utility>
namespace caskell {

	template<typename ReturnType >
	struct func{
		typedef ReturnType value_type;
		//Moreover, the return value of `operator()` should also be `ReturnType`
	};

	template<typename T,typename F >
	class fcurry:public func<typename F::value_type >{
		T first;
		F f;
		public:
		fcurry(F func,T first_arg) :
				first(first_arg), f(func){
		}
		template<typename ...Args >
		typename F::value_type operator()(Args ...args){
			return f(args...,first);
		}
	};
	template<typename T,typename F >
	fcurry<T,F >curry(F func,T fix_arg){
		return fcurry<T,F >(func,fix_arg);
	}

	template<typename T >
	typename T::value_type head(T gen){
		return *gen; //imminent problem:the generator may have already reached the end
	}
	template<typename T >
	typename T::value_type last(T gen){
		typename T::value_type tmp=*gen; //dito
		do{
			tmp=*gen;
			++gen;
		}while(!gen.is_end());
		return tmp;
	}

	template<typename T >
	struct inc{
		T operator ()(T x){
			return ++x;
		}
	};
	template<typename T >
	struct dec:public func<T >{
		T operator ()(T x){
			return --x;
		}
	};

	template<typename T >
	struct reflect:public func<T >{
		T operator ()(T x){
			return x;
		}
	};

	template<typename F >
	class fflip:public func<typename F::value_type >{
		F f;
		public:
		fflip(F func) :
				f(func){
		}
		template<typename A,typename B >
		typename F::value_type operator()(A a,B b){
			return f(b,a);
		}
	};
	template<typename F >
	fflip<F >flip(F func){
		return fflip<F >(func);
	}

	template<typename F >
	class fnotf:public func<bool >{
		F f;
		public:
		fnotf(F func) :
				f(func){
		}
		template<typename T1,typename T2 >
		bool operator ()(T1 a,T2 b){
			return !f(a,b);
		}
		template<typename T >
		bool operator ()(T x){
			return !f(x);
		}
	};
	template<typename F >
	fnotf<F >notf(F func){
		return fnotf<F >(func);
	}

	class linear{
		double a,b;
		public:
		linear(double slope,double offset):
			a(slope),b(offset){
		}
		double operator ()(double x){
			return a*x+b;
		}
	};

	//WARNING!!!!!
	//REALLY REALLY
	//DULLLLLL
	//THINGS
	//BELOW
	template<typename T >
	struct equal:public func<bool >{
		bool operator()(T a,T b){
			return a==b;
		}
	};
	template<typename T >
	struct not_equal:public func<bool >{
		bool operator()(T a,T b){
			return !(a==b);
		}
	};

	struct logic_and:public func<bool >{
		bool operator ()(bool a,bool b){
			return a&&b;
		}
	};
	struct logic_or:public func<bool >{
		bool operator ()(bool a,bool b){
			return a||b;
		}
	};
	struct logic_not:public func<bool >{
		bool operator ()(bool a){
			return !a;
		}
	};
	struct logic_xor:public func<bool >{
		bool operator ()(bool a,bool b){
			return !(a==b);
		}
	};

	template<typename T >
	struct less_than:public func<bool >{
		bool operator ()(T a,T b){
			return a<b;
		}
	};

	template<typename T >
	struct greater_equal_than:public func<bool >{
		bool operator ()(T a,T b){
			return !(a<b);
		}
	};

	template<typename T >
	struct greater_than:public func<bool >{
		bool operator ()(T a,T b){
			return !(a<b)&&!(a==b);
		}
	};

	template<typename T >
	struct less_equal_than:public func<bool >{
		bool operator ()(T a,T b){
			return (a<b)||a==b;
		}
	};

	template<typename T >
	struct add:public func<T >{
		T operator ()(T x,T y){
			return x+y;
		}
	};
	template<typename T >
	struct minus:public func<T >{
		T operator ()(T x,T y){
			return x-y;
		}
	};
	template<typename T >
	struct multi:public func<T >{
		T operator ()(T x,T y){
			return x*y;
		}
	};
	template<typename T >
	struct div:public func<T >{
		T operator ()(T x,T y){
			return x/y;
		}
	};
	template<typename T >
	struct mod:public func<T >{
		T operator ()(T x,T y){
			return x%y;
		}
	};

	template<typename T=int >
	struct gcd:public func<T >{
		T operator()(T a,T b){
			T r;
			while(b>0){
				r=a%b;
				a=b;
				b=r;
			}
			return a;
		}
	};
	template<typename T=int >
	struct lcm:public func<T >{
		T operator()(T a,T b){
			T t,r;
			if(a<b){
				t=a;
				a=b;
				b=t;
			}
			t=a*b;
			r=a%b;
			while(r!=0){
				a=b;
				b=r;
				r=a%b;
			}
			return t/b;
		}
	};
	class faverage:public func<double >{
		unsigned int c;
		double cur;
		public:
		faverage(){
			c=0;
			cur=0.0;
		}
		faverage(double x,unsigned int count) :
				c(count), cur(x){
		}
		double operator()(double x){
			return cur=cur+(x-cur)/(++c);
		}
	};

	template<typename T >
	struct bigger:public func<T >{
		T operator ()(T x,T y){
			return x<y?y:x;
		}
	};
	template<typename T >
	struct smaller:public func<T >{
		T operator ()(T x,T y){
			return x<y?x:y;
		}
	};
}

