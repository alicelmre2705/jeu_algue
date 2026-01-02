OUT = projet

CXX = g++
CXXFLAGS = -Wall -std=c++17

LINKING = `pkg-config --cflags gtkmm-4.0`
LDLIBS = `pkg-config --libs gtkmm-4.0`

BUILDDIR = build

CXXFILES = $(wildcard ./*.cc)

OFILES = $(patsubst ./%.cc,$(BUILDDIR)/%.o,$(CXXFILES))

all: $(OUT)

run:
	@make
	@(LC_ALL=fr_FR.UTF-8 LANG=fr_FR.UTF-8 ./$(OUT) t27.txt)

$(OUT): $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) -o $@ $(LDLIBS)

$(BUILDDIR)/%.o: ./%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@

clean:
	@echo " *** EFFACE MODULES OBJET ET EXECUTABLE ***"
	@/bin/rm -rf $(BUILDDIR) *.o *.x *.cc~ *.h~ $(OUT)
