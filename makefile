export LIBP = $(CURDIR)
export LIBP_CXX_LANG ?= 20
export CXXFLAGS_LANG ?= -std=c++$(LIBP_CXX_LANG)
export CPPFLAGS_LANG ?= $(CXXFLAGS_LANG)

export EXTERNAL = $(CURDIR)/external

# The MATH variable is documented by external/math/make/standalone.
export MATH = $(CURDIR)/external/math/

default :
	cd external && $(MAKE)
	cd test && $(MAKE) test

clean :
	cd external && $(MAKE) clean
	cd test && $(MAKE) clean

clean-all :
	cd external && $(MAKE) clean-all
	cd test && $(MAKE) clean-all
