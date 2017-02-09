/*

Copyright (c) 2017, Elias Aebi
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#pragma once

namespace peg {

template <class P> struct Peg {
	P p;
public:
	Peg(const P& p): p(p) {}
	bool operator ()(const char*& s) const {
		return p(s);
	}
	// parse without modifying the input
	bool test(const char* s) const {
		return p(s);
	}
};

template <class P> auto wrap(const P& p) {
	return Peg<P>(p);
}

inline auto operator ""_p(const char* str) {
	return wrap([str](const char*& s) {
		int i;
		for (i = 0; str[i]; ++i) {
			if (s[i] != str[i]) return false;
		}
		s += i;
		return true;
	});
}

inline auto operator ""_p(char c) {
	return wrap([c](const char*& s) {
		if (s[0] == c) {
			s += 1;
			return true;
		}
		return false;
	});
}

inline auto range(char start, char end) {
	return wrap([start, end](const char*& s) {
		if (s[0] >= start && s[0] <= end) {
			s += 1;
			return true;
		}
		return false;
	});
}

inline auto empty() {
	return wrap([](const char*& s) {
		return true;
	});
}

template <class... P> class Sequence;
template <> class Sequence<> {
public:
	Sequence() {}
	bool operator ()(const char*& s) const {
		return true;
	}
};
template <class P0, class... P> class Sequence<P0, P...> {
	P0 p0;
	Sequence<P...> p;
public:
	Sequence(const P0& p0, const P&... p): p0(p0), p(p...) {}
	bool operator ()(const char*& s) const {
		const char* start = s;
		if (!p0(s)) return false;
		if (!p(s)) {
			s = start;
			return false;
		}
		return true;
	}
};
template <class... P> auto sequence(const P&... p) {
	return wrap(Sequence<P...>(p...));
}
template <class PL, class PR> auto operator +(const Peg<PL>& pl, const Peg<PR>& pr) {
	return sequence(pl, pr);
}

template <class... P> class Choice;
template <> class Choice<> {
public:
	Choice() {}
	bool operator ()(const char*& s) const {
		return false;
	}
};
template <class P0, class... P> class Choice<P0, P...> {
	P0 p0;
	Choice<P...> p;
public:
	Choice(const P0& p0, const P&... p): p0(p0), p(p...) {}
	bool operator ()(const char*& s) const {
		if (p0(s)) return true;
		return p(s);
	}
};
template <class... P> auto choice(const P&... p) {
	return wrap(Choice<P...>(p...));
}
template <class PL, class PR> auto operator |(const Peg<PL>& pl, const Peg<PR>& pr) {
	return choice(pl, pr);
}

template <class P> auto optional(const P& p) {
	return wrap([p](const char*& s) {
		p(s);
		return true;
	});
}

template <class P> auto zero_or_more(const P& p) {
	return wrap([p](const char*& s) {
		while (p(s));
		return true;
	});
}

template <class P> auto one_or_more(const P& p) {
	return wrap([p](const char*& s) {
		if (!p(s)) return false;
		while (p(s));
		return true;
	});
}

inline auto end() {
	return wrap([](const char*& s) {
		return *s == '\0';
	});
}

}
