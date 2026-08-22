########################################################################
######################## Venenum Makefile ##############################
########################################################################

# Compiler settings.
CXX = g++
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

.DEFAULT_GOAL := all
.PHONY: all release run clean help

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

## Remove all build and binary output.
clean:
	rm -rf build bin

## List available targets.
help:
	@echo "Venenum Makefile targets:"
	@echo "  make               Build debug binary (-g -O0, ASan/UBSan) at bin/debug/$(APPNAME)"
	@echo "  make release       Build optimized binary (-O2 -DNDEBUG) at bin/release/$(APPNAME)"
	@echo "  make run           Build and run the binary (add BUILD=release for the optimized build)"
	@echo "  make clean         Remove build/ and bin/"
	@echo "  make help          Show this message"
	@echo ""
	@echo "BUILD can also be set directly, e.g.: make BUILD=release"
