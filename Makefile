########################################################################
######################## Venenum Makefile ##############################
########################################################################

# Compiler settings. Override with `make CXX=clang++` etc.
CXX ?= g++
WARNFLAGS = -Wall -Weffc++ -Wextra -Wsign-conversion -Werror -pedantic-errors
CXXSTD = -std=c++23
# The rook/bishop fancy-magic attack tables (src/attack.h) are built at
# compile time by a consteval function; Apple Clang's default constexpr
# step limit (1,048,576) is too low for that computation, so it's raised
# here. This flag is Clang-specific (GCC's equivalent, -fconstexpr-ops-limit,
# already defaults far higher) -- this project currently only builds with
# Apple Clang (CXX=g++ resolves to it on this toolchain).
CONSTEXPRFLAGS = -fconstexpr-steps=100000000

# Build type: debug (default) or release. Override with `make BUILD=release`.
BUILD ?= debug

ifeq ($(BUILD),debug)
    OPTFLAGS = -g -O0 -fsanitize=address,undefined
else ifeq ($(BUILD),release)
    OPTFLAGS = -O2 -DNDEBUG
else
    $(error Unknown BUILD '$(BUILD)': expected 'debug' or 'release')
endif

CXXFLAGS = $(CXXSTD) $(WARNFLAGS) $(CONSTEXPRFLAGS) $(OPTFLAGS)

# Project layout.
APPNAME = Venenum
SRCDIR = src
BUILDDIR = build/$(BUILD)
BINDIR = bin/$(BUILD)

# On Windows the linked binary needs a .exe suffix; the rest of this Makefile
# assumes a Unix-like shell (Git Bash/MSYS2/WSL) providing mkdir -p and rm -rf.
ifeq ($(OS),Windows_NT)
    EXE = .exe
else
    EXE =
endif

TARGET = $(BINDIR)/$(APPNAME)$(EXE)

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
DEP = $(OBJ:.o=.d)

# Test suite layout. Kept separate from src/ so SRC's wildcard doesn't sweep
# test files into the main binary (which would also collide main()s with
# venenum.cpp's).
TESTDIR = tests
TESTBUILDDIR = build/test-$(BUILD)
TESTBINDIR = bin/test-$(BUILD)
TESTTARGET = $(TESTBINDIR)/$(APPNAME)Tests$(EXE)

# Everything src/*.cpp builds under, minus -Weffc++: the vendored doctest.h's
# macro-generated types don't satisfy it, and no third-party header realistically does.
TESTWARNFLAGS = -Wall -Wextra -Wsign-conversion -Werror -pedantic-errors
TESTCXXFLAGS = $(CXXSTD) $(TESTWARNFLAGS) $(CONSTEXPRFLAGS) $(OPTFLAGS) -I$(SRCDIR)

TESTSRC = $(wildcard $(TESTDIR)/*.cpp)
TESTOBJ = $(TESTSRC:$(TESTDIR)/%.cpp=$(TESTBUILDDIR)/%.o)
TESTDEP = $(TESTOBJ:.o=.d)

# venenum.cpp's main() must not be linked into the test binary.
MAINOBJ = $(BUILDDIR)/venenum.o
ENGINEOBJ = $(filter-out $(MAINOBJ),$(OBJ))

.DEFAULT_GOAL := all
.PHONY: all release run test test-release clean help

## Build the debug binary (default), or whatever $(BUILD) is set to.
all: $(TARGET)

## Convenience alias for `make BUILD=release`.
release:
	$(MAKE) BUILD=release

$(TARGET): $(OBJ) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compiling also emits a matching .d file (via -MMD -MP) so editing a header
# correctly triggers a rebuild of every .o that includes it.
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR) $(BINDIR):
	mkdir -p $@

-include $(DEP)

## Build (if needed) and run the engine.
run: $(TARGET)
	./$(TARGET)

## Build (if needed) and run the test suite (add BUILD=release for the optimized build).
test: $(TESTTARGET)
	./$(TESTTARGET)

## Convenience alias for `make BUILD=release test`.
test-release:
	$(MAKE) BUILD=release test

$(TESTTARGET): $(ENGINEOBJ) $(TESTOBJ) | $(TESTBINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TESTBUILDDIR)/%.o: $(TESTDIR)/%.cpp | $(TESTBUILDDIR)
	$(CXX) $(TESTCXXFLAGS) -MMD -MP -c $< -o $@

$(TESTBUILDDIR) $(TESTBINDIR):
	mkdir -p $@

-include $(TESTDEP)

## Remove all build and binary output.
clean:
	rm -rf build bin

## List available targets.
help:
	@echo "Venenum Makefile targets:"
	@echo "  make               Build debug binary (-g -O0, ASan/UBSan) at bin/debug/$(APPNAME)"
	@echo "  make release       Build optimized binary (-O2 -DNDEBUG) at bin/release/$(APPNAME)"
	@echo "  make run           Build and run the binary (add BUILD=release for the optimized build)"
	@echo "  make test          Build and run the test suite (add BUILD=release for the optimized build)"
	@echo "  make test-release  Convenience alias for 'make BUILD=release test'"
	@echo "  make clean         Remove build/ and bin/"
	@echo "  make help          Show this message"
	@echo ""
	@echo "BUILD can also be set directly, e.g.: make BUILD=release"
