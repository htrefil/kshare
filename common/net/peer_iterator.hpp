#ifndef COMMON_NET_PEER_ITERATOR_HPP
#define COMMON_NET_PEER_ITERATOR_HPP
#include <enet/enet.h>

#include "peer.hpp"

namespace common {
namespace net {

template<typename T>
class peer_iterator {
public:
	bool operator!=(const peer_iterator<T>& other) {
		return begin_ != other.begin_;
	}

	peer_iterator<T>& operator++() {
		do {
			begin_++;
		} while (begin_ != end_ && !is_valid(begin_));

		return *this;
	}

	peer<T>& operator*() {
		return *(peer<T>*)begin_->data;
	}

private:
	friend class host<T>;

	peer_iterator(ENetPeer* begin, ENetPeer* end) : begin_(begin), end_(end) {
		if (begin_ != end_ && !is_valid(begin_))
			++(*this);
	}

	static bool is_valid(ENetPeer* peer) {
		return peer->state == ENET_PEER_STATE_CONNECTED && peer->data != nullptr;
	}

	ENetPeer* begin_;
	ENetPeer* end_;
};

}
}

#endif