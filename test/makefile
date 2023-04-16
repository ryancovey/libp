default : test

-include $(LIBP)/libp.make
-include $(EXTERNAL)/math.make

LIBPTESTOBJECTS = test.o interval_test.o

ifndef STAN_MPI
	BOOST_LIBRARY_ABSOLUTE_PATH = $(abspath $(BOOST)/stage/lib)
	LDFLAGS += -Wl,-L,"$(BOOST_LIBRARY_ABSOLUTE_PATH)" -Wl,-rpath,"$(BOOST_LIBRARY_ABSOLUTE_PATH)"
endif

LDLIBS += $(wildcard $(BOOST_LIBRARY_ABSOLUTE_PATH)/*.so)

# See "Compiling C++ programs" and "Linking a single object file" subsections of
# "https://www.gnu.org/software/make/manual/make.html#Catalogue-of-Rules".
test : $(LIBPTESTOBJECTS) $(BOOST_LIBRARY_ABSOLUTE_PATH)/libboost_unit_test_framework.so
	$(CXX) $(LDFLAGS) $(LIBPTESTOBJECTS) $(LDLIBS) -o test

# See ../external/math/make/libraries ifdef STAN_MPI block for stan boost dependencies.
# We aim for our use of boost test to be compatible and similar to stan's use of boost mpi
# and boost serialisation. We compile the shared library to avoid violating the one definition
# rule for boost dependencies that are shared between stan and boost test.
$(BOOST_LIBRARY_ABSOLUTE_PATH)/libboost_unit_test_framework.so :
	cd $(BOOST) && ./bootstrap.sh
	cd $(BOOST) && ./b2 --layout=system --with-test variant=release link=shared threading=multi runtime-link=shared hardcode-dll-paths=true dll-path="$(BOOST_LIBRARY_ABSOLUTE_PATH)" cxxstd=20

clean :
	$(RM) -f test *.o \
	$(BOOST_LIBRARY_ABSOLUTE_PATH)/libboost_prg_exec_monitor.*\
	$(BOOST_LIBRARY_ABSOLUTE_PATH)/libboost_test_exec_monitor.*\
	$(BOOST_LIBRARY_ABSOLUTE_PATH)/libboost_unit_test_framework.*

clean-all : clean

.PHONY : clean clean-all
