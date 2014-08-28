#include <list>
#include <utility>
#include <cstdlib>//std::size_t
#include <iostream>//gen_print
#include "metafunc.cpp"
#include "exce.hpp"

namespace caskell {

	template<typename T>
	class generator{
		public:
		virtual const T& operator *()=0;
		virtual void skip()=0;

		typedef T value_type;

		struct __end{
		};
		__end end(){
			return __end();
		}
		virtual bool is_end(){
			return false;
		}
		bool operator==(const __end &){
			return is_end();
		}
		bool operator!=(const __end &){
			return !is_end();
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
			while(start!=end&&!is_end()){
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
		gslice &begin(){
			return *this;
		}
	};
	template<typename T>
	gslice<T>slice(T gen,int begin,int end){
		return gslice<T>(gen,begin,end);
	}
	template<typename T>
	gslice<T>take(std::size_t size,T gen){
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
		greverse &begin(){
			return *this;
		}
	};
	template<typename T>
	greverse<T>reverse(T gen){
		return greverse<T>(gen);
	}

	template<typename T,typename F>
	class gtakeWhile:public generator<typename T::value_type>{
		F f;
		T g;
		bool end;
		public:
		gtakeWhile(F func,T gen) :
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
		gtakeWhile& begin(){
			return *this;
		}
	};
	template<typename T,typename F>
	gtakeWhile<T,F>takeWhile(F func,T gen){
		return gtakeWhile<T,F>(func,gen);
	}
	template<typename T,typename F>
	T dropWhile(F func,T gen){
		while(func(*gen))
			++gen;
		return gen;
	}
	template<typename T,typename F>
	std::pair<gtakeWhile<T,F>,T> span(F func,T gen){
		return std::make_pair(takeWhile(func,gen),dropWhile(func,gen)); //implementation from hoogle(may has low efficiency)
	}

	template<typename T,typename F>
	std::pair<gtakeWhile<T,F>,T> breakf(F func,T gen){
		return std::make_pair(takeWhile(notf(func),gen),dropWhile(notf(func),gen)); //implementation from hoogle(may has low efficiency)
	}

	template<typename T>
	class ginit:public generator<typename T::value_type>{
		T g;
		std::list<typename T::value_type> l;
		public:
		ginit(int skip,T gen) :
				g(gen){
			for(int i=0;i!=skip;++i){
				l.push_back(*g);
				++g;
			}
		}
		void skip(){
			l.push_back(*g);
			++g;
			l.pop_front();
			return;
		}
		const typename T::value_type& operator *(){
			return l.front();
		}
		bool is_end(){
			return g.is_end();
		}
		ginit& begin(){
			return *this;
		}
	};
	template<typename T>
	ginit<T>dropSuffix(std::size_t n,T gen){
		return ginit<T>(n,gen);
	}
	template<typename T>
	ginit<T>init(T gen){
		return dropSuffix(1,gen);
	}

	template<typename T>
	T drop(std::size_t skip,T gen){
		for(int i=0;i!=skip;++i)
			++gen;
		return gen;
	}
	template<typename T>
	T tail(T gen){
		return drop(1,gen);
	}

	template<typename T>
	std::pair<gslice<T>,T> splitAt(std::size_t n,T gen){
		return std::make_pair(take(n,gen),drop(n,gen));
	}

	template<typename T,typename F>
	class giterate:public generator<T>{
		T d;
		F f;
		public:
		giterate(F func,T data) :
				d(data), f(func){
		}
		void skip(){
			d=f(d);
		}
		const T& operator *(){
			return d;
		}
		giterate& begin(){
			return *this;
		}
	};
	template<typename T,typename F>
	giterate<T,F>iterate(F func,T data){
		return giterate<T,F>(func,data);
	}
	template<typename T=int>
	giterate<T,inc<T>>increment(T data=T()){
		return iterate(inc<T>(),data);
	}
	template<typename T=int>
	giterate<T,dec<T>>decrement(T data=T()){
		return iterate(dec<T>(),data);
	}
	template<typename T>
	giterate<T,reflect<T>>repeat(T data){
		return iterate(reflect<T>(),data);
	}
	template<typename T>
	gslice<giterate<T,reflect<T>>>replicate(std::size_t n,T data){
		return take(n,repeat(data));
	}

	template<typename T>
	class galternation:public func<T>{
		int n1,n2;
		T v1,v2;
		int count;
		public:
		galternation(T a,int ca,T b,int cb) :
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
		galternation& begin(){
			return *this;
		}
	};
	template<typename T>
	galternation<T>alternation(T a,int ca,T b,int cb){
		return galternation<T>(a,ca,b,cb);
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
		grange &begin(){
			return *this;
		}
	};
	template<typename T,typename F=inc<T>,typename cmpF=std::less<T>>
	grange<T,F,cmpF>range(T begin,T end,F func=inc<T>(),cmpF cmpf=cmpF()){
		return grange<T,F,cmpF>(begin,end,func,cmpf);
	}
	template<typename T,typename F=inc<T>,typename cmpF=std::less<T>>
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
		gcircle& begin(){
			return *this;
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
		gconcat(std::pair<T1,T2> p){
			gconcat(p.first,p.second);
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
		gconcat& begin(){
			return *this;
		}
	};
	template<typename T1,typename T2>
	gconcat<T1,T2>concat(T1 gen1,T2 gen2){
		return gconcat<T1,T2>(gen1,gen2);
	}
	//TODO: later should implement a `concat` that can surport multi generators

	template<typename T,typename F,typename Rt=typename F::result_type>
	class gmap:public generator<Rt>/*actually, since a infinite generator never end, it could handle infinite generator as well*/{
		//This class will not check the boundary condition for efficiency
		//simply pass it to the former level of generator
		T g;
		Rt c;
		bool dirty;
		F f;
		public:
		gmap(F func,T gen) :
				g(gen), f(func){
			dirty=false;
			c=f(*g);
		}
		void skip(){
			++g;
			dirty=true;
		}
		const Rt& operator *(){
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
		gmap& begin(){
			return *this;
		}
	};
	template<typename T,typename F,typename Rt=typename F::result_type>
	gmap<T,F,Rt>map(F func,T gen){
		return gmap<T,F,Rt>(func,gen);
	}

	template<typename T,typename F>
	class gfilter:public generator<typename T::value_type>{/*dito*/
		//consider to throw a exception when the result is empty?
		//imminent death loop when encountering infinite generator
		typename T::value_type c;
		T g;
		F f;
		public:
		gfilter(F func,T gen) :
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
		gfilter& begin(){
			return *this;
		}
	};
	template<typename T,typename F>
	gfilter<T,F>filter(F func,T gen){
		return gfilter<T,F>(func,gen);
	}
	template<typename T,typename F>
	std::pair<gfilter<T,F>,gfilter<T,ffg<std::logical_not<bool>,F>>>partition(F f,T gen){
		return std::make_pair(filter(gen,f),filter(gen,notf(f)));
	}

	template<typename T,typename F,typename Rt=typename T::value_type>
	class gscanl:public generator<Rt>{
		F f;
		T g;
		Rt c;
		public:
		gscanl(F func,Rt first,T gen) :
				c(first), g(gen), f(func){
			skip();
		}
		gscanl(F func,T gen) :
				f(func), g(gen){
			c=*g;
			++g;
		}
		const Rt& operator *(){
			return c;
		}
		void skip(){
			++g;
			c=f(c,*g);
		}
		bool is_end(){
			return g.is_end();
		}
		gscanl& begin(){
			return *this;
		}
	};
	template<typename T,typename F,typename Rt=typename T::value_type>
	gscanl<T,F,Rt>scanl(F func,Rt first,T gen){
		return gscanl<T,F,Rt>(func,first,gen);
	}
	template<typename T,typename F,typename Rt=typename T::value_type>
	gscanl<T,F,Rt>scanl1(F func,T gen){
		return gscanl<T,F,Rt>(func,gen);
	}
	template<typename T,typename F,typename Rt=typename T::value_type>
	gscanl<greverse<T>,F,Rt>scanr(F func,Rt first,T gen){
		return scanl(func,first,reverse(gen));
	}
	template<typename T,typename F,typename Rt=typename T::value_type>
	gscanl<greverse<T>,F,Rt>scanr1(F func,T gen){
		return gscanl<T,F,Rt>(func,reverse(gen));
	}

	template<typename T,typename F,typename Rt=typename T::value_type>
	typename T::value_type foldl(F func,Rt first,T gen){
		return last(scanl(func,first,gen));
	}
	template<typename T,typename F,typename Rt=typename T::value_type>
	typename T::value_type foldl1(F func,T gen){
		return last(scanl1(func,gen));
	}
	template<typename T,typename F,typename Rt=typename T::value_type>
	typename T::value_type foldr(F func,Rt first,T gen){
		return last(scanr(func,first,gen));
	}
	template<typename T,typename F,typename Rt=typename T::value_type>
	typename T::value_type foldr1(F func,Rt first,T gen){
		return last(scanr1(func,gen));
	}

	template<typename T>
	typename T::value_type sum(T gen,typename T::value_type identity=0){
		return foldl(std::plus<typename T::value_type>(),identity,gen);
	}
	template<typename T>
	typename T::value_type product(T gen,typename T::value_type identity=1){
		return foldl(std::multiplies<typename T::value_type>(),identity,gen);
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
		gzip& begin(){
			return *this;
		}
	};
	template<typename T1,typename T2>
	gzip<T1,T2>zip(T1 g1,T2 g2){
		return gzip<T1,T2>(g1,g2);
	}

	template<typename T>
	bool andf(T gen){
		return foldl1(std::logical_and<bool>(),gen);
	}
	template<typename T>
	bool orf(T gen){
		return foldl1(std::logical_or<bool>(),gen);
	}
	template<typename T,typename F>
	bool all(F func,T gen){
		return andf(map(func,gen));
	}

	template<typename T,typename F>
	bool any(F func,T gen){
		return orf(map(func,gen));
	}

	template<typename T>
	typename T::value_type maximum(T gen){
		return foldl1(max<typename T::value_type>(),gen);
	}
	template<typename T>
	typename T::value_type minimum(T gen){
		return foldl1(min<typename T::value_type>(),gen);
	}
	template<typename T>
	int collective_gcd(T gen){
		return foldl1(gcd<typename T::value_type>(),gen);
	}
	template<typename T>
	int collective_lcm(T gen){
		return foldl1(lcm<typename T::value_type>(),gen);
	}
	template<typename T>
	double average(T gen){
		return foldl1(faverage(),gen);
	}
	template<typename T>
	double average(T gen,double val,int count){
		return foldl1(faverage(val,count),gen);
	}

	template<typename T>
	typename T::value_type head(T gen){
		return *gen; //imminent problem:the generator may have already reached the end
	}
	template<typename T>
	typename T::value_type last(T gen){
		typename T::value_type tmp=*gen; //dito
		do{
			tmp=*gen;
			++gen;
		}while(!gen.is_end());
		return tmp;
	}

	template<typename T,typename F>
	int findIndex(F f,T gen){
		int result=0;
		while(!(gen.is_end()||f(*gen))){
			++gen;
			++result;
		}
		if(gen.is_end()) return -1;
		return result;
	}
	template<typename T,typename F>
	typename T::value_type find(F f,T gen){
		return head(filter(f,gen));
	}
	template<typename T>
	bool elem(typename T::value_type x,T gen){
		return findIndex(curry(std::equal_to<typename T::value_type>(),x),gen)!=-1;
	}
	template<typename T>
	bool notElem(typename T::value_type x,T gen){
		return !elem(x,gen);
	}

	template<typename T>
	std::size_t length(T gen){
		std::size_t ans=0;
		while(!gen.is_end())
			++ans;
		return ans;
	}
	template<typename T>
	bool null(T gen){
		return gen.is_end();
	}

	template<typename T,typename It=typename T::const_iterator>
	class gwrap:public generator<typename T::value_type>{
		T c;
		It p,cend;
		bool end;
		public:
		gwrap(T con) :
				c(con), p(c.begin()), cend(c.end()), end(false){
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
		gwrap& begin(){
			return *this;
		}
	};
	template<typename T,typename It=typename T::const_iterator>
	gwrap<T,It>wrap(T con){
		return gwrap<T,It>(con);
	}
	template<typename It>
	struct __iter_base_wrap{
		typedef typename It::value_type value_type;
	};
	//serves as a type transmition(shrug)
	template<typename It>
	gwrap<__iter_base_wrap <It>,It>wrap(It start,It end_){
		return gwrap<__iter_base_wrap <It>,It>(start,end_);
	}

	template<typename T,typename Tsep=std::string>
	void gen_print(T gen,Tsep sep="",std::ostream& s=std::cout){
		while(!gen.is_end()){
			s<<(*gen)<<sep;
			++gen;
		}
	}
}

