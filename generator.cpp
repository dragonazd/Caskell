#include <list>
#include <utility>
//#include <climits>

#include "metafunc.cpp"
#include "exce.hpp"

/*
 template<typename T>
 struct generator{
 void skip()
 const T& operator *()
 //for enumeratable
 bool is_end()
 };
 */
namespace caskell {

	template<typename T>
	class generator{
		public:
		virtual const T& operator *()=0;
		virtual void skip()=0;

		typedef T value_type;

		struct clend{
		} end;
		virtual bool is_end()=0;
		bool operator==(const clend &_){
			return is_end();
		}
		void operator++(){
			skip();
		}
		void operator++(int){
			skip();
		}
		const T* operator->(){
			return &(this->operator *());
		}
		template<typename ...Args>
		T operator ()(Args...){
			T tmp=this->operator *();
			++(*this);
			return tmp;
		}

		template<typename U>
		void dump(U start,U end){
			while(start!=end && !is_end()){
				*start=this->operator *();
				skip();
				++start;
			}
		}
		template<typename U>
		void dump(U inserter){
			while(!is_end()){
				*inserter=this->operator *();
				skip();
			}
		}
	};
	template<typename T>
	bool generator<T>::is_end(){
		return false;
	}

	template<typename T>
	class gslice:public generator<typename T::value_type>{
		T g;
		int s,e;
		const bool backward;
		std::list<typename T::value_type> l;
		public:
		gslice(T gen,int begin,int end) :
				g(gen), s(begin), e(end), backward(s<0||e<0){
			if(backward){
				while(!g.is_end()){
					l.push_back(*g);
					++g;
				}
				if(s<0) s+=l.size()-1;
				if(e<0) e+=l.size()-1;
				while(s&&!l.empty()){
					--s;
					--e;
					l.pop_front();
				}
			}
			else for(int i=0;i!=s&&!g.is_end();++i)
				++g;
		}
		void skip(){
			++s;
			if(backward){
				l.pop_front();
			}
			else{
				++g;
			}
		}
		const typename T::value_type& operator *(){
			if(backward)
				return l.front();
			else return g.operator *();
		}
		bool is_end(){
			return (backward?l.empty():g.is_end())||!(s<e);
		}
	};
	template<typename T>
	gslice<T>slice(T gen,int begin,int end){
		return gslice<T>(gen,begin,end);
	}
	template<typename T>
	gslice<T>take(int size,T gen){
		return gslice<T>(gen,0,size);
	}

	template<typename T>
	class greverse:public generator<typename T::value_type>{
		std::list<typename T::value_type> l; //actually a stack
		public:
		greverse(T gen){
			while(!gen.is_end()){
				l.push_back(*gen);
				++gen;
			}
		}
		void skip(){
			l.pop_back();
		}
		const typename T::value_type& operator *(){
			return l.back();
		}
		bool is_end(){
			return l.empty();
		}
	};
	template<typename T>
	greverse<T>reverse(T gen){
		return greverse<T>(gen);
	}

	template<typename T,typename F>
	class gtake_while:public generator<typename T::value_type>{
		F f;
		T g;
		bool end;
		public:
		gtake_while(T gen,F func) :
				g(gen), f(func), end(false){
		}
		void skip(){
			++g;
			is_end();
		}
		const typename T::value_type& operator *(){
			return g.operator *();
		}
		bool is_end(){
			return end||(end=!f(*g));
		}
	};
	template<typename T,typename F>
	gtake_while<T,F>take_while(T gen,F func){
		return gtake_while<T,F>(gen,func);
	}

	template<typename T>
	class gtail:public generator<typename T::value_type>{
		T g;
		public:
		gtail(T gen,int skip=1) :
				g(gen){
			for(int i=0;i!=skip;++i)
				++g;
		}
		void skip(){
			++g;
			return;
		}
		const typename T::value_type& operator *(){
			return (*g);
		}
		bool is_end(){
			return g.is_end();
		}
	};
	template<typename T>
	gtail<T>tail(T gen,int skip=1){
		return gtail<T>(gen,skip);
	}

	template<typename T,typename F>
	class giter:public generator<T>{
		T d;
		F f;
		public:
		giter(T data,F func) :
				d(data), f(func){
		}
		void skip(){
			d=f(d);
		}
		const T& operator *(){
			return d;
		}
	};
	template<typename T,typename F>
	giter<T,F>iter(T data,F func){
		return giter<T,F>(data,func);
	}
	template<typename T=int>
	giter<T,inc<T>>increment(T data=T()){
		return iter(data,inc<T>());
	}
	template<typename T=int>
	giter<T,dec<T>>decrement(T data=T()){
		return iter(data,dec<T>());
	}
	template<typename T>
	giter<T,reflect<T>>repeat(T data){
		return iter(data,reflect<T>());
	}

	template<typename T>
	class fn_n_switch:public func<T>{
		int n1,n2;
		T v1,v2;
		int count;
		public:
		fn_n_switch(T a,int ca,T b,int cb) :
				v1(a), v2(b), n1(ca), n2(cb){
			count=1;
		}
		void skip(){
			++count;
		}
		const T& operator*(){
			if(count<=n1)
				return v1;
			else if(count<=n1+n2||n2==-1)
				return v2;
			else{
				count=1;
				return v1;
			}
		}
	};
	template<typename T>
	fn_n_switch<T>n_n_switch(T a,int ca,T b,int cb){
		return fn_n_switch<T>(a,ca,b,cb);
	}
	//TODO:improve it to receive multi pairs of times and value

	template<typename T,typename F,typename cmpF>
	class grange:public generator<T>{
		T cur,e;
		F f;
		cmpF cmp;
		public:
		grange(T begin,T end,F func,cmpF cmpf) :
				cur(begin), e(end), f(func), cmp(cmpf){
		}
		void skip(){
			cur=f(cur);
		}
		const T& operator *(){
			return cur;
		}
		bool is_end(){
			return !cmp(cur,e);
		}
	};
	template<typename T,typename F=inc<T>,typename cmpF=less_than<T>>
	grange<T,F,cmpF>range(T begin,T end,F func=inc<T>(),cmpF cmpf=cmpF()){
		return grange<T,F,cmpF>(begin,end,func,cmpf);
	}
	template<typename T,typename F=inc<T>,typename cmpF=less_than<T>>
	grange<T,F,cmpF>range(T end,F func=inc<T>(),cmpF cmpf=cmpF()){
		return grange<T,F,cmpF>(T(),end,func,cmpf);
	}

	template<typename T>
	class gcircle:public generator<typename T::value_type>{
		T g,backup;
		public:
		gcircle(T gen) :
				g(gen), backup(gen){
		}
		void skip(){
			++g;
			if(g.is_end()) g=backup;
		}
		const typename T::value_type& operator *(){
			return *g;
		}
	};
	template<typename T>
	gcircle<T>circle(T gen=T()){
		return gcircle<T>(gen);
	}

	template<typename T1,typename T2>
	class gconcat:public generator<typename T1::value_type>{
		T1 g1;
		T2 g2;
		bool switched;
		public:
		gconcat(T1 gen1,T2 gen2) :
				g1(gen1), g2(gen2){
			switched=false;
		}
		void skip(){
			if(switched)
				++g2;
			else if(g1.is_end())
				switched=true;
			else ++g1;
		}
		const typename T1::value_type& operator *(){
			return switched?*g2:*g1;
		}
		bool is_end(){
			return g1.is_end()&&g2.is_end();
		}
	};
	template<typename T1,typename T2>
	gconcat<T1,T2>concat(T1 gen1,T2 gen2){
		return gconcat<T1,T2>(gen1,gen2);
	}
	//TODO: later should implement a `concat` that can surport multi generators

	template<typename T,typename F,typename ReturnType=typename F::value_type>
	class gmapf:public generator<ReturnType>/*actually, since a infinite generator never end, it could handle infinite generator as well*/{
		//This class will not check the boundary condition for efficiency
		//pass it to the former level of generator
		//anyway, an exception is an exception, right? No matter where it's thrown
		//by the way, the suffix "f" is a brute method to solve the conflict with std::map
		T g;
		ReturnType c;
		bool dirty;
		F f;
		public:
		gmapf(T gen,F func) :
				g(gen), f(func){
			dirty=false;
			c=f(*g);
		}
		void skip(){
			++g;
			dirty=true;
		}
		const ReturnType& operator *(){
			if(dirty){
				c=f(*g);
				dirty=false;
				return c;
			}
			else return c;
		}
		bool is_end(){
			return g.is_end();
		}
	};
	template<typename T,typename F,typename ReturnType=typename F::value_type>
	gmapf<T,F,ReturnType>mapf(T gen,F func){
		return gmapf<T,F,ReturnType>(gen,func);
	}

	template<typename T,typename F>
	class gfilter:public generator<typename T::value_type>{/*dito*/
		//consider to throw a exception when the result is empty?
		//imminent death loop when encountering infinite generator
		typename T::value_type c;
		T g;
		F f;
		public:
		gfilter(T gen,F func) :
				g(gen), f(func){
			while(!g.is_end()){
				if(f(*g)){
					c=*g;
					break;
				}
				++g;
			}
		}
		void skip(){
			++g;
			while(!g.is_end()){
				if(f(*g)){
					c=*g;
					break;
				}
				++g;
			}
		}
		const typename T::value_type& operator *(){
			return c;
		}
		bool is_end(){
			return g.is_end();
		}
	};
	template<typename T,typename F>
	gfilter<T,F>filter(T gen,F func){
		return gfilter<T,F>(gen,func);
	}

	template<typename T,typename F,typename ReturnType=typename T::value_type>
	class gfold:public generator<ReturnType>{
		F f;
		T g;
		ReturnType c;
		public:
		gfold(ReturnType first,T gen,F func) :
				c(first), g(gen), f(func){
			skip();
		}
		gfold(T gen,F func) :
				f(func), g(gen){
			c=*g;
			++g;
		}
		const ReturnType& operator *(){
			return c;
		}
		void skip(){
			c=f(c,*g);
			++g;
		}
		bool is_end(){
			return g.is_end();
		}
	};
	template<typename T,typename F,typename ReturnType=typename T::value_type>
	gfold<T,F,ReturnType>fold(ReturnType first,T gen,F func){
		return gfold<T,F,ReturnType>(first,gen,func);
	}
	template<typename T,typename F,typename ReturnType=typename T::value_type>
	gfold<T,F,ReturnType>fold1(T gen,F func){
		return gfold<T,F,ReturnType>(gen,func);
	}
	template<typename T,typename ReturnType=typename T::value_type>
	gfold<T,add<ReturnType>,ReturnType>sum(T gen,ReturnType identity=0){
		return gfold<T,add<ReturnType>,ReturnType>(identity,gen,add<ReturnType>());
	}
	template<typename T,typename ReturnType=typename T::value_type>
	gfold<T,multi<ReturnType>,ReturnType>product(T gen,ReturnType identity=1){
		return gfold<T,multi<ReturnType>,ReturnType>(identity,gen,multi<ReturnType>());
	}

	template<typename T1,typename T2>
	class gzip/*funny name*/:public generator<std::pair<typename T1::value_type,typename T2::value_type>>{
		T1 g1;
		T2 g2;
		//std::pair<typename T1::value_type,typename T2::value_type> ans;
		public:
		gzip(T1 gen1,T2 gen2) :
				g1(gen1), g2(gen2){
		}
		const std::pair<typename T1::value_type,typename T2::value_type>& operator *(){
			return std::make_pair(*g1,*g2);
		}
		void skip(){
			++g1;
			++g2;
		}
		bool is_end(){
			return g1.is_end()||g2.is_end();
		}
	};
	template<typename T1,typename T2>
	gzip<T1,T2>zip(T1 g1,T2 g2){
		return gzip<T1,T2>(g1,g2);
	}

	template<typename T>
	gfold<T,logic_and,bool>all(T gen){
		return gfold<T,logic_and,bool>(gen,logic_and(),true);
	}
	template<typename T>
	gfold<T,logic_and,bool>any(T gen){
		return gfold<T,logic_or,bool>(gen,logic_and(),false);
	}

	template<typename T>
	gfold<T,bigger<typename T::value_type>,typename T::value_type>max(T gen){
		return fold1(gen,bigger<typename T::value_type>());
	}
	template<typename T>
	gfold<T,smaller<typename T::value_type>,typename T::value_type>min(T gen){
		return fold1(gen,smaller<typename T::value_type>());
	}
	template<typename T=int>
	gfold<T,gcd<typename T::value_type>,typename T::value_type>collective_gcd(T gen){
		return fold1(gen,gcd<typename T::value_type>());
	}
	template<typename T=int>
	gfold<T,lcm<typename T::value_type>,typename T::value_type>collective_lcm(T gen){
		return fold1(gen,lcm<typename T::value_type>());
	}
	template<typename T>
	gfold<T,faverage,double>collective_lcm(T gen){
		return fold1(gen,faverage());
	}
	template<typename T>
	gfold<T,faverage,double>collective_lcm(T gen,double val,int count){
		return fold1(gen,faverage(val,count));
	}

	template<typename T,typename It=typename T::const_iterator>
	class gwrap:public generator<typename T::value_type>{
		T c;
		It p,cend;
		bool end;
		public:
		gwrap(T con) :
				c(con), p(c.begin()), cend(c.end()),end(false){
		}
		gwrap(It start,It end_) :
				p(start), cend(end_){
		}
		void skip(){
			++p;
		}
		const typename T::value_type& operator *(){
			return *p;
		}
		bool is_end(){
			return end||(end=(p==cend));
		}
	};
	template<typename T,typename It=typename T::const_iterator>
	gwrap<T,It>wrap(T con){
		return gwrap<T,It>(con);
	}
	template<typename It>
	struct __iter_base_wrap{
		typedef typename It::value_type value_type;
	};//serves as a type transmition(shrug)
	template<typename It>
	gwrap<__iter_base_wrap<It>,It>wrap(It start,It end_){
		return gwrap<__iter_base_wrap<It>,It>(start,end_);
	}

	template<typename T,typename F>
	class fexecutor{
		T g;
		F f;
		public:
		fexecutor(T gen,F func) :
				g(gen), f(func){
		}
		void exe(){
			f(*g);
		}
		void skip(){
			++g;
		}
		void operator++(){
			skip();
		}
		void operator++(int){
			skip();
		}
		void operator*(){
			exe();
		}
		bool is_end(){
			return g.is_end();
		}
	};
	template<typename T,typename F>
	fexecutor<T,F>executor(T gen,F func){
		return fexecutor<T,F>(gen,func);
	}

	template<typename T,typename Tsep=std::string>
	void gen_print(T gen,Tsep sep="",std::ostream& s=std::cout){
		while(!gen.is_end()){
			s<<(*gen)<<sep;
			++gen;
		}
	}
}

