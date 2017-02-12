#PROFILE_OPTIONS = -pg
CXXFLAGS +=	$(PROFILE_OPTIONS) -ggdb -DDEBUG=1 -I/usr/local/include -IInclude -ISources -ISources/Xmltar
LDFLAGS +=	$(PROFILE_OPTIONS) -L /usr/local/lib -lboost_filesystem -lboost_date_time
SOURCES:=	$(wildcard Sources/*.cpp) \
					$(wildcard Sources/Xmltar/*.cpp)
EXECUTABLES:= xmltar

SOURCE_OBJECTS:=	$(SOURCES:Sources/%.cpp=Objects/%.o)
EXECUTABLE_OBJECTS:=	$(EXECUTABLES:%=Objects/%.o)
SOURCE_DEPENDS:=	$(SOURCES:Sources/%.cpp=Depends/%.d)
EXECUTABLE_DEPENDS:=	$(EXECUTABLES:%=Depends/%.d)

all:	$(EXECUTABLES)

$(EXECUTABLES):	%:Objects/%.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(SOURCE_OBJECTS) $(LDFLAGS)
	echo 'finished'

$(EXECUTABLES):	 $(SOURCE_OBJECTS)														# each executable also depends on the other object files

$(SOURCE_OBJECTS):	Objects/%.o:Depends/%.d												# each object file depends on its' own dependency file
	if [ ! -d `/usr/bin/dirname $@` ]; then /bin/mkdir -p `/usr/bin/dirname  $@`; fi
	$(CXX)  -c $(CXXFLAGS) -o $@ $(<:Depends/%.d=Sources/%.cpp)

$(EXECUTABLE_OBJECTS):	Objects/%.o:Depends/%.d
	$(CXX)  -c $(CXXFLAGS) -o $@ $(<:Depends/%.d=%.cpp)

$(SOURCE_DEPENDS):	Depends/%.d:Sources/%.cpp
	if [ ! -d `/usr/bin/dirname $@` ]; then /bin/mkdir -p `/usr/bin/dirname  $@`; fi
	$(CXX) -MM $(CXXFLAGS) -MT $(<:.cpp=.o) -MT $@ $< > $@

$(EXECUTABLE_DEPENDS):	Depends/%.d:%.cpp
	$(CXX) -MM $(CXXFLAGS) -MT $(<:.cpp=.o) -MT $@ $< > $@

include	$(SOURCE_DEPENDS) $(EXECUTABLE_DEPENDS)

.PHONEY:	clean
clean:
	$(RM) $(SOURCE_OBJECTS) $(EXECUTABLE_OBJECTS) \
		$(SOURCE_DEPENDS) $(EXECUTABLE_DEPENDS) \
		$(EXECUTABLES)
