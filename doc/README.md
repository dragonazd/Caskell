sqd at dragonazd@gmail.com

Or while you hear that PRC's blocked gmail:757887477@qq.com

#Caskell
##A haskell-style functional programming library for C++

#1.The header file to include

    generator.cpp(TODO:improve the including structure)
    
#2.Functions

	Should not directly define the class templates started with "g" or "f" (So far), unless you are intended to extend this library. So ese the functions listed below.
    
	TODO:by improving the including structure, hide the "g" and "f" templates
    
	The functions provided in section I) and section II) (in which functions are all inherited from `class generator`) will have following universial interfaces:
		1)`operator *` and `operator ->`
			Access the element (by const reference or pointer) the generator is "pointing" to.
		2)`operator ++` (both suffix and prefix) and `void skip()`
			Abandon current element and jump to next element
		3)`bool is_end()`
			For finite generator, indicates whether the last element has been generated. For infinite generator, this function always returns false
		4)`clend end` and `bool operator ==(const clend &_)`
			Same as `is_end()`. With this you can write more C++-style code such as `for(auto g=create_a_generator();g!=g::end();++g)` 
		5)`void dump(start,end)` or `void dump(STL)
			This function will fill the memory indicated. 
			CAUTION: Will no check on validity of the range provided.

	Functions provided:
	(`obj`=object,`func`=function,`gen`=generator)
##	I)Source generator
		The functions(or functors, technically speaking) in this section directly generate elements in certain pattern.

		alteration(obj,uint,obj,uint)
		circle(obj)->gen
		decrement(obj)->gen
		increment(obj)->gen
		iterate(obj)->gen
		range(obj,obj,func=inc<obj>,func=less_then<obj>)
		range(obj,func=inc<obj>,func=less_then<obj>)
		repeat(obj)->gen
		replicate(uint,obj)->gen
		wrap(STL Container)->gen
		wrap(Iterator,Iterator)->gen
		
##	II)Embellish generator
		The funtions here embellish output of other generators
		concat(gen,gen)->gen
		drop(uint,gen)->gen
		dropSuffix(uint,gen)->gen
		dropWhile(func,gen)->gen
		filter(func,gen)->gen
		init(gen)->gen
		map(func,gen)->gen
		reverse(gen)->gen
		scanl(func,obj,gen)->gen
		scanl1(func,gen)->gen
		scanr(func,obj,gen)->gen
		scanr1(func,gen)->gen
		slice(gen,uint,uint)->gen
		tail(gen)->gen
		take(uint,gen)->gen
		takeWhile(func,gen)->gen
		zip(gen,gen)->gen
		
##	III)Other
		all(func,gen)->bool
		any(func,gen)->bool
		andf(gen)->bool
		average(gen)->double
		collective_gcd(gen)->uint
		collective_lcm(gen)->uint
		elem(obj,gen)->bool
		executor(func,gen)->void(Really unfunctional! Not recommended)
		find(func,gen)->obj
		findIndex(func,gen)->uint
		foldl(func,obj,gen)->obj
		foldl1(func,gen)->obj
		foldr(func,obj,gen)->obj
		foldr1(func,gen)->obj
		gen_print(gen)->void
		head(gen)->obj
		last(gen)->obj
		length(gen)->obj
		maximum(gen)->obj
		minimum(gen)->obj
		notElem(obj,gen)->bool
		null(gen)->bool
		orf(gen)->bool
		partition(func,gen)->std::pair<gen,gen>
		product(gen,obj=1)->obj
		splitAt(uint,gen)->std::pair<gen,gen>
		sum(gen,obj=0)->obj

##	IV)Meta function
		A set of pre-defined tiny but practical functions to be paramenter of generators


#3.How to extend this library

!TODO:
!The functions to be added in
	inter(calate/sperse)
	poly
	(un)foldr(1)
	(init/tail)s
!The functions to be improved

	concat: allow multi parameter by parameter pack
	curry: dito
	n_n_switch: dito
	zip: dito

!The function to be deleted
