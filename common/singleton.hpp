#ifndef COMMON_SINGLETON_HPP
#define COMMON_SINGLETON_HPP

namespace common {

template<typename T>
class singleton {
public:
	static T& get() {
		static T instance;
		
		return instance;
	}
};

}

#endif